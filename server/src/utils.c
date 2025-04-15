#include"utils.h"

t_log* logger;

int iniciar_servidor(void)
{
	int socket_servidor;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	socket_servidor = getaddrinfo(NULL, PUERTO, &hints, &servinfo);

	// Creamos el socket de escucha del servidor socket()

	int fd_escucha = socket(server_info->ai_family,
							server_info->ai_socktype,
							server_info->ai_protocol);
						
	// Asociamos el socket a un puerto bind()
	socket_servidor = setsockopt(fd_escucha, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
	socket_servidor = bind (fd_escucha, servinfo->ai_addr, servinfo->ai_addrlen);

	// Escuchamos las conexiones entrantes listen()
	socket_servidor = listen(fd_escucha, SOMAXCONN);

	freeaddrinfo(servinfo);
	log_trace(logger, "Listo para escuchar a mi cliente");

	return socket_servidor;
}

int esperar_cliente(int socket_servidor)
{
	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

void checkeoHandshake(int handshake, int socket_cliente){
	int handshake = recibir_operacion(socket_cliente);
	int handshake_ok = 0;
	int handshake_error = -1;

	if (handshake == handshake_error){
		log_error(logger, "El protocolo no es el esperado. Intenalo de nuevo");
		send(socket_cliente, &handshake_error, sizeof(int), 0);
		handshake = recibir_operacion(socket_cliente);	
	}else if(handshake == handshake_ok){
		log_info(logger, "Handshake correcto. El cliente es el esperado.");
		send(socket_cliente, &handshake_ok, sizeof(int), 0);
	}		
}

int recibir_operacion(int socket_cliente) 
{
	int cod_op;
	if(recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente); 
		return -1;
	}
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

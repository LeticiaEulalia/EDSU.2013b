/* Práctica: 	EDSU.2013b
 * Curso:	2013-2014
 * Autor:	080108:Gutierrez:Ramirez:Javier 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "editor.h"
#include "comun.h"
#include "edsu_comun.h"

int generar_evento(const char *tema, const char *valor) {
	int sfd;

	struct mensaje mnsjE, mnsjR;
	struct evento nuevoE;
	ssize_t aux;

	/* Recoger parametros por variables de entorno.*/
	char* servidor = getenv("SERVIDOR");
	char* puerto = getenv("PUERTO");

	/* Establecer la conexion.*//*
	if ((error = realizarConexion(servidor, puerto, &sfd)) != 0) {
		fprintf(stderr, "Error al realizar la conexion %d\n", error);
		exit(EXIT_FAILURE);
	}*/

	struct sockaddr_in dir;
	struct hostent *host_info;	
	if ((sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("Error creacion socket\n");
		return -1;
	}

	bzero(&dir, sizeof(dir));	
	if((host_info = gethostbyname(servidor)) < 0){
		perror("Variable de entorno SERVIDOR, no definida\n");
		return -1;
	}
	memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
	if((dir.sin_port = htons(atoi(puerto))) < 0){
		perror("Variable de entorno PUERTO, no definida\n");
		//return -1;
	}
	memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
	dir.sin_family = PF_INET;

	if (connect(sfd, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		perror("Error conexion socket");
		close(sfd);
		return -1;
	}

	/* Crear mensaje.*/
	mnsjE.tipo = eventoGenerado;
	mnsjE.tamT = strlen(tema);
	mnsjE.tamV = strlen(valor);

	nuevoE.tema = tema;
	nuevoE.valor = valor;

	/* Enviar del mensaje.*/
	aux = write(sfd, &mnsjE, sizeof(mnsjE));
	if ((aux == 0) || (aux == -1)) {
		fprintf(stderr, "editor: Error al hacer el write\n");
		exit(EXIT_FAILURE);
	}

	/* Envio del tema.*/
	aux = write(sfd, &nuevoE.tema, mnsjE.tamT);
	if ((aux == 0) || (aux == -1)) {
		fprintf(stderr, "editor: Error al hacer el write\n");
		exit(EXIT_FAILURE);
	}

	/* Envio del valor.*/
	aux = write(sfd, &nuevoE.valor, mnsjE.tamV);
	if ((aux == 0) || (aux == -1)) {
		fprintf(stderr, "editor: Error al hacer el write\n");
		exit(EXIT_FAILURE);
	}

	/* Confirmacion.*/
	aux = read(sfd, &mnsjR, sizeof(mnsjR));
	if ((aux > 0) && (mnsjR.tipo == OK)) {
		close(sfd);
		return 0;
	}
	else {
		fprintf(stderr, "editor: Error al hacer el read\n");
		exit(EXIT_FAILURE);
	}

	/* Cerrar descriptor.*/
	close(sfd);
	return EXIT_FAILURE;
}

/* solo para la version avanzada */
int crear_tema(const char *tema) {
	int sfd;

	struct mensaje mnsjE, mnsjR;
	struct evento nuevoE;
	ssize_t aux;

	/* Recoger parametros por variables de entorno.*/
	char* servidor = getenv("SERVIDOR");
	char* puerto = getenv("PUERTO");

	/* Comprobar las variables de entorno.*/

	/* Establecer la conexion.*//*
	if ((error = realizarConexion(servidor, puerto, &sfd)) != 0) {
		fprintf(stderr, "Error al realizar la conexion %d\n", error);
		exit(EXIT_FAILURE);
	}*/

	struct sockaddr_in dir;
	struct hostent *host_info;	
	if ((sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("Error creacion socket\n");
		return -1;
	}

	bzero(&dir, sizeof(dir));	
	if((host_info = gethostbyname(servidor)) < 0){
		perror("Variable de entorno SERVIDOR, no definida\n");
		return -1;
	}
	memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
	if((dir.sin_port = htons(atoi(puerto))) < 0){
		perror("Variable de entorno PUERTO, no definida\n");
		//return -1;
	}
	memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
	dir.sin_family = PF_INET;

	if (connect(sfd, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		perror("Error conexion socket");
		close(sfd);
		return -1;
	}

	/* Crear en mensaje.*/
	mnsjE.tipo = crearTema;
	mnsjE.tamT = strlen(tema);
	mnsjE.tamV = strlen("");

	nuevoE.tema = tema;

	/* Enviar de la cabecera.*/
	aux = write(sfd, &mnsjE, sizeof(mnsjE));
	if ((aux == 0) || (aux == -1)) {
		fprintf(stderr, "editor: Error al hacer el write\n");
		exit(EXIT_FAILURE);
	}

	/* Envio del tema.*/
	aux = write(sfd, &nuevoE.tema, mnsjE.tamT);
	if ((aux == 0) || (aux == -1)) {
		fprintf(stderr, "editor: Error al hacer el write\n");
		exit(EXIT_FAILURE);
	}

	/* Confirmacion.*/
	aux = read(sfd, &mnsjR, sizeof(mnsjR));
	if ((aux > 0) && (mnsjR.tipo == OK)) {
		close(sfd);
		return 0;
	}
	else {		
		exit(EXIT_FAILURE);
	}

	close(sfd);
	return EXIT_FAILURE;
}

/* solo para la version avanzada */
int eliminar_tema(const char *tema) {
	int sfd;

	struct mensaje mnsjE, mnsjR;
	struct evento nuevoE;
	ssize_t aux;

	/* Recoger parametros por variables de entorno.*/
	char* servidor = getenv("SERVIDOR");
	char* puerto = getenv("PUERTO");

	/* Comprobar las variables de entorno.*/

	/* Establecer la conexion.*//*
	if ((error = realizarConexion(servidor, puerto, &sfd)) != 0) {
		fprintf(stderr, "Error al realizar la conexion %d\n", error);
		exit(EXIT_FAILURE);
	}*/

	struct sockaddr_in dir;
	struct hostent *host_info;	
	if ((sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("Error creacion socket\n");
		return -1;
	}

	bzero(&dir, sizeof(dir));	
	if((host_info = gethostbyname(servidor)) < 0){
		perror("Variable de entorno SERVIDOR, no definida\n");
		return -1;
	}
	memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
	if((dir.sin_port = htons(atoi(puerto))) < 0){
		perror("Variable de entorno PUERTO, no definida\n");
		//return -1;
	}
	memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
	dir.sin_family = PF_INET;

	if (connect(sfd, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		perror("Error conexion socket");
		close(sfd);
		return -1;
	}

	/* Crear en mensaje.*/
	mnsjE.tipo = eliminarTema;
	mnsjE.tamT = strlen(tema);
	mnsjE.tamV = strlen("");

	nuevoE.tema = tema;

	/* Enviar de la cabecera.*/
	aux = write(sfd, &mnsjE, sizeof(mnsjE));
	if ((aux == 0) || (aux == -1)) {
		fprintf(stderr, "editor: Error al hacer el write\n");
		exit(EXIT_FAILURE);
	}

	/* Envio del tema.*/
	aux = write(sfd, &nuevoE.tema, mnsjE.tamT);
	if ((aux == 0) || (aux == -1)) {
		fprintf(stderr, "editor: Error al hacer el write\n");
		exit(EXIT_FAILURE);
	}

	/* Confirmacion.*/
	aux = read(sfd, &mnsjR, sizeof(mnsjR));
	if ((aux > 0) && (mnsjR.tipo == OK)) {
		close(sfd);
		return 0;
	}
	else {		
		exit(EXIT_FAILURE);
	}

	close(sfd);
	return EXIT_FAILURE;
}

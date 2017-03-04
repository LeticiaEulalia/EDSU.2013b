#include "subscriptor.h"
#include "comun.h"
#include "edsu_comun.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

int  puertoSub = 0;
void (*notificacion_evento)(const char*,const char*);

int alta_subscripcion_tema(const char *tema) {
	int sfd;

	struct sockaddr_in dir, dirDest;
	struct hostent *host_info;
	char hostname[1024];

	struct mensaje mnsjE, mnsjR;
	struct evento eventoAlta;

	char *servidor = getenv("SERVIDOR");
	char *puerto = getenv("PUERTO");

	if (notificacion_evento == NULL) {
		exit(EXIT_FAILURE);
	}

	if (servidor < 0) { 
		fprintf(stderr, "Error: Variable SERVIDOR no definida.\n");
		exit(EXIT_FAILURE);
	}

	if (atoi(puerto) < 0) { 
		fprintf(stderr, "Error: Variable PUERTO no definida.\n");
		exit(EXIT_FAILURE);
	}

	bzero(&dir, sizeof(dir));

	host_info = gethostbyname(servidor);
	memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);	
	dir.sin_port = htons(atoi(puerto)); 
	dir.sin_family = PF_INET;

	bzero(&dirDest, sizeof(dirDest));

	hostname[1023] = '\0';
	gethostname(hostname, 1024);
	host_info = gethostbyname(hostname);	
	memcpy(&dirDest.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
	dirDest.sin_port = htons(puertoSub);
	dirDest.sin_family = PF_INET;

	if ((sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "Error creacion socket\n");
		exit(EXIT_FAILURE);
	}
	
	if (connect(sfd, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		fprintf(stderr, "Error conexion socket\n");
		exit(EXIT_FAILURE);
	}

	mnsjE.tipo = altaSubscripcion;
	mnsjE.tamT = strlen(tema);
	mnsjE.tamV = 0;

	eventoAlta.tema = tema;

//	fprintf (stdout, "Peticion de alta en el tema %s.\n",tema);
	
	write(sfd, &mnsjE, sizeof(mnsjE));	
	write(sfd, eventoAlta.tema, mnsjE.tamT);
	write(sfd, &dirDest, sizeof(dirDest));

//	fprintf (stdout, "Esperando respuesta...\n");

	read (sfd, &mnsjR, sizeof(mnsjR));
	if (mnsjR.tipo != OK) {
		fprintf (stdout, "Error: Petición de alta a un tema inexistente.\n"); 
		exit(EXIT_FAILURE);
	}

	close(sfd);
	return 0;
}

int baja_subscripcion_tema(const char *tema) {
	int sfd;

	struct sockaddr_in dir, dirDest;
	struct hostent *host_info;
	char hostname[1024];

	struct mensaje mnsjE, mnsjR;
	struct evento eventoAlta;

	char *servidor = getenv("SERVIDOR");
	char *puerto = getenv("PUERTO");

	if (notificacion_evento == NULL) {
		exit(EXIT_FAILURE);
	}

	if (servidor < 0) { 
		fprintf(stderr, "Error: Variable SERVIDOR no definida.\n");
		exit(EXIT_FAILURE);
	}

	if (atoi(puerto) < 0) { 
		fprintf(stderr, "Error: Variable PUERTO no definida.\n");
		exit(EXIT_FAILURE);
	}

	bzero(&dir, sizeof(dir));

	host_info = gethostbyname(servidor);
	memcpy(&dir.sin_addr.s_addr, host_info->h_addr, host_info->h_length);	
	dir.sin_port = htons(atoi(puerto)); 
	dir.sin_family = PF_INET;

	bzero(&dirDest, sizeof(dirDest));

	hostname[1023] = '\0';
	gethostname(hostname, 1024);
	host_info = gethostbyname(hostname);	
	memcpy(&dirDest.sin_addr.s_addr, host_info->h_addr, host_info->h_length);
	dirDest.sin_port = htons(puertoSub);
	dirDest.sin_family = PF_INET;

	if ((sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "Error creacion socket\n");
		exit(EXIT_FAILURE);
	}
	
	if (connect(sfd, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		fprintf(stderr, "Error conexion socket\n");
		exit(EXIT_FAILURE);
	}

	mnsjE.tipo = bajaSubscripcion;
	mnsjE.tamT = strlen(tema);
	mnsjE.tamV = 0;

	eventoAlta.tema = tema;

//	fprintf (stdout, "Peticion de baja en el tema %s.\n", tema);
	
	write(sfd, &mnsjE, sizeof(mnsjE));	
	write(sfd, eventoAlta.tema, mnsjE.tamT);
	write(sfd, &dirDest, sizeof(dirDest));

//	fprintf (stdout, "Esperando respuesta...\n");

	read (sfd, &mnsjR, sizeof(mnsjR));
	if (mnsjR.tipo != OK) {
		fprintf (stdout, "Error: Petición de baja a un tema inexistente.\n"); 
		exit(EXIT_FAILURE);
	}

	close(sfd);
	return 0;
}

int inicio_subscriptor(void (*notif_evento)(const char *, const char *),
                void (*alta_tema)(const char *),
                void (*baja_tema)(const char *)) {
	int sfd;
	unsigned int t = sizeof(dir);
	struct sockaddr_in dir;
	pthread_t proceso;
	pthread_attr_t atrib;
		
	//if (notif_evento != NULL){ 
		notificacion_evento = notif_evento;
	/*}else{
		return -1;
	}*/
	
	if ((sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		fprintf(stderr, "Error creando socket\n");
		exit(EXIT_FAILURE);
	}

	bzero(&dir, sizeof(dir));
	dir.sin_addr.s_addr = INADDR_ANY;
	dir.sin_port = htons(puertoSub);
	dir.sin_family = PF_INET;	
	
	if (bind(sfd, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		fprintf(stderr, "Error al hacer el bind\n");
		exit(EXIT_FAILURE);
	}

	if (listen(sfd, 5) < 0) {
		fprintf(stderr, "Error al hacer el listen\n");
		exit(EXIT_FAILURE);
	}

	if (getsockname(sfd, (struct sockaddr *)&dir, &t) < 0) {
		fprintf(stderr, "Error al hacer el getsockname\n");
		exit(EXIT_FAILURE);
	}

	puertoSub = ntohs(dir.sin_port);
	pthread_attr_init(&atrib);
	pthread_attr_setdetachstate(&atrib, PTHREAD_CREATE_JOINABLE);
	pthread_create(&proceso, &atrib, recibirNotif, (void *)sfd);	

	return 0;
}

int fin_subscriptor() {
	return 0;
}

void* recibirNotif(void *arg) {

	int sfd = (int)arg;
	int cfd;
	unsigned int tamDir;
	char *tema,*valor;

	struct sockaddr_in dirC;
	struct mensaje mnsjR;

	while (1) {

		tamDir = sizeof(dirC);
		if ((cfd = accept(sfd, (struct sockaddr *)&dirC, &tamDir))<0){
			fprintf(stderr, "Error accept\n");
			close(sfd);
		}
			
		read(cfd, &mnsjR, sizeof(mnsjR));

		tema = calloc (0, mnsjR.tamT);
		valor = calloc (0, mnsjR.tamV);
		
//		printf("\nNuevo evento publicado:\n");
		
		if (mnsjR.tipo == eventoGenerado) {
			read(cfd, tema, mnsjR.tamT);
//			fprintf(stdout, "Tema: %s\n", tema);
			read(cfd, valor, mnsjR.tamV);
//			fprintf(stdout, "Valor: %s\n",valor);
			notificacion_evento(tema, valor);
		}
		close(cfd);
	}
}

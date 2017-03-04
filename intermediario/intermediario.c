#include "comun.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>

#define TAM_MAX 32
#define LONG_MAX 300
#define SUBS 1024

char temas[TAM_MAX][LONG_MAX];
char *tema_;
char *valor_;

typedef struct subscriptores {
	struct sockaddr_in dirSubscritos [TAM_MAX][SUBS];
	int suscritos[SUBS];
} subscriptores;

int encuentra(char [TAM_MAX][LONG_MAX],char *, int );
void inicializa(void);

int main(int argc, char *argv[]) {
	int sfd, fdsock, puerto, i, j, p, sockEnvSub;
	int ntemas = 0;

	struct sockaddr_in dir, dirC;
	socklen_t tamDir;
	struct sockaddr_in dirSubs;
	subscriptores listaSubs;
	mensaje mnsj, mnsjR, msgEnvSub;
	FILE *fd;

	if (argc!=3) {
		fprintf(stderr, "Uso: %s puerto fichero_temas\n", argv[0]);
		return 1;
	}

	puerto = atoi(argv[1]);
	if ((fd = fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "No se puede leer el fichero de temas o inexistente\n");
		exit(EXIT_FAILURE);
	}

	for (i = 0; (i < TAM_MAX) && (fgets(temas[i], 60, fd) != NULL); i++){
 		ntemas++;
	}

	fclose(fd);


	if ((sfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("Error creacion socket\n");
		exit(EXIT_FAILURE);
	}

	bzero(&dir, sizeof(dir));
	dir.sin_addr.s_addr = INADDR_ANY;
	dir.sin_port = htons(puerto);
	dir.sin_family = PF_INET;
	
	if (bind(sfd, (struct sockaddr *)&dir, sizeof(dir)) < 0) {
		perror("Error bind\n");
		close(sfd);
		exit(EXIT_FAILURE);
	}

	printf("Esperando eventos (ed), altas o bajas en temas (subs)...\n");
	if (listen(sfd, 5) < 0) {
		perror("Error listen\n");
		close(sfd);
		exit(EXIT_FAILURE);
	}

	//Inicializar el tema y valor de los eventos que recibe el intermediario
	inicializa();	

	while(1){

		tamDir = sizeof(dirC);
		if ((fdsock = accept(sfd, (struct sockaddr *)&dirC, &tamDir)) < 0){
			perror("Error accept\n");
			close(sfd);
			exit(EXIT_FAILURE);
		}

		read(fdsock, &mnsj, sizeof(mnsj)); 

 		switch (mnsj.tipo){
			case eventoGenerado:
				if (read(fdsock, tema_, mnsj.tamT) < 0){
					printf("Error al leer el tema.\n"); 
					exit(EXIT_FAILURE);
				}else{
					printf("Tema: %s\n", tema_);
				}

				if (read(fdsock, valor_, mnsj.tamV) < 0){
					printf("Error al leer el valor.\n"); 
					exit(EXIT_FAILURE);
				}else{
					printf("Valor: %s\n",valor_);
				}	
				
				if ((p = encuentra(temas, tema_, ntemas)) != -1) {
					bzero(&mnsjR, sizeof(mnsjR));
					mnsjR.tipo = OK;
					write(fdsock, &mnsjR, sizeof(mnsjR));

					mnsjR.tipo = eventoGenerado;
					mnsjR.tamT = strlen(tema_);
					mnsjR.tamV = strlen(valor_);
						
					for (j = 0; j < listaSubs.suscritos[p]; j++) {
						if ((sockEnvSub = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
							perror("Error creacion socket\n");
							exit(EXIT_FAILURE);
						}
			
						if ((connect(sockEnvSub, (struct sockaddr *)&listaSubs.dirSubscritos[p][j],
							            sizeof(listaSubs.dirSubscritos[p][j]))) < 0) {
								perror("Error conexion socket\n");
								exit(EXIT_FAILURE);
						}
						
						printf("Difundiendo evento del tema %s y valor %s...\n", tema_, valor_);
						write(sockEnvSub, &mnsj, sizeof(mnsj));
						write(sockEnvSub, tema_, mnsj.tamT);
						write(sockEnvSub, valor_, mnsj.tamV);
						
						close(sockEnvSub);
					}
				}else {
					printf("Error: tema no encontrado.\n");
					mnsjR.tipo = eventoGenerado;
					write(fdsock, &mnsjR, sizeof(mnsjR));
					exit(EXIT_FAILURE);						
				}
				bzero(tema_, strlen(tema_));
				bzero(valor_, strlen(valor_));
				break;

			case altaSubscripcion:
				if (read(fdsock, tema_, mnsj.tamT) < 0){ 
					printf("Error leyendo tema.\n"); 
					exit(EXIT_FAILURE);
				}else{
					printf("Petición de alta recibida en el tema: %s\n", tema_);
				}

				if ((p = encuentra(temas, tema_, ntemas)) != -1) {
					printf("Tema correcto\n");
					read(fdsock, &dirSubs, sizeof(dirSubs));
					listaSubs.dirSubscritos[p][listaSubs.suscritos[p]] = dirSubs;
					listaSubs.suscritos[p]++;

					printf("Emitiendo respuesta de alta correcta\n");
					mnsjR.tipo = RA;
					mnsjR.tamT = 0;
					mnsjR.tamV = 0;
					write(fdsock, &mnsjR, sizeof(mnsjR));

				} else {
					printf("Error: tema no encontrado.\n");	
					mnsjR.tipo = eventoGenerado;
					mnsjR.tamT = 0;
					mnsjR.tamV = 0;
					write(fdsock, &mnsjR, sizeof(mnsjR));	
					exit(EXIT_FAILURE);
				}
				bzero(tema_, strlen(tema_));
				bzero(valor_, strlen(valor_));
				break;

			case bajaSubscripcion:
				if (read(fdsock, tema_, mnsj.tamT) < 0){ 
					printf("Error leyendo tema.\n"); 
					exit(EXIT_FAILURE);
				} else {
					printf("Recibida petición de baja para el tema: %s\n",tema_);
				}
				
				if ((p = encuentra(temas, tema_, ntemas)) != -1) {
						
					printf("Tema correcto\n");	
					listaSubs.suscritos[p]--; 
					bzero(&listaSubs.dirSubscritos[p][listaSubs.suscritos[p]], sizeof(listaSubs.dirSubscritos[p][listaSubs.suscritos[p]]));

					printf("Emitiendo respuesta de baja correcta\n");						
					mnsjR.tipo = RB;
					mnsjR.tamT = 0;
					mnsjR.tamV = 0;
					write(fdsock, &mnsjR, sizeof(mnsjR));
				} else {
					printf("Error: tema no encontrado.\n");	
					mnsjR.tipo = ERROR;
					mnsjR.tamT = 0;
					mnsjR.tamV = 0;
					write(fdsock, &mnsjR, sizeof(mnsjR));
					exit(EXIT_FAILURE);
				}			
				bzero(tema_, strlen(tema_));
				bzero(valor_, strlen(valor_));
				break;
		}
	}

	close(sfd);
	return 0;
}

int encuentra(char lista_temas[TAM_MAX][LONG_MAX],char *paramTema, int num_temas){

		int pos = 0,encontrado = 0;	
		while ((pos < num_temas) && !encontrado){
			
			if (strncmp(lista_temas[pos], paramTema, strlen(paramTema)) == 0){
 				encontrado = 1;
			}else{ 
				pos++;
			}		
		}//end while
		if (!encontrado) pos = -1;

		return pos;
}

void inicializa (void){
	mensaje msj;
	tema_ = (char *) calloc(0, msj.tamT);
	valor_ = (char *) calloc(0, msj.tamV);
}

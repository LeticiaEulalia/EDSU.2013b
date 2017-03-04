/*
   Incluya en este fichero todas las implementaciones que pueden
   necesitar compartir los módulos editor y subscriptor,
   si es que las hubiera.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "edsu_comun.h"

int realizarConexion(const char* servidor, const char* puerto, int* psfd) {
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int s;

	/* Obtener direccion.*/
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;	/* Stream socket */
	hints.ai_flags = 0;
	hints.ai_protocol = IPPROTO_TCP;	/* TCP */

	s = getaddrinfo(servidor, puerto, &hints, &result);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* Se prueban las posibles direcciones hasta conectarse.*/
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		*psfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
		if (*psfd == -1) {
			continue;		/* Error al crear el socket.*/
		}

		if (connect(*psfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;			/* Success */
		}

		close(*psfd);
	}

	/* Ninguna direccion ha sido valida.*/
        if (rp == NULL) { 		
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}

	/* Se libera la estructura con las conexiones.*/
	freeaddrinfo(result);

	return 0;
}

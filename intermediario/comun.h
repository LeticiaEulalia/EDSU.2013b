/*
   Incluya en este fichero todas las definiciones que pueden
   necesitar compartir todos los módulos (editor, subscriptor y
   proceso intermediario), si es que las hubiera.
*/

typedef enum tipoMnsj {
	eventoGenerado,
	altaSubscripcion,
	bajaSubscripcion,
	OK,
	crearTema,
	eliminarTema
} tipoMnsj;

typedef struct mensaje {
	int tipo;
	int tamT;
	int tamV;
} mensaje;

typedef struct evento{
	char *tema;
	char *valor;
} evento;

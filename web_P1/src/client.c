/** 
* @brief Definición del modulo cliente, cliente TCP
*  implementado.
* 
* @file client.c
* @author John Ander Rolingson Avellanal \
*  Diego Núñez Garcia
* @version 2.0 
* @date 06-03-2024
* @copyright GNU Public License
*/

#include <sys/types.h>
#include <unistd.h>
#include "net_lvl.h"
#include "utils.h"

/**
* @brief Punto de entrada principal del programa.
*
* @param argc Número de argumentos de línea de comandos
* @param argv Array de argumentos de línea de comandos
*
* @return Código de estado de la operación
*/
int main(int argc, char *argv[]) {
    int socket_descc = 0;
    char buffer[BUFF_SIZE], rec[BUFF_SIZE];
    
    if (argc < 2) {
        printf("Faltan argumentos:\n-Puerto\n");
        return -1;
    }

    if ((socket_descc = client_socket_init(atoi(argv[1]))) == -1) return -1;

    printf("Introduzca el mensaje a enviar: "); fgets(buffer, BUFF_SIZE, stdin);

    send(socket_descc, buffer, (socklen_t)sizeof(buffer), 0);

    recv(socket_descc, (void*)&rec, (socklen_t)sizeof(rec), MSG_PEEK);

    printf("Mensaje recibido: %s\n", rec);

    close(socket_descc);
    
    return 0;
}
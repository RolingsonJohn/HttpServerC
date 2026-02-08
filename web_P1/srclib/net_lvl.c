/** 
* @brief Definición del modulo net_lvl.
* 
* @file net_lvl.c
* @author John Ander Rolingson Avellanal \
*  Diego Núñez Garcia
* @version 2.0 
* @date 06-03-2024
* @copyright GNU Public License
*/

#include <arpa/inet.h>
#include <string.h>
#include "utils.h"
#include "net_lvl.h"

int client_socket_init(short server_port) {
    short socket_desc = 0;
    struct sockaddr_in server_address;

    /* Inicialización del socket TCP */
    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error de apertura del socket");
        return -1;
    }

    /* Configuracion del socket */
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(server_port);
    /* inet_addr() pasa string con dir ip a unsigned long con ordenación adecuada, la inversa es inet_ntoa() */
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    bzero(&(server_address.sin_zero), 8);

    /* Conexion al servidor */
    if (connect(socket_desc, (struct sockaddr*)&server_address, (socklen_t)sizeof(struct sockaddr)) < 0) {
        perror("Error al establecer la conexion");
        return -1;
    }

    return socket_desc;
}

int server_socket_init(short port) {
    short socket_desc = 0;
    struct sockaddr_in server_address;

    /* Inicialización del socket TCP */
    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error de apertura del socket");
        return -1;
    }

    /* Configuracion del socket */
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    bzero(&(server_address.sin_zero), 8);

    /** Cambio en la configuracion del socket para permitir la reutilizacion
    *   de direccion y puerto
    */
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    /* Asociación del socket con una dirección local (IP:Puerto) */
    if ((bind(socket_desc, (struct sockaddr*)&server_address, (socklen_t)sizeof(struct sockaddr))) == -1) {
        perror("Error de enlace del socket");
        return -1;
    }

    /* Puesta del socket en modo escucha*/
    if ((listen(socket_desc, max_clients)) == -1) {
        perror("Error al poner el socket en modo pasivo");
        return -1;
    }

    return socket_desc;
}



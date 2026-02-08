/** 
* @brief Declaración del modulo net_lvl.
* 
* @file net_lvl.h
* @author John Ander Rolingson Avellanal \
*  Diego Núñez Garcia
* @version 2.0 
* @date 06-03-2024
* @copyright GNU Public License
*/

#ifndef NET_LVL
#define NET_LVL

    #include <stdio.h>
    #include <stdlib.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <errno.h>

    #define SERVER_ADDR "127.0.0.1"         /*!< Direccion del servidor */

    /**
    * @brief Función que se encarga de inicializar
    *  el socket del cliente.
    *
    * @param server_port Puerto del servidor
    * 
    * @return Socket inicializado
    */
    int client_socket_init(short server_port);

    /**
    * @brief Función que se encarga de inicializar
    *  el socket del servidor.
    *
    * @param port Puerto del servidor
    * 
    * @return Socket inicializado
    */
    int server_socket_init(short port);
#endif
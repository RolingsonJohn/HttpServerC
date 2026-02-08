/** 
* @brief Definición del modulo server, fichero principal
*  que contiene main.
* 
* @file server.c
* @author John Ander Rolingson Avellanal \
*  Diego Núñez Garcia
* @version 2.0 
* @date 06-03-2024
* @copyright GNU Public License
*/

#include "net_lvl.h"
#include "utils.h"
#include "http.h"

int socket_desc = 0, new_socket_desc = 0;       /*!< Descriptores de los sockets */
Diccionario *dict_post;                         /*!< Diccionario que guarda informacion enviada por los clientes */
sem_t sem;                                      /*!< Semaforo para controlar la concurrencia */
pid_t pids[MAX_PROCS];                          /*!< Array con los pids de todos los procesos */

/**
* @brief Función que se encarga de la recepción de mensajes
*  y el envio de respuestas del servidor.
* 
* @return Código de estado de la operación
*/
int handler();

/**
* @brief Handler de la señal SIGINT.
*
* @param sig SIGINT
*/
void sigint_handler(int sig) {
    int i;
    if (getpid() != 0) {
        for (i = 0; i < MAX_PROCS; i++) {
            kill(pids[i], SIGINT);
        }
    }
    while(wait(NULL) > 0);
    sem_destroy(&sem);
    close(new_socket_desc);
    close(socket_desc);
    free(dict_post);
    free_confuse();
    printf("Salida mediante SIGINT\n");
    exit(OK);
}

/**
* @brief Handler de la señal SIGPIPE.
*
* @param sig SIGPIPE
*/
void sigpipe_handler(int sig) {
}

/**
* @brief Punto de entrada principal del programa.
*
* @param argc Número de argumentos de línea de comandos
* @param argv Array de argumentos de línea de comandos
*
* @return Código de estado de la operación
*/
int main(int argc, char *argv[]) {
    int i;
    struct sockaddr_in client_addr;
    struct sigaction act;
    socklen_t client_addr_len = sizeof(client_addr);
    pid_t pid;

    /* Obtención de la configuración del servidor */
    confuse_init();

    /* Creación del diccionario del servidor */
    if ((dict_post = create_dictionary()) == NULL) return FAILURE;

    /* Inicialización del socket del servidor */
    if ((socket_desc = server_socket_init(listen_port)) == -1) return FAILURE;

    /* Preparación de la mascara para SIGINT */
    act.sa_handler = sigint_handler;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    /* Preparación de la mascara para SIGPIPE */
    act.sa_handler = sigpipe_handler;
    sigemptyset(&(act.sa_mask));
    act.sa_flags = 0;
    sigaction(SIGPIPE, &act, NULL);

    /* Aceptación de las peticiones de los clientes*/
    sem_init(&sem, 1, 1);
    for (i = 0; i < MAX_PROCS; i++) {
        pid = fork();
        if ( pid == 0) {
            while (1) {
                sem_wait(&sem);
                if ((new_socket_desc = accept(socket_desc, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
                    perror("Several error");
                    free(dict_post);
                    exit(FAILURE);
                }
                sem_post(&sem);

                if (handler(dict_post) == (FAILURE)) {
                    free(dict_post);
                    close(new_socket_desc);
                    exit(FAILURE);
                }
                
                close(new_socket_desc);
            }    
        }else if (pid < 0) {
            perror("Fallo de concurrencia:");
        }

        pids[i] = pid;
    }

    while(wait(NULL) > 0);
    for (i = 0; i < MAX_PROCS; i++) {
        kill(pids[i], SIGINT);
    }
    close(new_socket_desc);
    close(socket_desc);

    return OK;
}

/**
* @brief Función que se encarga enviar el contenido
*  de la petición del cliente.
*
* @param response Respuesta del servidor
* @param socket Socket de conexion con el cliente
* 
* @return Código de estado de la operación
*/
int build_http_content(Http_response *response, int socket) {
    FILE *fd = NULL;
    ssize_t bytes = 0;

    if (response == NULL)
        return FAILURE;

    if ((fd = fopen(response->path, "rb")) == NULL) return FAILURE;
   
    while ((bytes = fread(response->content, 1, BUFF_SIZE, fd)) > 0) {
        if (bytes <= 0)
            break;
        send(socket, response->content, BUFF_SIZE, 0);
    }

    fclose(fd);

    return OK;
}

/**
* @brief Función que se encarga de la recepción de peticiones
*  y el envio de respuestas por parte del servidor.
*
* @param dict_post Diccionario con informacion del cliente
* 
* @return Código de estado de la operación
*/
int handler(Diccionario *dict_post) {
    Http_response *response = NULL;
    char *buffer = NULL;
    ssize_t bytes;
    int ret = OK;

    if ((buffer = (char*)calloc(BUFF_SIZE, sizeof(char))) == NULL) return FAILURE;

    /* Recepción de peticiones */
    if (new_socket_desc >= 0) {
        bytes = recv(new_socket_desc, (void*)buffer, BUFF_SIZE, 0);
        if (bytes <= 0) {
            free(buffer);
            close(new_socket_desc);
            printf("Cliente ha cerrado conexión\n");
            return CLOSE;
        }
    }

    if ((response = generate_response(buffer, dict_post)) == NULL) {
        perror("Error al generar el mensaje:");
        ret = FAILURE;
        response = generate_error_response();
    }
    
    /* Envio de mensajes */
    if (new_socket_desc >= 0) {
        bytes = send(new_socket_desc, (void *)response->header, strlen(response->header), 0);
        /* Revisar comparaciones */
        if (strcmp(response->method, "GET") == 0 && response->executed == FALSE)
            build_http_content(response, new_socket_desc);
        else if (strcmp(response->method, "OPTIONS") != 0 && strcmp(response->method, "HEAD") != 0)
            bytes = send(new_socket_desc, (void *)response->content,response->content_length, 0);

    }
    
    if (response->content != NULL)
        free(response->content);
    free(response);
    free(buffer);
    return ret;
}

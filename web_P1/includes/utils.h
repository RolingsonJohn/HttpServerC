/** 
* @brief Declaración del modulo utils.
* 
* @file utils.h
* @author John Ander Rolingson Avellanal \
*  Diego Núñez Garcia
* @version 2.0 
* @date 06-03-2024
* @copyright GNU Public License
*/

#ifndef _UTILS
#define _UTILS
    #define _GNU_SOURCE
    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <errno.h>
    #include <signal.h>
    #include <time.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <sys/wait.h>
    #include <sys/mman.h>
    #include <sys/time.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <semaphore.h>
    #include "confuse.h"

    #define MAX_PROCS 25                /*!< Número maximo de procesos simultáneos */
    #define BUFF_SIZE 1024              /*!< Tamaño máximo de el buffer */
    #define SHORT_SIZE 128              /*!< Tamaño auxiliar */
    #define OK 0                        /*!< Codigo que indica que una operación ha sido correcta */
    #define FAILURE -1                  /*!< Codigo que indica que una operación ha sido incorrecta */
    #define CLOSE 1                     /*!< Codigo que indica el cierre de conexion por parte del cliente */

    extern char *server_root;           /*!< Variable que indica la ruta principal del servidor */
    extern long int max_clients;        /*!< Variable que indica el maximo de clientes */
    extern long int listen_port;        /*!< Variable que indica el puerto al que se debe conectar el servidor */
    extern char *server_signature;      /*!< Variable que indica el nombre del servidor */

    /**
    * @brief Estructura que almacena la información clave-valor
    *  de un Diccionario.
    */
    typedef struct {
        char key[128];                  /*!< Clave del diccionario */
        char value[128];                /*!< Valor del diccionario */
    }clave_valor;

    /**
    * @brief Estructura que define un diccionario
    *  clave-valor.
    */
    typedef struct
    {
        clave_valor content[BUFF_SIZE]; /*!< Tupla clave-valor */
        int total;                      /*!< Número total de tuplas clave-valor */
    }Diccionario;

    /**
    * @brief Función que se encarga de inicializar un diccionario
    *  vacio.
    * 
    * @return Nuevo Diccionario inicializado.
    */
    Diccionario *create_dictionary();

    /**
    * @brief Función que se encarga de obtener una valor
    *  del diccionario mediante una clave.
    *
    * @param dict Diccionario a usar
    * @param key Clave necesaria para obtener el valor
    * 
    * @return Valor asociado a la clave
    */
    char *get_value(Diccionario *dict, char *key);

    /**
    * @brief Función que se encarga de poner una nueva
    *  tupla clave-valor en el diccionario.
    * 
    * @param dict Diccionario a usar
    * @param key Clave a colocar en el diccionario
    * @param value Valor asociado a la clave
    */
    void set_value(Diccionario *dict, char *key, char *value);

    /**
    * @brief Función que se encarga de obtener
    *  la longitud de un determinado archivo.
    *
    * @param file Archivo del que se quiere obtener la longitud
    * 
    * @return Longitud del archivo
    */
    long file_length(char *file);

    /**
    * @brief Función que se encarga de leer un archivo
    *  y copiar su contenido en una cadena.
    *
    * @param file Archivo a leer
    * 
    * @return Cadena con el contenido del archivo
    */
    char *read_file(char *file);

    /**
    * @brief Función que se encarga de poner la fecha actual
    *  en formato HTTP en la cadena pasada como argumento.
    *
    * @param date_buffer Cadena donde se almacenará la fecha HTTP
    * @param buffer_size Tamaño de la cadena
    */
    void get_http_date(char *date_buffer, size_t buffer_size);

    /**
    * @brief Función que se encarga dividir una cadena mediante
    *  un caracter pasado como argumento.
    *
    * @param org Cadena a dividir
    * @param left Cadena con la parte izquierda de la cadena original
    * @param right Cadena con la parte derecha de la cadena original
    * @param elem Caracter que dicta donde se hace la división
    */
    void split_by_char(char *org, char *left, char *right, char elem);
    /**
     * @brieg Función que obtiene todas las variables de un string
     * 
     * @param str Cadena con todo el contenido en formato http
     * 
     * @return Cadena con las claves de las variables separadas por ' ' 
    */
    char *get_variables(char* str);

    /**
    * @brief Función que se encarga de hacer la lectura
    *  del archivo de la configuración del servidor y guardarla
    *   en una estructura.
    */
    void confuse_init();

    /**
    * @brief Función que se encarga liberar la estructura
    *  con la configuracion del servidor.
    */
    void free_confuse();
#endif
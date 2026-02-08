/** 
* @brief Declaración del modulo http.
* 
* @file http.h
* @author John Ander Rolingson Avellanal \
*  Diego Núñez Garcia
* @version 2.0 
* @date 06-03-2024
* @copyright GNU Public License
*/

#ifndef _HTTP
#define _HTTP
    #include <time.h>
    #include "utils.h"
    #include "picohttpparser.h"

    #define TRUE 1                      /*!< Macro auxiliar para indicar correcto funcionamiento */
    #define FALSE -1                    /*!< Macro auxiliar para indicar funcionamiento incorrecto */
    #define EXECUTE 100                 /*!< Codigo informativo HTTP */
    #define NOT_SUPP 505                /*!< Codigo informativo HTTP */
    #define BAD_ADDR 400                /*!< Codigo informativo HTTP */

    /**
    * @brief Estructura que almacena informacion acerca
    *  de la peticion del cliente
    */
    typedef struct _Http_request Http_request;

    /**
    * @brief Estructura que almacena informacion acerca
    *  de la respuesta del servidor.
    */
    typedef struct {
        char method[SHORT_SIZE];        /*!< Variable que guarda el metodo de la petición */
        char header[BUFF_SIZE];         /*!< Variable que guarda linea de estado y cabeceras */
        char path[SHORT_SIZE];          /*!< Variable que guarda la ruta al recurso */
        char *content;                  /*!< Variable que guarda el contenido de la respuesta */
        int content_length;             /*!< Tamaño del contenido de la respuesta */
        short executed;                 /*!< Variable auxiliar que indica si hay ejecución */
    }Http_response;

    /**
    * @brief Función que se encarga de generar la respuesta
    *  HTTP del servidor.
    *
    * @param mssg Petición del cliente
    * @param dict_post Diccionario con informacion del cliente
    * 
    * @return Respuesta del servidor
    */
    Http_response *generate_response(char *mssg, Diccionario *dict_post);

    /**
    * @brief Función que se encarga de generar una respuesta de error
    *  HTTP Internal Server Error.
    *
    * @return Respuesta de error del servidor
    */
    Http_response *generate_error_response();

#endif
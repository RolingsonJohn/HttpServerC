/** 
* @brief Definición del modulo utils.
* 
* @file utils.c
* @author John Ander Rolingson Avellanal \
*  Diego Núñez Garcia
* @version 2.0 
* @date 06-03-2024
* @copyright GNU Public License
*/

#include <string.h>
#include <stdio.h>
#include "utils.h"

char *server_root = NULL;
long int max_clients = 0;
long int listen_port = 0;
char *server_signature = NULL;

cfg_t *cfg = NULL;                  /*!< Variable global con la configuración del servidor */

Diccionario *create_dictionary() {
    Diccionario *dict;

    if ((dict = (Diccionario*)malloc(sizeof(dict[0]))) == NULL) return NULL;
    dict->total = 0;

    return dict;
}

char *get_value(Diccionario *dict, char *key) {
    int i;

    /* Búsqueda del valor en el diccionario mediante la clave proporcionada */
    for (i = 0; i < dict->total; i++) {
        if (strcmp(key, dict->content[i].key) == 0) {
            return key;
        }
    }

    return NULL;
}

void set_value(Diccionario *dict, char *key, char *value) {

    strcpy(dict->content[dict->total].key, key); strcpy(dict->content[dict->total].value, value);
    dict->total++;
    dict->total%=BUFF_SIZE;
}

long file_length(char *file) {
    long length;
    FILE *fd = NULL;

    if ((fd = fopen(file,"rb")) == NULL) {
        perror("Error al abrir el archivo");
        return -1;
    }

    /* Colocación del descriptor al final del archivo */
    fseek(fd, 0, SEEK_END);
    length = ftell(fd);
    fclose(fd);

    return length;
}

void get_http_date(char *date_buffer, size_t buffer_size) {
    time_t current_time;
    struct tm *time_info;
    
    /* Obtención de la fecha actual */
    time(&current_time);
    time_info = gmtime(&current_time);

    /* Codificación de la fecha a formato HTTP */
    strftime(date_buffer, buffer_size, "%a, %d %b %Y %H:%M:%S GMT", time_info);
}

void split_by_char(char *org, char *left, char *right, char elem) {
    char *split;
    size_t left_len, right_len;

    if (org == NULL || left == NULL || right == NULL) return;

    /* Localización del caracter pedido */
    split = strchr(org, elem);
    left_len = split - org;
    right_len = strlen(split+1);
    
    /* Separación de la cadena actual en izquierda y derecha */
    strncpy(left, org, left_len); strncpy(right, split + 1, right_len);
    left[left_len] = '\0'; right[right_len] = '\0';
}



char *read_file(char *file) {
    int tam = 0, i;
    FILE *fd;
    char *result;

    if (file == NULL) return NULL;

    if ((fd = fopen(file, "rb")) == NULL) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    /* Colocación del descriptor al final del archivo */
    fseek(fd, 0, SEEK_END);
    tam = ftell(fd);
    fclose(fd);

    if ((result = (char *) malloc(tam+1*sizeof(result[0]))) == NULL) return NULL;

    if ((fd = fopen(file, "r")) == NULL) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    /* Copia del fichero caracter a caracter */
    for (i = 0; i < tam; i++) {
        result[i] = fgetc(fd);
    }
    result[i] = '\0';
    fclose(fd);

    return result;
}


char *get_variables(char* str) {
    int len;
    char *key = NULL, *value = NULL, *token = NULL, *dst = NULL;

    if (str == NULL) return NULL;
    if (strchr(str, '&') == NULL || (len = strlen(str)) <= 0) return NULL;

    if ((dst = (char*)malloc(len*sizeof(char*))) == NULL)
        return NULL;

    dst[0] = '\0';
    token = strtok(str, "\0");
    while (token != NULL) {
        key = strtok(token, "="); value = strtok(NULL, "&");
        
        if (key != NULL && value != NULL) {
            strcat(dst, value); strcat(dst, " ");
        }
        
        token = strtok(NULL, "&");
    }

    return dst;
}


void confuse_init() {
    /* Declaración de las opciones de configuracion del servidor */
    cfg_opt_t opts[] = {
        CFG_SIMPLE_STR("server_root", &server_root),
        CFG_SIMPLE_INT("max_clients", &max_clients),
        CFG_SIMPLE_INT("listen_port", &listen_port),
        CFG_SIMPLE_STR("server_signature", &server_signature),
        CFG_END()
    };

    cfg = cfg_init(opts, 0);
    cfg_parse(cfg, "server.conf");
}

void free_confuse() {
    cfg_free(cfg);
    free(server_root);
    free(server_signature);
}
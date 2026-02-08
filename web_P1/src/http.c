/** 
* @brief Definición del modulo http.
* 
* @file http.c
* @author John Ander Rolingson Avellanal \
*  Diego Núñez Garcia
* @version 2.0 
* @date 06-03-2024
* @copyright GNU Public License
*/

#include  "http.h"

struct _Http_request{
    char method[SHORT_SIZE];        /*!< Variable Metodo HTTP */
    char desc[SHORT_SIZE];          /*!< Cadena descriptiva del codigo de respuesta */
    char version[10];               /*!< Version HTTP */
    char *ext;                      /*!< Extensión del recurso */
    char file[SHORT_SIZE];          /*!< Recurso que se va a emplear */
    char *request;                  /*!< Variables del metodo POST */
};

/**
* @brief Función que se encarga de liberar un
*  Http_request.
* 
* @param header Http_request a liberar
*/
void free_header (Http_request *header) {
    if (header->request != NULL)
        free(header->request);
    free(header);
}

/**
* @brief Función que se encarga de extraer información del
*  cuerpo de la petición del cliente.
* 
* @param mssg Petición del cliente
* @param header Estructura donde se guardará la informacion
* 
* @return Código de estado de la operación
*/
int extract_body_info(const char *mssg, Http_request *header) {
    char *body_start;
    int i, len;

    body_start = strstr(mssg, "\r\n\r\n");

    body_start += 4;
    for (len = 0; body_start[len] != '\0'; len++);

    
    if ((header->request = (char*)malloc(len*sizeof(header->request))) ==NULL) {
        perror("Error de reserva de memoria");
        return FAILURE;
    }
    
    for (i = 0; i < len; i++) {
        header->request[i] = body_start[i];
    }
    header->request[i] = '\0';

    return OK;
}

/**
* @brief Función que se encarga de extraer información de
* la petición del cliente.
* 
* @param mssg Petición del cliente
* @param header Estructura donde se guardará la informacion
* 
* @return Código de estado de la operación
*/
int parse_http_request(const char *mssg, Http_request *header) {
    const char *method, *path;
    size_t method_len, path_len;
    struct phr_header headers[100];
    size_t num_headers = sizeof(headers) / sizeof(headers[0]);
    int pret, minor_version;

    pret = phr_parse_request(mssg, strlen(mssg), &method, &method_len, &path, &path_len, &minor_version, headers, &num_headers, 0);
    
    if (pret < 0) return FAILURE;
    
    snprintf(header->method, method_len+1, "%.*s",(int)method_len,method);  snprintf(header->file, path_len+1, "%.*s",(int)path_len,path); snprintf(header->version, SHORT_SIZE, "HTTP/1.%d", minor_version);
    header->method[method_len] = '\0';header->file[path_len] = '\0';

    if (strcmp(header->method, "POST") == 0) return extract_body_info(mssg, header);
    
    return OK;
}

/**
* @brief Función que se encarga de extraer las variables y sus
*  valores de una petición.
* 
* @param request Petición del cliente
* @param dict_post Diccionario con información del cliente
* 
* @return Código de estado de la operación
*/
int split_post_content(char *request, Diccionario *dict_post) {
    int i;
    char *key, *value, *token;

    if (request == NULL) return FAILURE;
    token = strtok(request, "\0");
    for (i = 0; token != NULL;) {
        key = strtok(token, "="); value = strtok(NULL, "&");
        
        if (key != NULL && value != NULL) {
            set_value(dict_post, key, value);
            i++;
        }
        
        token = strtok(NULL, "&");
    }

    return i;
}

/**
* @brief Función que permite distinguir la extensión de un fichero dado
*  y lo clasifica.
* 
* @param file String con el nombre del fichero
* @param content String que contiene la información de identificación
*               del documento dado (content-type)
* 
* @return Código de estado de la operación
*/
int get_content_type(char *file, char *content) {
    char *ext;
    
    file++;
    if (strchr(file, '.') == NULL) {
        return BAD_ADDR;
    }
    file--;
        
    strtok(file, "."); ext = strtok(NULL, "");

    if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0 )
        strcpy(content,"text/html");
    else if (strcmp(ext, "txt") == 0 )
        strcpy(content, "text/plain");
    else if (strcmp(ext, "gif") == 0 )
        strcpy(content, "image/gif");
    else if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0 )
        strcpy(content, "image/jpeg");
    else if (strcmp(ext, "mpeg") == 0 || strcmp(ext, "mpg") == 0)
        strcpy(content, "video/mpeg");
    else if (strcmp(ext, "doc") == 0 || strcmp(ext, "docx") == 0 )
        strcpy(content, "application/msword");
    else if (strcmp(ext, "pdf") == 0 )
        strcpy(content, "application/pdf");
    else if (strcmp(ext,"py") == 0 || strcmp(ext,"php") == 0)
        return EXECUTE;
    else
        return NOT_SUPP;
    
    return OK;
}

/**
* @brief Función que se encarga de construir una respuesta de 
*  error HTTP.
* 
* @param header Estructura donde se guardará la informacion
* 
* @return Respuesta del servidor
*/
Http_response *build_http_response_error (Http_request *header) {
    Http_response *response = NULL;
    char actual_date[50];


    if ((response = (Http_response*)malloc(sizeof(Http_response))) == NULL)
        return NULL;
    if ((response->content = (char*)malloc(BUFF_SIZE*sizeof(response->content[0]))) == NULL)
        return NULL;

    get_http_date(actual_date, sizeof(actual_date));
    response->method[0] = '\0';

    snprintf(response->content, BUFF_SIZE, "<html>"
                                                "<h1> Error: %s</h1>"
                                            "</html>", header->desc);
    response->content_length = strlen(response->content);

    snprintf(response->header, BUFF_SIZE, "%s %s"
                                        "\r\nDate: %s"
                                        "\r\nServer: %s"
                                        "\r\nContent-Length: %d"
                                        "\r\nContent-Type: %s"
                                        "\r\n\r\n", header->version, header->desc,actual_date, server_signature, response->content_length, "text/html");

    return response;
}


/**
* @brief Función que se encarga de ejecutar un script
*  requerido por el usuario.
* 
* @param file Script
* @param header Estructura donde se guardará la informacion
* @param args Argumentos del srcipt
* 
* @return Resultado del script
*/
char *get_script_result(char *file, Http_request *header, char *args) {
    char command[BUFF_SIZE], extension[SHORT_SIZE], name_file[SHORT_SIZE], dst_file[SHORT_SIZE], *result;

    chmod(header->file, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    split_by_char(file, name_file, extension, '.');

    if (strchr(args, '|') != NULL || strchr(args, '>') != NULL) {
        strcpy(header->desc, "400 Bad Request");
        return NULL;
    }else if (strcmp(extension, "py") == 0) 
        snprintf(command, BUFF_SIZE, "python3 %s %s > %d.txt", header->file, args, getpid());

    else if(strcmp(extension, "php") == 0) 
         snprintf(command, BUFF_SIZE, "php %s %s > %d.txt", header->file, args, getpid());
    else {
        strcpy(header->desc, "415 Unsupported Media Type");
        return NULL;
    }
    sprintf(dst_file, "%d.txt", getpid());
    system(command);
    result = read_file(dst_file);

    if (result == NULL)
        strcpy(header->desc, "500 Internal Server Error");

    return result;
}

/**
* @brief Función que se encarga de construir la respuesta del servidor
*  a una petición GET.
* 
* @param header Estructura donde se guardará la informacion
* @param variable Variables del metodo GET
* 
* @return Respuesta del servidor
*/
Http_response *build_GET_response(Http_request *header, char *variable) {
    Http_response *response = NULL;
    char actual_date[50], last_modified_str[50], content_type[50] = "\0"; 
    char path[SHORT_SIZE], command[SHORT_SIZE], *file, *result, key[SHORT_SIZE], value[SHORT_SIZE];
    struct stat file_stat;
    struct tm *tm_info;
    time_t last_modified;
    int content_type_flag = OK, size = 0;

    if ((response = (Http_response*)malloc(sizeof(response[0]))) == NULL)
        return NULL;
    if ((response->content = (char*)calloc(BUFF_SIZE, sizeof(response->content[0]))) == NULL)
        return NULL;

    response->executed = FALSE;
    get_http_date(actual_date, sizeof(actual_date));

    strcpy(path, header->file);
    content_type_flag = get_content_type(path, &content_type[0]);
    
    strcpy(response->method, header->method);
    strcpy(response->path, header->file);
    strcpy(header->desc, "200 OK");

    if (stat(header->file, &file_stat) == 0 && content_type_flag == OK) {
        last_modified = file_stat.st_mtime;
        tm_info = localtime(&last_modified);
        strftime(last_modified_str, sizeof(last_modified_str), "%a, %d %b %Y %H:%M:%S GMT", tm_info);
        
        response->content_length = file_length(header->file);

    }else if (content_type_flag == EXECUTE) {
        response->executed = TRUE;
        file = header->file;
        file++;


        strcpy(value, variable);
        if (strchr(variable, '=') != NULL)
            split_by_char(variable, key, value, '=');

        result = get_script_result(file, header, value);

        last_modified = file_stat.st_mtime;
        tm_info = localtime(&last_modified);
        strftime(last_modified_str, sizeof(last_modified_str), "%a, %d %b %Y %H:%M:%S GMT", tm_info);

        if (size+40 >= BUFF_SIZE || result == NULL) {
            if(result == NULL) 
                return (response = build_http_response_error(header));
    
            if ((response->content = realloc(response->content, BUFF_SIZE+size+40)) == NULL) {
                free(response);
                return (response = build_http_response_error(header));
            }
        }

        size = strlen(result);
        sprintf(command, "rm -f %d.txt", getpid());
        system(command);
        
        sprintf(response->content, "<html>"
                                        "<h1>Execution</h1>"
                                        "<pre>%s</pre>"
                                    "</html>",result);
        free(result);
        response->content_length = strlen(response->content);
        strcpy(content_type,"text/html");

    }else{
        strcpy(response->content, "<html>\n<body>\n<h1>Error: 400 Bad Request\n</h1>\n</body>\n</html>\0");
        response->content_length = strlen(response->content);

        /* Revisar codigo de error */
        snprintf(response->header, BUFF_SIZE, "%s 400 Bad Request"
                                            "\r\nDate: %s"
                                            "\r\nServer: %s"
                                            "\r\nContent-Length: %d"
                                            "\r\nContent-Type: %s"
                                            "\r\n\r\n", header->version, actual_date, server_signature, response->content_length, "text/html");
        return response;
    }

    snprintf(response->header, BUFF_SIZE, "%s %s"
                                    "\r\nDate: %s"
                                    "\r\nServer: %s"
                                    "\r\nLast-Modified: %s"
                                    "\r\nContent-Length: %d"
                                    "\r\nContent-Type: %s"
                                    "\r\n\r\n", header->version, header->desc, actual_date, server_signature, last_modified_str, response->content_length, content_type);

    return response;
}

/**
* @brief Función que se encarga de construir la respuesta del servidor
*  a una petición POST.
* 
* @param header Estructura donde se guardará la informacion
* @param dict_post Diccionario con información del cliente
* 
* @return Respuesta del servidor
*/
Http_response *build_POST_response(Http_request *header, Diccionario *dict_post) {
    Http_response *response = NULL;
    char actual_date[50], last_modified_str[50], content_type[50] = "\0", command[SHORT_SIZE], variables[BUFF_SIZE]="", *file, *result;
    struct stat file_stat;
    struct tm *tm_info;
    time_t last_modified;
    int content_type_flag = OK, size = 0, total_vars = 0, i;

    if ((response = (Http_response*)malloc(sizeof(response[0]))) == NULL)
        return NULL;
    if ((response->content = (char*)malloc(BUFF_SIZE*sizeof(response->content[0]))) == NULL)
        return NULL;

    get_http_date(actual_date, sizeof(actual_date));

    file = header->file;

    total_vars = split_post_content(header->request, dict_post);

    strcpy(response->method, header->method);
    strcpy(response->path, header->file);
    
    file++;

    if (stat(header->file, &file_stat) == 0 && content_type_flag == OK && total_vars > 0) {

        last_modified = file_stat.st_mtime;
        tm_info = localtime(&last_modified);
        strftime(last_modified_str, sizeof(last_modified_str), "%a, %d %b %Y %H:%M:%S GMT", tm_info);
        
        for (i = 1; i <= total_vars; i++) {
            strcat(variables, dict_post->content[(dict_post->total-i+BUFF_SIZE)%BUFF_SIZE].value);
            strcat(variables, " ");
        }
        result = get_script_result(file, header, variables);

        if (size+40 >= BUFF_SIZE || result == NULL) {

            if(result == NULL) 
                return build_http_response_error(header);
    
            if ((response->content = realloc(response->content, BUFF_SIZE+size+40)) == NULL) {
                free(response);
                return build_http_response_error(header);
            }
        }

        size = strlen(result);
        sprintf(command, "rm -f %d.txt", getpid());
        system(command);
        
        sprintf(response->content, "<html>"
                                        "<h1>Execution</h1>"
                                        "<pre>%s</pre>"
                                    "</html>",result);
        free(result);

        response->content_length = strlen(response->content);
        strcpy(content_type, "text/html");

        snprintf(response->header, BUFF_SIZE, "%s 201 Created"
                                            "\r\nDate: %s"
                                            "\r\nServer: %s"
                                            "\r\nLast-Modified: %s"
                                            "\r\nContent-Length: %d"
                                            "\r\nContent-Type: %s"
                                            "\r\n\r\n", header->version, actual_date, server_signature, last_modified_str, response->content_length, content_type);
    }else {
        strcpy(response->content, "<html>\n<body>\n<h1>Error: 400 Bad Request\n</h1>\n</body>\n</html>\0");
        response->content_length = strlen(response->content);

        /* Revisar codigo de error */
        snprintf(response->header, BUFF_SIZE, "%s 400 Bad Request"
                                            "\r\nDate: %s"
                                            "\r\nServer: %s"
                                            "\r\nContent-Length: %d"
                                            "\r\nContent-Type: %s"
                                            "\r\n\r\n", header->version, actual_date, server_signature, response->content_length, "text/html");
    }

    return response;
}

/**
* @brief Función que se encarga de construir la respuesta del servidor
*  a una petición OPTIONS.
* 
* @param header Estructura donde se guardará la informacion
* 
* @return Respuesta del servidor
*/
Http_response *build_OPTIONS_response(Http_request *header) {
    char actual_date[50] = "\0";
    Http_response *response = NULL;

    if ((response = (Http_response*)malloc(sizeof(response[0]))) == NULL)
        return NULL;
    if ((response->content = (char*)malloc(BUFF_SIZE*sizeof(response->content[0]))) == NULL)
        return NULL;


    get_http_date(actual_date, sizeof(actual_date));
    strcpy(response->method, header->method);
    
    snprintf(response->header, BUFF_SIZE, "%s 204 No Content"
                                          "\r\nDate: %s"
                                          "\r\nServer: %s"
                                          "\r\nAllow: OPTIONS, GET, HEAD, POST"
                                          "\r\nCache-Control: max-age=%d"
                                          "\r\n\r\n", header->version, actual_date, server_signature, 3600);

    return response;
}

/**
* @brief Función que se encarga de construir la respuesta del servidor
*  a una petición HEAD.
* 
* @param header Estructura donde se guardará la informacion
* 
* @return Respuesta del servidor
*/
Http_response *build_HEAD_response(Http_request *header) {
    Http_response *response = NULL;
    char actual_date[50], last_modified_str[50], content_type[50] = "\0", file[SHORT_SIZE];
    struct stat file_stat;
    struct tm *tm_info;
    time_t last_modified;
    int content_type_flag = OK;

    if ((response = (Http_response*)malloc(sizeof(response[0]))) == NULL)
        return NULL;
    if ((response->content = (char*)malloc(BUFF_SIZE*sizeof(response->content[0]))) == NULL)
        return NULL;

    get_http_date(actual_date, sizeof(actual_date));
    strcpy(file, header->file);
    content_type_flag = get_content_type(file, &content_type[0]);
    strcpy(response->method, header->method);

    if (stat(header->file, &file_stat) == 0 && content_type_flag == OK) {
        last_modified = file_stat.st_mtime;
        tm_info = localtime(&last_modified);
        strftime(last_modified_str, sizeof(last_modified_str), "%a, %d %b %Y %H:%M:%S GMT", tm_info);
        
        response->content_length = file_length(header->file);

        snprintf(response->header, BUFF_SIZE, "%s 204 No Content"
                                            "\r\nDate: %s"
                                            "\r\nServer: %s"
                                            "\r\nLast-Modified: %s"
                                            "\r\nContent-Length: %d"
                                            "\r\nContent-Type: %s"
                                            "\r\n\r\n", header->version, actual_date, server_signature, last_modified_str, response->content_length, content_type);
    }else {

        strcpy(response->content, "<html>\n<body>\n<h1>Error: 400 Bad Request\n</h1>\n</body>\n</html>\0");
        response->content_length = strlen(response->content);

        /* Revisar codigo de error */
        snprintf(response->header, BUFF_SIZE, "%s 400 Bad Request"
                                            "\r\nDate: %s"
                                            "\r\nServer: %s"
                                            "\r\nContent-Length: %d"
                                            "\r\nContent-Type: %s"
                                            "\r\n\r\n", header->version, actual_date, server_signature, response->content_length, "text/html");
    }

    return response;
}


Http_response *generate_error_response() {
    Http_request header;

    strcpy(header.version,"HTTP/1.1"); strcpy(header.desc, "500 Internal Server Error");

    return build_http_response_error(&header);
}

Http_response *generate_response(char *mssg, Diccionario *dict_post) {
    Http_request *header = NULL;
    Http_response *response = NULL;
    char path[SHORT_SIZE], file[SHORT_SIZE], variable[SHORT_SIZE] = "\0", *variables;
    
    strcpy(path, server_root);
    if ((header = (Http_request *) malloc(sizeof(header[0]))) == NULL)
        return NULL;

    header->request = NULL;
    if ((parse_http_request(mssg, header)) == FAILURE) {
        strcpy(header->version,"HTTP/1.1"); strcpy(header->desc, "400 Bad Request");
        return build_http_response_error(header);
    }
         
    strcat(path, header->file); 
    /* Verifica si existe alguna variable en la url */
    if (strchr(path, '?') != NULL) 
        split_by_char(path, file, variable, '?'); 
    else 
        strcpy(file, path);
    
    strcpy(header->file, file);

    if (access(file, F_OK) == -1) {
        strcpy(header->desc, "404 Not found");
        response = build_http_response_error(header);
    
    }else if (strcmp("GET", header->method) == 0) {
        /* Comprueba si existe mas de una variable en la url */
        if ((variables = get_variables(variable)) == NULL)
            response = build_GET_response(header, variable);
        else {
            response = build_GET_response(header, variables);free(variables);
        }

    }else if (strcmp("POST", header->method) == 0) {
        response = build_POST_response(header, dict_post);

    }else if (strcmp("OPTIONS", header->method) == 0) {
        response = build_OPTIONS_response(header);

    }else if (strcmp("DELETE", header->method) == 0) {
        strcpy(header->desc, "501 Method Not Implemented");
        response = build_http_response_error(header);
    }else if (strcmp("HEAD", header->method) == 0) {
        response = build_HEAD_response(header);

    }else if (strcmp("PUT", header->method) == 0) {
        strcpy(header->desc, "501 Method Not Implemented");
        response = build_http_response_error(header);
    }else {
        strcpy(header->desc, "400 Bad Request");
        response = build_http_response_error(header);
    }
    
    free_header(header);
    return response;
}
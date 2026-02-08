> [!NOTE]
> Este proyecto ha sido realizado por ambos miembros del equipo simultáneamente, haciendo uso de la extensión Liveshare de VSCode.

# Servidor Web
Primer Proyecto de Redes de Comunicación II.<br>Realizado por los alumnos:
<li>Diego Núñez García.</li>
<li>John Ander Rolingson Avellanal.</li>

## Introducción
Este proyecto consiste en la creación de un servidor web en el lenguaje de programación C.
## Dependencias
Es necesario que el host que depliegue el servidor tenga instaladas las siguientes librerías/herramientas:
<li>Makefile</li>
<li>Confuse3.3</li>
<li>Doxygen</li>
<li>Dot</li>

## Documento de Técnico
El proyecto está dividido en directorios varios:
<li>includes: Contiene todas las cabeceras
    <ul>
        <li>net_lvl.h: Cabeceras pertenecientes al módulo net_lvl.c.</li>
        <li>utils.h: Cabeceras pertenecientes al módulo utils.c e includes y macros necesarias para todo el sistema.</li>
        <li>http.h: Cabeceras necesarias para el módulo http.c y struct de respuestas http.</li>
        <li>picohttpparser.h: Cabeceras necesarias para el módulo picohttpparser.c.</li>
    </ul>
</li>
<li>src: Contiene todo la lógica de implementación del servidor
    <ul>
        <li>server.c: Código encargado del arranque del servidor, aceptación y recibimiento de peticiones http y emisión de respuestas http.</li>
        <li>client.c: Cliente TCP.</li>
        <li>client.py: Cliente encargado de realizar peticiones diversas al servidor e imprimir por pantalla los resultados.</li>
        <li>utils.c: Código necesario para funciones auxiliares.</li>
    </ul>
</li>
<li>lib: Contiene las librerías necesarias para</li>
<li>srclib: Contiene toda la implementación para de las librerias empleadas
    <ul>
        <li>net_lvl.c: Código encargado de la gestión de sockets.</li>
        <li>picohttpparser.c: Código encargado del parse de peticiones http.</li>
    </ul>
</li>
<li>www: Directorio que contiene todos los recursos disponibles del servidor.</li>
<li>Doxyfile: Fichero que se encarga de generar toda la documentación, comentada en las cabeceras de cada función</li>
<li>makefile: Fichero que se encarga de la compilación del código. Tiene como flags:
    <ul>
        <li>all: Se encarga de la compilación de todos los ficheros .c en objetos y de la generación de dos ejecutables <i>client</i>, cliente TCP y <i>server</i>, ejecutable del servidor.</li>
        <li>clean: Se encarga de eliminar todos los objetos y de borrar los ejecutables.</li>
    </ul>
</li>
<li>server.conf: Fichero de configuración del servidor.
    <ul>
        <li>server_root: Path donde se encuentran todos los recursos.</li>
        <li>max_clients: Número máximo de clientes aceptados a la vez.</li>
        <li>listen_port: Puerto en dedicado al servidor.</li>
        <li>server_signature: Nombre del servidor.</li>
    </ul>
</li>

El código se encuentra modularizado principlamente en los siguientes ficheros.

## Desarrollo Diseño
El servidor está implementado como un pool de procesos. La decisión de dicha implementación fue tomada tras comparar la implementación como *pool de procesos* con la implementación como *reactivos* y verificar los tiempos de ejecución de cada uno, llegando a la conclusión de que el pool de procesos es más eficiente.
<br>
Código que implementa el *pool de procesos*:

```c
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
```
## Códigos de Retorno Implementados
Ejecución correcta:
- **GET**: Obtención de un recurso del servidor. <pre>200 Ok</pre>
- **POST**: Subida de datos al servidor. <pre>201 Created</pre>
- **HEAD**: Obtención de la cabecera de la respuesta frente a un recurso. <pre>204 No Content</pre>
- **OPTIONS**: Obtención de información relevante del servidor. <pre>204 No Content</pre>

En casos de error:
- Método no implementado <pre>501 Not Implemented</pre>
- Solicitud de recursos no existente <pre>404 Not Found</pre>
- Formato incorrecto de petición <pre>400 Bad Request</pre>
- Solicitud de un recurso existente pero no soportado por el servidor <pre>415 Unsupported Media Type</pre>
## Conclusiones

### Conclusión Técnica
El servidor soporta los verbos HTTP: **GET**, **POST**, **HEAD** y **OPTIONS**. En caso de realizarse una petición con algún método no implementado se retorna una respuesta con código de error.
### Conclusión Personal
Se ha adquirido bastante conocimiento en el contexto del protocolo HTTP, en relación al parseo de peticiones, de respuestas, etc. Además, se ha comprendido el funcionamiento del protocolo en función de aceptación de peticiones y la ejecución de procesos desde la parte del servidor.
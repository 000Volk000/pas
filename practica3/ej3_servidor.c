/*
Servidor que lee de una cola abierta para lectura una cadena de caracteres y la
imprime por pantalla.

Lo hace mientras que el valor de esa cadena sea distinto a la palabra exit.
*/

#include "ej3_common.h"
#include <errno.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

// Prototipo de funcionn
void funcionLog(char *);
// Apuntador al fichero de log.  No se usa en este ejemplo, pero le servira en ejercicio resumen
FILE *fLog = NULL;

int main(int argc, char **argv)
{
    // Cola del servidor para recibir mensajes del cliente
    mqd_t mq_server_receive;
    // Cola del servidor para enviar respuestas al cliente
    mqd_t mq_server_send_response;
    // Atributos de la cola
    struct mq_attr attr;
    // Buffer para intercambiar mensajes
    char buffer[MAX_SIZE];
    char response_buffer[MAX_SIZE];
    // flag que indica cuando hay que parar. Se escribe palabra exit
    int must_stop = 0;
    // Buffer para mensajes de log
    char log_msg[MAX_SIZE + 200];

    // Inicializar los atributos de la cola
    attr.mq_maxmsg = 10;        // Maximo número de mensajes
    attr.mq_msgsize = MAX_SIZE; // Maximo tamaño de un mensaje
    // Nombre para la cola de recepción
    char serverReceiveQueueName[100];
    // Nombre para la cola de respuesta
    char serverResponseQueueName[100];

    // Nombre para la cola de recepción. Al concatenar el login sera unica en un sistema compartido.
    sprintf(serverReceiveQueueName, "%s-%s", SERVER_QUEUE, getenv("USER"));
    sprintf(log_msg, "[Servidor]: El nombre de la cola de recepción es: %s", serverReceiveQueueName);
    printf("%s\n", log_msg);
    funcionLog(log_msg);

    // Nombre para la cola de respuesta.
    sprintf(serverResponseQueueName, "%s-%s", RESPONSE_QUEUE, getenv("USER"));
    sprintf(log_msg, "[Servidor]: El nombre de la cola de respuesta es: %s", serverResponseQueueName);
    printf("%s\n", log_msg);
    funcionLog(log_msg);

    // Crear la cola de mensajes del servidor para recibir del cliente.
    mq_server_receive = mq_open(serverReceiveQueueName, O_CREAT | O_RDONLY, 0644, &attr);

    if (mq_server_receive == (mqd_t)-1)
    {
        perror("[Servidor]: Error al abrir la cola de recepción");
        funcionLog("[Servidor]: Error al abrir la cola de recepción (perror)");
        exit(-1);
    }
    sprintf(log_msg, "[Servidor]: El descriptor de la cola de recepción es: %d", (int)mq_server_receive);
    printf("%s\n", log_msg);
    funcionLog(log_msg);

    // Crear la cola de mensajes del servidor para enviar al cliente.
    mq_server_send_response = mq_open(serverResponseQueueName, O_CREAT | O_WRONLY, 0644, &attr);

    if (mq_server_send_response == (mqd_t)-1)
    {
        perror("[Servidor]: Error al abrir la cola de respuesta");
        funcionLog("[Servidor]: Error al abrir la cola de respuesta (perror)");
        // Si falla, cerrar la cola de recepción antes de salir
        mq_close(mq_server_receive);
        mq_unlink(serverReceiveQueueName);
        exit(-1);
    }
    sprintf(log_msg, "[Servidor]: El descriptor de la cola de respuesta es: %d", (int)mq_server_send_response);
    printf("%s\n", log_msg);
    funcionLog(log_msg);

    do
    {
        // Número de bytes leidos
        ssize_t bytes_read;

        // Recibir el mensaje del cliente
        bytes_read = mq_receive(mq_server_receive, buffer, MAX_SIZE, NULL);

        if (bytes_read < 0)
        {
            perror("[Servidor]: Error al recibir el mensaje");
            funcionLog("[Servidor]: Error al recibir el mensaje (perror)");

            strcpy(log_msg, "[Servidor]: Intentando enviar MSG_STOP al cliente por error en recepción.");
            fprintf(stderr, "%s\n", log_msg);
            funcionLog(log_msg);
            if (mq_send(mq_server_send_response, MSG_STOP, strlen(MSG_STOP) + 1, 0) == -1)
            {
                perror("[Servidor]: Error al enviar MSG_STOP al cliente");
                funcionLog("[Servidor]: Error al enviar MSG_STOP al cliente (perror)");
            }
            must_stop = 1; // Para salir del bucle y limpiar
            continue;      // Saltar el procesamiento del mensaje
        }
        // buffer ya está null-terminado por fgets del cliente si se envió MAX_SIZE
        // Si el cliente enviara strlen+1, entonces buffer[bytes_read] = '\0'; sería necesario.
        // Dado que el cliente original envía MAX_SIZE, strlen(buffer) es seguro.

        // Comprobar el fin del bucle
        if (strncmp(buffer, MSG_STOP, strlen(MSG_STOP)) == 0)
        {
            must_stop = 1;
            strcpy(log_msg, "[Servidor]: Recibido MSG_STOP. Terminando.");
            printf("%s\n", log_msg);
            funcionLog(log_msg);
        }
        else
        {
            time_t t;
            char tiempo[300];
            t = time(NULL);
            struct tm *p = localtime(&t);
            strftime(tiempo, 1000, "[%Y-%m-%d, %H:%M:%S]", p);

            sprintf(log_msg, "\n%s-> %s", tiempo, buffer); // buffer ya tiene \n
            printf("%s", log_msg);                         // No añadir \n extra si buffer ya lo tiene
            // Para el log, es mejor asegurar que no haya múltiples \n si buffer ya lo tiene
            char temp_log_msg[MAX_SIZE + 200];
            strncpy(temp_log_msg, buffer, MAX_SIZE);
            if (temp_log_msg[strlen(temp_log_msg) - 1] == '\n')
            {
                temp_log_msg[strlen(temp_log_msg) - 1] = '\0';
            }
            sprintf(log_msg, "[Servidor]: Recibido el mensaje: %s", temp_log_msg);
            funcionLog(log_msg);

            size_t num_chars = strlen(buffer);
            // Si el último caracter es \n y no se debe contar, ajustar:
            if (num_chars > 0 && buffer[num_chars - 1] == '\n')
            {
                num_chars--;
            }
            // Si se cuenta el \n, no hacer num_chars--. El enunciado dice "exceptuando el fin de cadena"
            // strlen no cuenta el \0, pero sí el \n si fgets lo incluyó.
            // Para ser precisos con "exceptuando el fin de cadena (\0)", strlen(buffer) es correcto
            // si el \n de fgets se considera un caracter. Si no, se debe restar 1 si está presente.
            // Asumiré que el \n de fgets no se cuenta como carácter de la cadena útil.

            sprintf(response_buffer, "Número de caracteres recibidos: %zu", num_chars);

            if (mq_send(mq_server_send_response, response_buffer, strlen(response_buffer) + 1, 0) == -1)
            {
                perror("[Servidor]: Error al enviar respuesta al cliente");
                funcionLog("[Servidor]: Error al enviar respuesta al cliente (perror)");

                strcpy(log_msg, "[Servidor]: Intentando enviar MSG_STOP al cliente por error en envío de respuesta.");
                fprintf(stderr, "%s\n", log_msg);
                funcionLog(log_msg);
                if (mq_send(mq_server_send_response, MSG_STOP, strlen(MSG_STOP) + 1, 0) == -1)
                {
                    perror("[Servidor]: Error al enviar MSG_STOP al cliente");
                    funcionLog("[Servidor]: Error al enviar MSG_STOP al cliente (perror)");
                }
                must_stop = 1; // Para salir del bucle y limpiar
            }
            else
            {
                sprintf(log_msg, "Enviada respuesta: %s", response_buffer);
                printf("%s\n", log_msg);
                funcionLog(log_msg);
            }
        }
    } while (!must_stop);

    // Cerrar la cola de respuesta
    if (mq_close(mq_server_send_response) == (mqd_t)-1)
    {
        perror("[Servidor]: Error al cerrar la cola de respuesta");
        funcionLog("[Servidor]: Error al cerrar la cola de respuesta (perror)");
        // Continuar para intentar cerrar y eliminar la otra cola
    }

    // Eliminar la cola de respuesta
    if (mq_unlink(serverResponseQueueName) == (mqd_t)-1)
    {
        perror("[Servidor]: Error al eliminar la cola de respuesta");
        funcionLog("[Servidor]: Error al eliminar la cola de respuesta (perror)");
    }
    strcpy(log_msg, "[Servidor]: Cola de respuesta cerrada y eliminada.");
    printf("%s\n", log_msg);
    funcionLog(log_msg);

    // Cerrar la cola de recepción
    if (mq_close(mq_server_receive) == (mqd_t)-1)
    {
        perror("[Servidor]: Error al cerrar la cola de recepción");
        funcionLog("[Servidor]: Error al cerrar la cola de recepción (perror)");
        exit(-1);
    }

    // Eliminar la cola de recepción
    if (mq_unlink(serverReceiveQueueName) == (mqd_t)-1)
    {
        perror("[Servidor]: Error al eliminar la cola de recepción");
        funcionLog("[Servidor]: Error al eliminar la cola de recepción (perror)");
        exit(-1);
    }
    strcpy(log_msg, "[Servidor]: Cola de recepción cerrada y eliminada.");
    printf("%s\n", log_msg);
    funcionLog(log_msg);

    return 0;
}

/* Función auxiliar, escritura de un log.
No se usa en este ejemplo, pero le puede servir para algun
ejercicio resumen */
void funcionLog(char *mensaje)
{
    int resultado;
    char nombreFichero[100];
    char mensajeAEscribir[300];
    time_t t;

    // Abrir el fichero
    sprintf(nombreFichero, "log-servidor.txt");
    if (fLog == NULL)
    {
        fLog = fopen(nombreFichero, "at");
        if (fLog == NULL)
        {
            perror("Error abriendo el fichero de log");
            exit(1);
        }
    }

    // Obtener la hora actual
    t = time(NULL);
    struct tm *p = localtime(&t);
    strftime(mensajeAEscribir, 1000, "[%Y-%m-%d, %H:%M:%S]", p);

    // Vamos a incluir la hora y el mensaje que nos pasan
    sprintf(mensajeAEscribir, "%s ==> %s\n", mensajeAEscribir, mensaje);

    // Escribir finalmente en el fichero
    resultado = fputs(mensajeAEscribir, fLog);
    if (resultado < 0)
        perror("Error escribiendo en el fichero de log");

    fclose(fLog);
    fLog = NULL;
}

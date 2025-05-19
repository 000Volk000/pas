/*
Cliente que envia por una cola abierta para escritura una cadena de caracteres
recogida por teclado, mientras que el valor de esa cadena sea distinto a la palabra exit
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
#include <signal.h> // Required for signal handling

// Apuntador al fichero de log. No se usa en este ejemplo, pero le servira en ejercicio resumen
FILE *fLog = NULL;

// Global queue descriptors for signal handler access
mqd_t mq_to_server_global = (mqd_t)-1;
mqd_t mq_from_server_global = (mqd_t)-1;

// Prototipos de funciones
void funcionLog(char *);
void cleanup_and_exit(int exit_status);
void handle_signal(int signum);

void cleanup_and_exit(int exit_status)
{
    char log_msg[256]; // Buffer for logging closure status

    // Mandar MSG_STOP si la salida es por señal y no se ha hecho aún
    // (handle_signal ya lo hace, esto es más para errores generales si se quisiera)

    // Cerrar la cola de recepción de respuestas
    if (mq_from_server_global != (mqd_t)-1)
    {
        if (mq_close(mq_from_server_global) == (mqd_t)-1)
        {
            perror("[Cliente]: Error al cerrar la cola de recepción de respuestas en cleanup");
            // funcionLog no se puede usar si fLog da problemas, o si este es el problema
        }
        else
        {
            // Loggear antes de cerrar fLog si es posible
            if (fLog != NULL)
                funcionLog("[Cliente]: Cola de recepción de respuestas cerrada en cleanup.");
        }
        mq_from_server_global = (mqd_t)-1;
    }

    // Cerrar la cola de envío al servidor
    if (mq_to_server_global != (mqd_t)-1)
    {
        if (mq_close(mq_to_server_global) == (mqd_t)-1)
        {
            perror("[Cliente]: Error al cerrar la cola de envío al servidor en cleanup");
        }
        else
        {
            if (fLog != NULL)
                funcionLog("[Cliente]: Cola de envío al servidor cerrada en cleanup.");
        }
        mq_to_server_global = (mqd_t)-1;
    }

    // Cerrar el fichero de log
    if (fLog != NULL)
    {
        sprintf(log_msg, "[Cliente]: Cerrando fichero de log.");
        funcionLog(log_msg); // Log this attempt
        if (fclose(fLog) != 0)
        {
            perror("[Cliente]: Error al cerrar el fichero de log en cleanup");
        }
        fLog = NULL;
    }

    exit(exit_status);
}

void handle_signal(int signum)
{
    char log_msg[256]; // Buffer for log messages
    const char *signal_name;

    switch (signum)
    {
    case SIGINT:
        signal_name = "SIGINT";
        break;
    case SIGTERM:
        signal_name = "SIGTERM";
        break;
    default:
        signal_name = "UNKNOWN SIGNAL";
        break;
    }

    printf("[Cliente]: Señal %d (%s) capturada.", signum, signal_name);
    sprintf(log_msg, "[Cliente]: Señal %d (%s) capturada.", signum, signal_name);
    funcionLog(log_msg);

    if (mq_to_server_global != (mqd_t)-1)
    {
        if (mq_send(mq_to_server_global, MSG_STOP, strlen(MSG_STOP) + 1, 0) != 0)
        {
            perror("[Cliente]: Error al enviar MSG_STOP desde el manejador de señal");
            funcionLog("[Cliente]: Error al enviar MSG_STOP desde el manejador de señal (perror)");
        }
        else
        {
            sprintf(log_msg, "[Cliente]: Enviado MSG_STOP al servidor desde manejador de señal.");
            funcionLog(log_msg);
        }
    }
    else
    {
        funcionLog("[Cliente]: Cola al servidor no está abierta, no se puede enviar MSG_STOP desde manejador.");
    }

    // Limpiar y salir. El status podría ser 128 + signum
    cleanup_and_exit(128 + signum);
}

int main(int argc, char **argv)
{
    // Cola para enviar mensajes al servidor - ahora global: mq_to_server_global
    // Cola para recibir respuestas del servidor - ahora global: mq_from_server_global
    // Buffer para intercambiar mensajes
    char send_buffer[MAX_SIZE];
    char recv_buffer[MAX_SIZE];
    // Nombre para la cola de envío al servidor
    char serverQueueName[100];
    // Nombre para la cola de recepción de respuestas
    char responseQueueName[100];
    ssize_t bytes_received;
    // Buffer para mensajes de log
    char log_msg[MAX_SIZE + 200];

    // Configurar manejadores de señales
    if (signal(SIGINT, handle_signal) == SIG_ERR)
    {
        perror("[Cliente]: No se pudo establecer el manejador para SIGINT");
        // funcionLog podría no estar lista si fLog no se pudo abrir
        // Considerar un log a stderr aquí o una salida temprana.
        // Para este ejercicio, se asume que funcionLog se inicializará.
    }
    if (signal(SIGTERM, handle_signal) == SIG_ERR)
    {
        perror("[Cliente]: No se pudo establecer el manejador para SIGTERM");
    }

    // Nombre para la cola a la que el cliente envía.
    sprintf(serverQueueName, "%s-%s", SERVER_QUEUE, getenv("USER"));
    sprintf(log_msg, "[Cliente]: El nombre de la cola para enviar al servidor es: %s", serverQueueName);
    printf("%s\n", log_msg);
    funcionLog(log_msg);

    // Nombre para la cola de la que el cliente recibe respuestas.
    sprintf(responseQueueName, "%s-%s", RESPONSE_QUEUE, getenv("USER"));
    sprintf(log_msg, "[Cliente]: El nombre de la cola para recibir del servidor es: %s", responseQueueName);
    printf("%s\n", log_msg);
    funcionLog(log_msg);

    // Abrir la cola para enviar mensajes al servidor.
    mq_to_server_global = mq_open(serverQueueName, O_WRONLY);
    if (mq_to_server_global == (mqd_t)-1)
    {
        perror("[Cliente]: Error al abrir la cola para enviar al servidor");
        funcionLog("[Cliente]: Error al abrir la cola para enviar al servidor (perror)");
        cleanup_and_exit(-1);
    }
    sprintf(log_msg, "[Cliente]: El descriptor de la cola para enviar es: %d", (int)mq_to_server_global);
    printf("%s\n", log_msg);
    funcionLog(log_msg);

    // Abrir la cola para recibir respuestas del servidor.
    // El servidor debe haberla creado.
    mq_from_server_global = mq_open(responseQueueName, O_RDONLY);
    if (mq_from_server_global == (mqd_t)-1)
    {
        perror("[Cliente]: Error al abrir la cola para recibir del servidor. ¿Está el servidor en ejecución y la ha creado?");
        funcionLog("[Cliente]: Error al abrir la cola para recibir del servidor. ¿Está el servidor en ejecución y la ha creado? (perror)");
        // mq_close(mq_to_server_global); // cleanup_and_exit lo hará
        cleanup_and_exit(-1);
    }
    sprintf(log_msg, "[Cliente]: El descriptor de la cola para recibir es: %d", (int)mq_from_server_global);
    printf("%s\n", log_msg);
    funcionLog(log_msg);

    sprintf(log_msg, "Mandando mensajes al servidor (escribir \"%s\" para parar):", MSG_STOP);
    printf("%s\n", log_msg);
    funcionLog(log_msg);
    do
    {
        printf("> "); // Prompt no se suele loguear, pero si se desea: funcionLog("> ");
        fgets(send_buffer, MAX_SIZE, stdin);

        // Enviar el mensaje al servidor (puede ser texto o MSG_STOP)
        // Se envía MAX_SIZE como en el original, fgets asegura null terminación dentro.
        if (mq_send(mq_to_server_global, send_buffer, MAX_SIZE, 0) != 0)
        {
            perror("[Cliente]: Error al enviar el mensaje");
            funcionLog("[Cliente]: Error al enviar el mensaje (perror)");
            // Salir si no se puede comunicar
            break;
        }
        // Log del mensaje enviado (sin el \n si fgets lo incluyó y no se quiere en el log)
        char temp_send_log[MAX_SIZE];
        strncpy(temp_send_log, send_buffer, MAX_SIZE - 1);
        temp_send_log[MAX_SIZE - 1] = '\0'; // ensure null termination
        if (strlen(temp_send_log) > 0 && temp_send_log[strlen(temp_send_log) - 1] == '\n')
        {
            temp_send_log[strlen(temp_send_log) - 1] = '\0';
        }
        sprintf(log_msg, "[Cliente]: Enviado mensaje: %s", temp_send_log);
        funcionLog(log_msg);

        // Si el cliente escribió MSG_STOP, salir del bucle después de enviarlo.
        // No se espera respuesta para MSG_STOP.
        if (strncmp(send_buffer, MSG_STOP, strlen(MSG_STOP)) == 0)
        {
            strcpy(log_msg, "[Cliente]: Enviado MSG_STOP. Terminando.");
            printf("%s\n", log_msg);
            funcionLog(log_msg);
            break;
        }

        // Recibir la respuesta del servidor (número de caracteres o MSG_STOP si el servidor tuvo un error)
        bytes_received = mq_receive(mq_from_server_global, recv_buffer, MAX_SIZE, NULL);
        if (bytes_received < 0)
        {
            perror("[Cliente]: Error al recibir respuesta del servidor");
            funcionLog("[Cliente]: Error al recibir respuesta del servidor (perror)");
            break; // Salir del bucle si hay error en la recepción
        }

        recv_buffer[bytes_received] = '\0'; // Asegurar null terminación

        sprintf(log_msg, "Respuesta del servidor: %s", recv_buffer);
        printf("%s\n\n", log_msg); // El \n\n es para formato de consola, el log tendrá uno
        funcionLog(log_msg);

        // Si el servidor envió MSG_STOP (debido a un error del servidor), el cliente también debe parar.
        if (strncmp(recv_buffer, MSG_STOP, strlen(MSG_STOP)) == 0)
        {
            strcpy(log_msg, "[Cliente]: El servidor ha indicado una parada. Terminando.");
            printf("%s\n", log_msg);
            funcionLog(log_msg);
            break;
        }

    } while (1); // El bucle se controla con 'break'

    // Cerrar colas y fichero de log a través de la función de limpieza
    cleanup_and_exit(0);
    // La siguiente línea no se alcanzará porque cleanup_and_exit llama a exit()
    return 0;
}

/* Función auxiliar, escritura de un log.
No se usa en este ejemplo, pero le puede servir para algun
ejercicio resumen */
void funcionLog(char *mensaje)
{
    int resultado;
    char nombreFichero[100];
    char mensajeAEscribir[MAX_SIZE + 400]; // Asegurar suficiente espacio
    time_t t;

    // Abrir el fichero si no está abierto ya
    sprintf(nombreFichero, "log-cliente.txt");
    if (fLog == NULL)
    {
        fLog = fopen(nombreFichero, "at");
        if (fLog == NULL)
        {
            perror("Error abriendo el fichero de log");
            // No llamar a exit(1) directamente aquí, especialmente si es llamada desde un handler
            // o si el programa debe intentar continuar o limpiar de otra forma.
            // Para este caso, si fLog no se puede abrir, los logs fallarán silenciosamente o con perror.
            return;
        }
    }

    // Obtener la hora actual
    t = time(NULL);
    struct tm *p = localtime(&t);                                // Nota: localtime no es async-signal-safe
    strftime(mensajeAEscribir, 1000, "[%Y-%m-%d, %H:%M:%S]", p); // Nota: strftime no es async-signal-safe

    // Vamos a incluir la hora y el mensaje que nos pasan
    // Usar snprintf para evitar desbordamientos
    char temp_full_message[MAX_SIZE + 500]; // Buffer temporal grande
    snprintf(temp_full_message, sizeof(temp_full_message), "%s ==> %s\n", mensajeAEscribir, mensaje);

    // Escribir finalmente en el fichero
    resultado = fputs(temp_full_message, fLog);
    if (resultado < 0)
        perror("Error escribiendo en el fichero de log");
    else
        fflush(fLog); // Asegurar que se escribe al disco

    // No cerrar fLog aquí, se cerrará al final del programa o en cleanup_and_exit
    // fclose(fLog); // ELIMINADO
    // fLog = NULL;  // ELIMINADO
}

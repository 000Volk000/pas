#include <errno.h> // Control de errores
#include <stdio.h>
#include <stdlib.h>
#include <string.h>   // strerror(
#include <sys/wait.h> // wait()
#include <time.h>
#include <unistd.h> //  fork(), getpid(), getppid()

int main()
{
    pid_t pid;
    int flag, status;
    int resultado;
    float num1, num2, suma;
    int fileDes[2]; // Array para los descriptores de fichero de la tubería: fileDes[0] para lectura, fileDes[1] para escritura

    if (pipe(fileDes) == -1) // pipe() crea una tubería y devuelve dos descriptores de fichero en fileDes.
    {
        printf("ERROR al crear la tubería");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    switch (pid)
    {
    case -1:
        printf("No se ha podido crear el proceso hijo");
        exit(EXIT_FAILURE);
    case 0:
        printf("[HIJO]: Mi PID es %d y mi PPID es %d\n", getpid(), getppid());

        if (close(fileDes[1]) == -1) // El hijo lee de la tubería, cierra la escritura.
        {
            printf("[HIJO]: Error en close (escritura)");
            exit(EXIT_FAILURE);
        }

        resultado = read(fileDes[0], &suma, sizeof(float)); // read() bloquea until datos disponibles o se cierre de escritura.

        if (resultado == -1)
        {
            printf("[HIJO]: ERROR al leer de la tubería");
            exit(EXIT_FAILURE);
        }
        else if (resultado == 0)
        {
            // El padre cierra escritura antes de que el hijo lea.
            printf("[HIJO]: Padre ha cerrado la tuberia...\n");
            exit(EXIT_FAILURE);
        }
        else if (resultado != sizeof(float))
        {
            // Se leyeron menos bytes de los esperados.
            printf("[HIJO]: ERROR al leer de la tubería, bytes leídos incorrectos: %d\n", resultado);
            exit(EXIT_FAILURE);
        }

        printf("[HIJO]: La suma recibida es: %.2f\n", suma);

        if (close(fileDes[0]) == -1) // Cerrar el extremo de lectura
        {
            printf("[HIJO]: Error en close (lectura)");
            exit(EXIT_FAILURE);
        }
        printf("[HIJO]: Tubería cerrada.\n");
        exit(EXIT_SUCCESS);

        // Padre
    default:
        printf("[PADRE]: Mi PID es %d y el PID de mi hijo es %d \n", getpid(), pid);

        if (close(fileDes[0]) == -1) // Padre cierra lectura
        {
            printf("[PADRE]: Error en close (lectura)");
            exit(EXIT_FAILURE);
        }

        srand(time(NULL));

        num1 = (float)rand() / (float)(RAND_MAX / 100.0);
        num2 = (float)rand() / (float)(RAND_MAX / 100.0);
        suma = num1 + num2;

        printf("[PADRE]: Generados números %.2f y %.2f. Enviando suma: %.2f\n", num1, num2, suma);

        resultado = write(fileDes[1], &suma, sizeof(float)); // Escribir la suma en la tubería
        if (resultado == -1)
        {
            printf("[PADRE]: ERROR al escribir en la tubería");
            exit(EXIT_FAILURE);
        }
        else if (resultado != sizeof(float))
        {
            // Se escribieron menos bytes de los esperados.
            printf("[PADRE]: ERROR al escribir en la tubería, bytes escritos incorrectos.\n");
            exit(EXIT_FAILURE);
        }

        if (close(fileDes[1]) == -1)
        {
            printf("[PADRE]: Error en close (escritura)");
            exit(EXIT_FAILURE);
        }
        printf("[PADRE]: Tubería cerrada.\n");

        while ((flag = wait(&status)) > 0) // Esperar al hijo
        {
            if (WIFEXITED(status))
            {
                printf("[PADRE]: Hijo con PID %ld finalizado, status = %d\n", (long int)flag, WEXITSTATUS(status));
            }
            else if (WIFSIGNALED(status))
            {
                printf("[PADRE]: Hijo con PID %ld finalizado al recibir la señal %d\n", (long int)flag, WTERMSIG(status));
            }
        }

        if (flag == (pid_t)-1 && errno == ECHILD)
        {
            printf("[PADRE]: No hay más hijos que esperar. Valor de errno = %d, definido como: %s\n", errno, strerror(errno));
        }
        else
        {
            printf("[PADRE]: Error en el wait");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
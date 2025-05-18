#include <stdio.h>  // printf() scanf()
#include <unistd.h> // Variables como optarg, optfind, etc..
#include <stdlib.h> // abort()
#include <ctype.h>  // isprint()
#include <getopt.h> // long_options[]

int main(int argc, char *argv[])
{
    char *uvalue = NULL;
    int check;

    static struct option long_options[] = // Struct necesario para las versiones largas de las opciónes
        {
            //  {<nombre largo>, <recibe/no recibe argumento>, NULL, <nombre corto>}
            {"user", required_argument, NULL, 'u'},

            {0, 0, 0, 0} // Siempre tiene que estar el último
        };

    while ((check = getopt_long(argc, argv, "u:", long_options, NULL)) != -1)
    {
        switch (check)
        {
        case 'u':
            uvalue = optarg; // optarg == Option Argument
            break;

        case '?': // En caso de no usar getopt_long
                  // // Opción no reconocida o sin argumento
                  // if (optopt == 'u')                                                                     // optopt == Current Option
                  //     printf("La opción %c necesita argumento, valor de opterr = %d\n", optopt, opterr); // opterr == Similar a errno

            // else if (isprint(optopt)) // isprint == es imprimible
            //     printf("La opción \"-%c\" no es válida, valor de opterr = %d\n", optopt, opterr);

            // else
            //     printf("Caracter `\\x%x' invalido. Valor de opterr = %d\n",
            //            optopt, opterr);
            // return -1;
            break;

        default:
            abort();
        }
    }

    int i;
    for (i = optind; i < argc; i++) // optind == Indice por donde se ha quedado getopt()
        printf("\nArgumento \"%s\" de la línea de comandos que NO ES UNA OPCIÓN.\n\n", argv[i]);

    // if (uvalue == NULL) // Para el caso en el que no se ponga la flag -u
    //     uvalue = "ValorPorDefecto";
    return 0;
}
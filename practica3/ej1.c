#include <stdio.h>   // printf() scanf()
#include <unistd.h>  // Variables como optarg, optfind, etc..
#include <stdlib.h>  // abort()
#include <ctype.h>   // isprint()
#include <getopt.h>  // long_options[]
#include <pwd.h>     // struct passwd, getpwuid
#include <grp.h>     // struct group, getgrgid, getgrnam
#include <stdbool.h> // bool

int main(int argc, char *argv[])
{
    char *uvalue = NULL;
    char *gvalue = NULL;
    bool aflag = false;
    int check;

    static struct option long_options[] = // Struct necesario para las versiones largas de las opciónes
        {
            //  {<nombre largo>, <recibe/no recibe argumento>, NULL, <nombre corto>}
            {"user", required_argument, NULL, 'u'},
            {"group", required_argument, NULL, 'g'},
            {"active", no_argument, NULL, 'a'},

            {0, 0, 0, 0} // Siempre tiene que estar el último
        };

    while ((check = getopt_long(argc, argv, "u:g:a", long_options, NULL)) != -1)
    {
        switch (check)
        {
        case 'u':
            uvalue = optarg; // optarg == Option Argument
            break;
        case 'g':
            gvalue = optarg;
            break;
        case 'a':
            aflag = true;
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

    // if (uvalue == NULL) // Para el caso en el que no se ponga una flag con argumento
    //     uvalue = "ValorPorDefecto";

    if (aflag)
    {
        uvalue = getenv("USER");
    }

    if (uvalue != NULL)
    {
        int uid;
        if ((uid = atoi(uvalue)))
        {
            struct passwd *pwd;
            pwd = getpwuid(uid);
            if (pwd == NULL)
            {
                printf("Ha surjido un error en la busqueda del passwd");
                return -1;
            }

            printf("#######################################################\n");
            printf("Información para UID %d:\n", uid);
            printf("  Nombre de usuario: %s\n", pwd->pw_name);
            printf("  Password: %s\n", pwd->pw_passwd);
            printf("  UID: %u\n", pwd->pw_uid);
            printf("  GID: %u\n", pwd->pw_gid);
            printf("  Información GECOS: %s\n", pwd->pw_gecos);
            printf("  Directorio personal: %s\n", pwd->pw_dir);
            printf("  Shell: %s\n", pwd->pw_shell);
            printf("#######################################################\n");
        }
        else
        {
            struct passwd *pwd;
            pwd = getpwnam(uvalue);
            if (pwd == NULL)
            {
                printf("Ha surjido un error en la busqueda del passwd");
                return -1;
            }

            printf("#######################################################\n");
            printf("Información para usuario %s:\n", uvalue);
            printf("  Nombre de usuario: %s\n", pwd->pw_name);
            printf("  Password: %s\n", pwd->pw_passwd);
            printf("  UID: %u\n", pwd->pw_uid);
            printf("  GID: %u\n", pwd->pw_gid);
            printf("  Información GECOS: %s\n", pwd->pw_gecos);
            printf("  Directorio personal: %s\n", pwd->pw_dir);
            printf("  Shell: %s\n", pwd->pw_shell);
            printf("#######################################################\n");
        }
    }

    if (gvalue != NULL)
    {
        int gid;

        if ((gid = atoi(gvalue)))
        {
            struct group *grp;
            grp = getgrgid(gid);
            if (grp == NULL)
            {
                printf("Ha surjido un error en la busqueda del group para GID %d\n", gid);
                return -1;
            }

            printf("#######################################################\n");
            printf("Información para GID %d:\n", gid);
            printf("  Nombre del grupo: %s\n", grp->gr_name);
            printf("  Password: %s\n", grp->gr_passwd);
            printf("  GID: %u\n", grp->gr_gid);
            printf("  Miembros del grupo:\n");
            if (grp->gr_mem != NULL && grp->gr_mem[0] != NULL)
            {
                for (char **member = grp->gr_mem; *member != NULL; member++)
                {
                    printf("    %s\n", *member);
                }
            }
            else
            {
                printf("    (ninguno)\n");
            }
            printf("#######################################################\n");
        }
        else
        {
            struct group *grp;
            grp = getgrnam(gvalue);
            if (grp == NULL)
            {
                printf("Ha surjido un error en la busqueda del group con nombre '%s'\n", gvalue);
                return -1;
            }

            printf("#######################################################\n");
            printf("Información para el grupo %s:\n", gvalue);
            printf("  Nombre del grupo: %s\n", grp->gr_name);
            printf("  Password: %s\n", grp->gr_passwd);
            printf("  GID: %u\n", grp->gr_gid);
            printf("  Miembros del grupo:\n");
            if (grp->gr_mem != NULL && grp->gr_mem[0] != NULL)
            {
                for (char **member = grp->gr_mem; *member != NULL; member++)
                {
                    printf("    %s\n", *member);
                }
            }
            else
            {
                printf("    (ninguno)\n");
            }
            printf("#######################################################\n");
        }
    }

    return 0;
}
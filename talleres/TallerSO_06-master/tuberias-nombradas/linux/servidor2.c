#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "tuberias_nombradas.h"
#include "servidor.h"

static int consecutivos[MAXIMO + 1] = { 0, 0, 0 };

char nombre_peticion[256];
char nombre_solicitud[256];

void manejador_sigquit(int signo) {
    fprintf(stdout, "\nServidor2 terminando por SIGQUIT...\n");
    borrar_tuberia(nombre_peticion);
    borrar_tuberia(nombre_solicitud);
    _exit(EXIT_SUCCESS);
}

int
main(int argc, char* argv[]) {

    int opt;
    int crear = 0;

    strcpy(nombre_peticion, TUBERIA_PETICION);
    strcpy(nombre_solicitud, TUBERIA_SOLICITUD);

    while ((opt = getopt(argc, argv, "cp:s:")) != -1) {
        switch (opt) {
            case 'c':
                crear = 1;
                break;
            case 'p':
                strcpy(nombre_peticion, optarg);
                break;
            case 's':
                strcpy(nombre_solicitud, optarg);
                break;
            default:
                fprintf(stderr, "Uso: %s [-c] [-p <peticion-nombre>] [-s <solicitud-nombre>]\n", argv[0]);
                exit(1);
        }
    }

    struct sigaction sa;
    sa.sa_handler = manejador_sigquit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGQUIT, &sa, NULL);

    if (crear) {
        crear_tuberia(nombre_peticion);
        crear_tuberia(nombre_solicitud);
    }

    int df_lectura = -2;

    for (;;) {

        if (df_lectura == -2 && (df_lectura = open(nombre_peticion, O_RDONLY)) == -1) {
            fprintf(stderr, "Error abriendo: %s %d %s",
                    nombre_peticion, errno, strerror(errno));
            _exit(EXIT_FAILURE);
        }

        char amortiguador_entrada[LONGITUD_MENSAJE_PETICION];
        fprintf(stdout, "Preparando lectura\n");
        ssize_t caracteres_leidos = read(df_lectura, amortiguador_entrada, LONGITUD_MENSAJE_PETICION);

        if (caracteres_leidos < -1) {
            close(df_lectura);
            break;
        }

        if (caracteres_leidos > 0) {

            amortiguador_entrada[FIN_PID] = amortiguador_entrada[LONGITUD_MENSAJE_PETICION - 1] = '\0';
            pid_t proceso_cliente = atoi(amortiguador_entrada);
            Consecutivo_t consecutivo = atoi(&amortiguador_entrada[FIN_PID + 1]);

            fprintf(stdout, "Peticion: %d en la cola: %d\n", proceso_cliente, consecutivo);

            int df_escritura;

            if ((df_escritura = open(nombre_solicitud, O_WRONLY)) == -1) {
                fprintf(stderr, "Error abriendo: %s %d %s",
                        nombre_solicitud, errno, strerror(errno));
                _exit(EXIT_FAILURE);
            }

            char amortiguador_salida[LONGITUD_MENSAJE_SOLICITUD];

            bzero(amortiguador_salida, LONGITUD_MENSAJE_SOLICITUD);
            sprintf(amortiguador_salida, "%06d %06d\n", proceso_cliente, consecutivos[consecutivo]++);
            write(df_escritura, amortiguador_salida, LONGITUD_MENSAJE_SOLICITUD);

            close(df_escritura);
        }
        else {
            fprintf(stdout, "No se leyeron caracteres %zd.\n", caracteres_leidos);
            close(df_lectura);
            df_lectura = -2;
        }
    }

    return EXIT_SUCCESS;
}

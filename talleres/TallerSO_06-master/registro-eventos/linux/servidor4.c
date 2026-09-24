#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <sys/stat.h>
#include "tuberias_nombradas.h"
#include "servidor.h"

static int consecutivos[MAXIMO + 1] = { 0, 0, 0 };

char nombre_peticion[256];
char nombre_solicitud[256];

void manejador_sigquit(int signo) {
    syslog(LOG_INFO, "Servidor4 terminando por SIGQUIT");
    borrar_tuberia(nombre_peticion);
    borrar_tuberia(nombre_solicitud);
    closelog();
    _exit(EXIT_SUCCESS);
}

void convertir_en_demonio(void) {
    pid_t pid = fork();

    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        // El padre termina, el hijo sigue como demonio
        exit(EXIT_SUCCESS);
    }

    // A partir de aqui solo corre el hijo (el demonio)
    setsid();
    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

int
main(int argc, char* argv[]) {

    int opt;
    int crear = 0;
    int demonio = 0;

    strcpy(nombre_peticion, TUBERIA_PETICION);
    strcpy(nombre_solicitud, TUBERIA_SOLICITUD);

    while ((opt = getopt(argc, argv, "dcp:s:")) != -1) {
        switch (opt) {
            case 'd':
                demonio = 1;
                break;
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
                fprintf(stderr, "Uso: %s [-d] [-c] [-p <peticion-nombre>] [-s <solicitud-nombre>]\n", argv[0]);
                exit(1);
        }
    }

    openlog("servidor4", LOG_CONS | LOG_PID, LOG_USER);

    if (demonio) {
        convertir_en_demonio();
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

    syslog(LOG_INFO, "Servidor4 iniciado");

    int df_lectura = -2;

    for (;;) {

        if (df_lectura == -2 && (df_lectura = open(nombre_peticion, O_RDONLY)) == -1) {
            syslog(LOG_ERR, "Error abriendo %s: %d %s", nombre_peticion, errno, strerror(errno));
            _exit(EXIT_FAILURE);
        }

        char amortiguador_entrada[LONGITUD_MENSAJE_PETICION];
        ssize_t caracteres_leidos = read(df_lectura, amortiguador_entrada, LONGITUD_MENSAJE_PETICION);

        if (caracteres_leidos < -1) {
            close(df_lectura);
            break;
        }

        if (caracteres_leidos > 0) {

            amortiguador_entrada[FIN_PID] = amortiguador_entrada[LONGITUD_MENSAJE_PETICION - 1] = '\0';
            pid_t proceso_cliente = atoi(amortiguador_entrada);
            Consecutivo_t consecutivo = atoi(&amortiguador_entrada[FIN_PID + 1]);

            syslog(LOG_INFO, "Peticion del cliente %d en la cola %d", proceso_cliente, consecutivo);

            int df_escritura;

            if ((df_escritura = open(nombre_solicitud, O_WRONLY)) == -1) {
                syslog(LOG_ERR, "Error abriendo %s: %d %s", nombre_solicitud, errno, strerror(errno));
                _exit(EXIT_FAILURE);
            }

            char amortiguador_salida[LONGITUD_MENSAJE_SOLICITUD];

            bzero(amortiguador_salida, LONGITUD_MENSAJE_SOLICITUD);
            sprintf(amortiguador_salida, "%06d %06d\n", proceso_cliente, consecutivos[consecutivo]++);
            write(df_escritura, amortiguador_salida, LONGITUD_MENSAJE_SOLICITUD);

            close(df_escritura);
        }
        else {
            syslog(LOG_INFO, "No se leyeron caracteres");
            close(df_lectura);
            df_lectura = -2;
        }
    }

    closelog();
    return EXIT_SUCCESS;
}

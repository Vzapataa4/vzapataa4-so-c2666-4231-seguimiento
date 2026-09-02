#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int contador = 0;

const char* nombre_senal(int signo) {
    switch (signo) {
        case SIGHUP:  return "SIGHUP";
        case SIGQUIT: return "SIGQUIT";
        default:      return "DESCONOCIDA";
    }
}

void manejador_senal(int signo) {
    ++contador;
    fprintf(stdout, "Senal capturada: %s (%d) - contador: %d\n",
            nombre_senal(signo), signo, contador);
}

int main(int argc, char *argv[]) {
    struct sigaction sa;
    sa.sa_handler = manejador_senal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    pid_t pId = getpid();

    while (contador < 4) {
        fprintf(stdout, "Proceso: %d esperando SIGHUP o SIGQUIT\n", pId);
        sleep(4);
    }

    fprintf(stdout, "Terminado despues de: %d senales\n", contador);
    return 0;
}

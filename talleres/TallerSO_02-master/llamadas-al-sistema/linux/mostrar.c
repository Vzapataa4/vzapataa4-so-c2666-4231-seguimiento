#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define SIZE_BUFFER 12

void mostrar_fichero(char *nombre) {
    int fd;
    char buffer[SIZE_BUFFER + 1];

    bzero(buffer, SIZE_BUFFER);

    fd = open(nombre, O_RDONLY);

    if (fd == -1) {
        fprintf(stderr, "Error abriendo archivo %s: %d\n", nombre, errno);
        return;
    }

    int tam;
    tam = read(fd, buffer, SIZE_BUFFER);

    if (tam == -1) {
        fprintf(stderr, "Error leyendo archivo %s: %d\n", nombre, errno);
        close(fd);
        return;
    }

    buffer[tam] = '\0';
    while (tam != 0) {
        write(STDOUT_FILENO, buffer, tam);
        tam = read(fd, buffer, SIZE_BUFFER);

        if (tam == -1) {
            fprintf(stderr, "Error leyendo archivo %s: %d\n", nombre, errno);
            close(fd);
            return;
        }
        buffer[tam] = '\0';
    }

    close(fd);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s fichero1 [fichero2] [fichero3] ...\n", argv[0]);
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        mostrar_fichero(argv[i]);
    }

    return 0;
}

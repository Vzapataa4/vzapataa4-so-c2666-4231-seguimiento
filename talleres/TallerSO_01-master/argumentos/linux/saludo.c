#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int opt;
    int despedida = 0;

    while ((opt = getopt(argc, argv, "hsd")) != -1) {
        switch (opt) {
            case 'h':
                printf("Uso: ./saludo -h\n");
                printf("     ./saludo [-s|-d] <nombre>\n");
                return 0;
            case 's':
                despedida = 0;
                break;
            case 'd':
                despedida = 1;
                break;
            default:
                printf("Uso: ./saludo -h\n");
                printf("     ./saludo [-s|-d] <nombre>\n");
                return 1;
        }
    }

    if (optind >= argc) {
        printf("Uso: ./saludo -h\n");
        printf("     ./saludo [-s|-d] <nombre>\n");
        return 1;
    }

    char *nombre = argv[optind];

    if (despedida) {
        printf("Adios %s\n", nombre);
    } else {
        printf("Hola %s\n", nombre);
    }

    return 0;
}

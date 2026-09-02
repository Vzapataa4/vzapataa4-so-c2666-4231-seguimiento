#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <fichero> <cadena-traduccion> <linea-modulo>\n", argv[0]);
        exit(1);
    }

    char *fichero = argv[1];
    char traduccion[256];
    strncpy(traduccion, argv[2], sizeof(traduccion) - 1);
    int modulo = atoi(argv[3]);

    FILE *f = fopen(fichero, "r");
    if (f == NULL) {
        perror("Error abriendo fichero");
        exit(1);
    }

    char *set1 = strtok(traduccion, " ");
    char *set2 = strtok(NULL, " ");

    int pipe_eco_cat[2];
    int pipe_cat_tr[2];
    int pipe_tr_eco[2];

    pipe(pipe_eco_cat);
    pipe(pipe_cat_tr);
    pipe(pipe_tr_eco);

    pid_t cat_pid = fork();
    if (cat_pid == 0) {
        dup2(pipe_eco_cat[0], 0);
        dup2(pipe_cat_tr[1], 1);
        close(pipe_eco_cat[0]);
        close(pipe_eco_cat[1]);
        close(pipe_cat_tr[0]);
        close(pipe_cat_tr[1]);
        close(pipe_tr_eco[0]);
        close(pipe_tr_eco[1]);
        execlp("cat", "cat", NULL);
        _exit(1);
    }

    pid_t tr_pid = fork();
    if (tr_pid == 0) {
        dup2(pipe_cat_tr[0], 0);
        dup2(pipe_tr_eco[1], 1);
        close(pipe_eco_cat[0]);
        close(pipe_eco_cat[1]);
        close(pipe_cat_tr[0]);
        close(pipe_cat_tr[1]);
        close(pipe_tr_eco[0]);
        close(pipe_tr_eco[1]);
        if (set2 != NULL) {
            execlp("tr", "tr", set1, set2, NULL);
        } else {
            execlp("tr", "tr", set1, NULL);
        }
        _exit(1);
    }

    close(pipe_eco_cat[0]);
    close(pipe_cat_tr[0]);
    close(pipe_cat_tr[1]);
    close(pipe_tr_eco[1]);

    pid_t writer_pid = fork();
    if (writer_pid == 0) {
        close(pipe_tr_eco[0]);
        char line[1024];
        while (fgets(line, sizeof(line), f) != NULL) {
            write(pipe_eco_cat[1], line, strlen(line));
        }
        fclose(f);
        close(pipe_eco_cat[1]);
        _exit(0);
    }
    close(pipe_eco_cat[1]);
    fclose(f);

    FILE *in = fdopen(pipe_tr_eco[0], "r");
    char buffer[1024];
    int numero_linea = 0;
    while (fgets(buffer, sizeof(buffer), in) != NULL) {
        numero_linea++;
        if (modulo == 0 || numero_linea % modulo != 0) {
            fputs(buffer, stdout);
        }
    }
    fclose(in);

    int status;
    waitpid(cat_pid, &status, 0);
    waitpid(tr_pid, &status, 0);
    waitpid(writer_pid, &status, 0);

    return 0;
}

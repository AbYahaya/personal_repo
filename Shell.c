#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024

void execute_command(char *command) {
    pid_t pid, wpid;
    int status;

    pid = fork();

    if (pid == 0) {
        // Child process
        char *args[] = {"/bin/sh", "-c", command, NULL};
        execvp(args[0], args);
        perror("hsh");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Forking error
        perror("hsh");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

void interactive_mode() {
    char input[MAX_INPUT_SIZE];

    while (1) {
        printf("($) ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        // Remove newline character from input
        input[strcspn(input, "\n")] = '\0';

        // Exit if the user enters "exit"
        if (strcmp(input, "exit") == 0) {
            break;
        }

        execute_command(input);
    }
}

void batch_mode(FILE *file) {
    char input[MAX_INPUT_SIZE];

    while (fgets(input, sizeof(input), file) != NULL) {
        // Remove newline character from input
        input[strcspn(input, "\n")] = '\0';
        execute_command(input);
    }
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        // Interactive mode
        interactive_mode();
    } else if (argc == 2) {
        // Batch mode
        FILE *file = fopen(argv[1], "r");
        if (file == NULL) {
            perror("hsh");
            exit(EXIT_FAILURE);
        }
        batch_mode(file);
        fclose(file);
    } else {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    return 0;
}

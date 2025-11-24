#include <stdio.h>
#include <stdlib.h>
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define N 12
#define MAXARGS 20

extern char **environ;

char *allowed[N] = {
    "cp","touch","mkdir","ls","pwd","cat","grep","chmod","diff",
    "cd","exit","help"
};

int isAllowed(const char*cmd) {
    for (int i = 0; i < N; i++) {
        if (strcmp(cmd, allowed[i]) == 0)
            return 1;
    }
    return 0;
}

int main() {

    char line[256];

    while (1) {

        fprintf(stderr, "rsh>");

        if (fgets(line, 256, stdin) == NULL)
            continue;

        if (strcmp(line, "\n") == 0)
            continue;

        line[strlen(line) - 1] = '\0';

        // tokenize
        char *argv[MAXARGS + 1];
        int argc = 0;

        char *tok = strtok(line, " ");
        while (tok != NULL && argc < MAXARGS) {
            argv[argc++] = tok;
            tok = strtok(NULL, " ");
        }
        argv[argc] = NULL;

        if (argc == 0)
            continue;

        char *cmd = argv[0];

        // exit
        if (strcmp(cmd, "exit") == 0) {
            return 0;
        }

        // help 
        if (strcmp(cmd, "help") == 0) {
            printf("The allowed commands are:\n");
            printf("1: cp\n2: touch\n3: mkdir\n4: ls\n5: pwd\n6: cat\n7: grep\n8: chmod\n9: diff\n10: cd\n11: exit\n12: help\n");
            continue;
        }

        // cd
        if (strcmp(cmd, "cd") == 0) {
            if (argc > 2) {
                printf("-rsh: cd: too many arguments\n");
                continue;
            }
            if (argc == 2) {
                if (chdir(argv[1]) != 0) {
                    perror("-rsh: cd");
                }
            }
            continue;
        }

        // external commands
        if (isAllowed(cmd) && strcmp(cmd, "cd") != 0 &&
            strcmp(cmd, "exit") != 0 && strcmp(cmd, "help") != 0) {

            pid_t pid;
            int status;

            if (posix_spawnp(&pid, cmd, NULL, NULL, argv, environ) != 0) {
                printf("NOT ALLOWED!\n");
                continue;
            }

            waitpid(pid, &status, 0);
            continue;
        }
        printf("NOT ALLOWED!\n");
    }

    return 0;
}

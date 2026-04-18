#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main() {
    pid_t child_pid, grandchild_pid;
    int i;
    printf("1. Parent process ID: %d\n", getpid());
    child_pid = fork();
    if (child_pid < 0) {
        perror("Fork failed");
        exit(1);
    }
    if (child_pid == 0) {
        // This is the child process
        printf("2. Child process ID: %d\n", getpid());
        for (i = 0; i < 3; i++) {
            grandchild_pid = fork();

            if (grandchild_pid < 0) {
                perror("Fork failed");
                exit(1);
            }
            if (grandchild_pid == 0) {
                // Grandchild process
                printf("%d. Grand Child process ID: %d\n", 3 + i, getpid());
                exit(0); // grandchild exits 
            }      
        }     
        for (i = 0; i < 3; i++) {
            wait(NULL);
        }
        exit(0); // child exits 
    } else {
        // Parent waits for child to finish
        wait(NULL);
    }
    return 0;
}

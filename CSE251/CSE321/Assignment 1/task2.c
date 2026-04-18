#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid, grandchild_pid;

    child_pid = fork(); // Create child

    if (child_pid == 0) {
        // Child process
        grandchild_pid = fork(); // Create grandchild

        if (grandchild_pid == 0) {
            // Grandchild process
            printf("I am grandchild\n");
        } else {
            wait(NULL); // Wait for grandchild
            printf("I am child\n");
        }
    } else {
        wait(NULL); // Wait for child
        printf("I am parent\n");
    }

    return 0;
}

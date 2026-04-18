#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

struct shared {
    char sel[100];
    int b;
};

int main() {
    int fd[2];
    pid_t pid;
    key_t key = IPC_PRIVATE;

   
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

 
    int shmid = shmget(key, sizeof(struct shared), IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    struct shared *shm;
    shm = (struct shared*) shmat(shmid, NULL, 0);
    if (shm == (void*) -1) {
        perror("shmat");
        exit(1);
    }

    shm->b = 1000; 

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {

        close(fd[0]); 

        printf("Provide Your Input From Given Options:\n");
        printf("1. Type a to Add Money\n");
        printf("2. Type w to Withdraw Money\n");
        printf("3. Type c to Check Balance\n");

        scanf("%s", shm->sel);
        printf("Your selection: %s\n", shm->sel);

        write(fd[1], "Thank you for using\n", 21);
        close(fd[1]);

        exit(0);
    }
    else {
        
        wait(NULL);

        if (strcmp(shm->sel, "a") == 0) {
            int amt;
            printf("Enter amount to be added:\n");
            scanf("%d", &amt);

            if (amt > 0) {
                shm->b += amt;
                printf("Balance added successfully\n");
                printf("Updated balance after addition:\n%d\n", shm->b);
            } else {
                printf("Adding failed, Invalid amount\n");
            }
        }
        else if (strcmp(shm->sel, "w") == 0) {
            int amt;
            printf("Enter amount to be withdrawn:\n");
            scanf("%d", &amt);

            if (amt > 0 && amt < shm->b) {
                shm->b -= amt;
                printf("Balance withdrawn successfully\n");
                printf("Updated balance after withdrawal:\n%d\n", shm->b);
            } else {
                printf("Withdrawal failed, Invalid amount\n");
            }
        }
        else if (strcmp(shm->sel, "c") == 0) {
            printf("Your current balance is:\n%d\n", shm->b);
        }
        else {
            printf("Invalid selection\n");
        }

        close(fd[1]); 
        char buf[100];
        read(fd[0], buf, sizeof(buf));
        printf("%s", buf);
        close(fd[0]);


        shmctl(shmid, IPC_RMID, NULL);
    }

    return 0;
}

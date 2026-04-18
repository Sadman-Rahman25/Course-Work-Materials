#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct msg {
    long int type;
    char txt[20];
};

int main() {
    key_t key;
    int msgid;
    struct msg message;

    key = ftok(".", 'x');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    msgid = msgget(key, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    char workspace[20];
    printf("Please enter the workspace name:\n");
    scanf("%s", workspace);

    if (strcmp(workspace, "cse321") != 0) {
        printf("Invalid workspace name\n");
        msgctl(msgid, IPC_RMID, NULL); 
        exit(0);
    }

    strcpy(message.txt, workspace);
    message.type = 1; 
    if (msgsnd(msgid, &message, sizeof(message.txt), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
    printf("Workspace name sent to otp generator from log in: %s\n", workspace);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        struct msg recvmsg, sendmsg;

        if (msgrcv(msgid, &recvmsg, sizeof(recvmsg.txt), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("OTP generator received workspace name from log in: %s\n", recvmsg.txt);

        int otp = getpid();
        sprintf(sendmsg.txt, "%d", otp);

        sendmsg.type = 2;
        msgsnd(msgid, &sendmsg, sizeof(sendmsg.txt), 0);
        printf("OTP sent to log in from OTP generator: %s\n", sendmsg.txt);

        sendmsg.type = 3;
        msgsnd(msgid, &sendmsg, sizeof(sendmsg.txt), 0);
        printf("OTP sent to mail from OTP generator: %s\n", sendmsg.txt);

        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("fork");
            exit(1);
        }

        if (pid2 == 0) {
            struct msg mrecv, msend;

            if (msgrcv(msgid, &mrecv, sizeof(mrecv.txt), 3, 0) == -1) {
                perror("msgrcv");
                exit(1);
            }
            printf("Mail received OTP from OTP generator: %s\n", mrecv.txt);

            msend.type = 4;
            strcpy(msend.txt, mrecv.txt);
            msgsnd(msgid, &msend, sizeof(msend.txt), 0);
            printf("OTP sent to log in from mail: %s\n", msend.txt);

            exit(0);
        } else {
            wait(NULL); 
            exit(0);  
        }
    } else {
       
        wait(NULL); 

        struct msg r1, r2;

        if (msgrcv(msgid, &r1, sizeof(r1.txt), 2, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("Log in received OTP from OTP generator: %s\n", r1.txt);

        if (msgrcv(msgid, &r2, sizeof(r2.txt), 4, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("Log in received OTP from mail: %s\n", r2.txt);

        if (strcmp(r1.txt, r2.txt) == 0) {
            printf("OTP Verified\n");
        } else {
            printf("OTP Incorrect\n");
        }

        msgctl(msgid, IPC_RMID, NULL);
    }

    return 0;
}

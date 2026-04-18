#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main() {
    pid_t a, b, c;
    pid_t original_pid = getpid();

    int fd = open("process_count.tmp", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }

    a = fork();
    if (a < 0) {
         perror("fork a failed"); exit(1); }

    b = fork();
    if (b < 0) { 
        perror("fork b failed"); exit(1); }

    c = fork();
    if (c < 0) { 
        perror("fork c failed"); exit(1); }

    write(fd, "+", 1);
    if (getpid() % 2 != 0) {
        pid_t extra = fork();
        if (extra < 0) {
            perror("extra fork failed");
            exit(1);
        }
        if (extra == 0) {
            write(fd, "+", 1);
            exit(0);
        }
    }

    sleep(1);

    if (getpid() == original_pid) {
        lseek(fd, 0, SEEK_SET); 
        char buf[1024];
        int bytes = read(fd, buf, sizeof(buf));
        buf[bytes] = '\0';
        int total_processes = bytes;
        printf("Total processes created: %d\n", total_processes);
    }

    close(fd);
    return 0;
}

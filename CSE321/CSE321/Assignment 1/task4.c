#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <string.h>

int is_number(const char *str) {
    if (str[0] == '\0') return 0; 
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}
void bubble_sort_desc(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (arr[j] < arr[j + 1]) {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}
void print_odd_even(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        if (arr[i] % 2 == 0)
            printf("%d is even\n", arr[i]);
        else
            printf("%d is odd\n", arr[i]);
    }
}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Please enter some numbers as command line arguments.\n");
        return 1;
    }
    int count = argc - 1;
    int numbers[count];

    for (int i = 1; i <= count; i++) {
        if (!is_number(argv[i])) {
            printf("Invalid input: %s\n", argv[i]);
            return 1;
        }
        numbers[i - 1] = atoi(argv[i]);
    }
    pid_t pid = fork();

    if (pid == 0) {
        bubble_sort_desc(numbers, count);
        printf("Sorted array in descending order (Child Process):\n");
        for (int i = 0; i < count; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");
        exit(0);
    } else if (pid > 0) {
        wait(NULL);
        bubble_sort_desc(numbers, count);
        printf("Odd/Even check (Parent Process):\n");
        print_odd_even(numbers, count);
    } else {
        printf("Failed to create child process.\n");
        return 1;
    }
    return 0;
}

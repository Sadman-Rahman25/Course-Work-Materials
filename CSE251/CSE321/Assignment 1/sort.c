#include <stdio.h>
#include <string.h>
#include <ctype.h>

int is_number(const char *str) {
    for (int i = 0; i < (int)strlen(str); i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

void sort_desc(int arr[], int n) {
    for (int i = 0; i < n -1; i++) {
        for (int j = i +1; j < n; j++) {
            if (arr[i] < arr[j]) {
                int temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s num1 num2 num3 ...\n", argv[0]);
        return 1;
    }

    int n = argc -1;
    int arr[n];

    for (int i = 1; i <= n; i++) {
        if (!is_number(argv[i])) {
            printf("Invalid number: %s\n", argv[i]);
            return 1;
        }
        int num = 0;
        for (int j = 0; j < (int)strlen(argv[i]); j++) {
            num = num * 10 + (argv[i][j] - '0');
        }

        arr[i - 1] = num;
    }

    sort_desc(arr, n);

    printf("Sorted array in descending order: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int is_number(const char *str) {
    if (str[0] == '\0') return 0; 
    for (int i = 0; i < (int)strlen(str); i++) {
        if (!isdigit(str[i])) return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s num1 num2 num3 ...\n", argv[0]);
        return 1;
    }

    int n = argc - 1;
    int arr[n];

    for (int i = 1; i <= n; i++) {
        if (!is_number(argv[i])) {
            printf("Invalid input: %s\n", argv[i]);
            return 1; 
        }

        
        int num = 0;
        for (int j = 0; j < (int)strlen(argv[i]); j++) {
            num = num * 10 + (argv[i][j] - '0');
        }
        arr[i - 1] = num;
    }

    for (int i = 0; i < n; i++) {
        if (arr[i] % 2 == 0)
            printf("%d is even\n", arr[i]);
        else
            printf("%d is odd\n", arr[i]);
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void write_to_file(FILE *file_pointer) {
    char user_input[256];

    while (1) {
        printf("Enter a string (-1 to stop): ");
        fgets(user_input, sizeof(user_input), stdin);

        user_input[strcspn(user_input, "\n")] = '\0';


        if (strcmp(user_input, "-1") == 0) {
            break;
        }

        fprintf(file_pointer, "%s\n", user_input);
    }
}

int main(int argument_count, char *argument_values[]) {
    if (argument_count != 2) {
        printf("Usage: %s <filename>\n", argument_values[0]);
        return 1;
    }

    FILE *file_pointer = fopen(argument_values[1], "a");
    if (!file_pointer) {
        perror("Error opening file");
        return 1;
    }

    write_to_file(file_pointer);

    fclose(file_pointer);
    printf("Data written to file: %s\n", argument_values[1]);

    return 0;
}


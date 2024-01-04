#include <stdio.h>

int main() {
    // File path
    const char *filePath = "output.txt";

    // String to append
    const char *textToAppend = "Hello, World!\n";

    // Open the file in append mode
    FILE *file = fopen(filePath, "a");

    if (file == NULL) {
        perror("Error opening the file");
        return 1;
    }

    // Append the text
    fprintf(file, "%s", textToAppend);

    // Close the file
    fclose(file);

    printf("Appended 'Hello, World!' to %s\n", filePath);

    return 0;
}


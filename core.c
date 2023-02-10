#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 32

int main(int argc, char *argv[]) {
    // if (argc < 2) {
    //     printf("Usage: %s filename\n", argv[0]);
    //     return 1;
    // }

    FILE *fp;
    char buffer[BUFFER_SIZE];

    fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening file");
        return 1;
    }

    while (fgets(buffer, BUFFER_SIZE, fp)) {
        printf("%s", buffer);
    }

    fclose(fp);
    return 0;
}

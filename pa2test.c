#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // For malloc(), free(), exit()
#include <fcntl.h>      // For open(), O_RDWR, O_CREAT
#include <unistd.h>     // For read(), write(), lseek(), close()
#include <string.h>     // For strerror(), strlen(), strcspn()
#include <errno.h>      // For error handling (errno)
#include <ctype.h>      // This is for tolower() to handle uppercase input

// Function to handle reading from the file
void handle_read(int fd) {
    int bytes_to_read;
    char *buffer;

    printf("Enter the number of bytes you want to read: ");
    if (scanf("%d", &bytes_to_read) != 1) {
        printf("Invalid input.\n");
        return;
    }

    buffer = (char *)malloc(bytes_to_read + 1);
    if (!buffer) {
        printf("Memory allocation failed.\n");
        return;
    }

    ssize_t bytes_read = read(fd, buffer, bytes_to_read);
    
    if (bytes_read < 0) { 
        printf("Error reading file: %s\n", strerror(errno));
    } else if (bytes_read == 0) { 
        printf("No data to read (your file may be empty).\n");
    } else {
        buffer[bytes_read] = '\0'; 
        printf("Read data: %s\n\n", buffer);
    }

    free(buffer);
}




// Function to handle writing to the file
void handle_write(int fd) {
    char buffer[1024];

    printf("Enter the string you want to write: ");
    
    getchar();  // Consume leftover newline

    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        printf("Error reading input.\n");
        return;
    }

    strncat(buffer, "\n", sizeof(buffer) - strlen(buffer) - 1);

    ssize_t bytes_written = write(fd, buffer, strlen(buffer));
    
    if (bytes_written < 0) { 
        printf("Error writing to file: %s\n", strerror(errno));
    }
}

// Function to handle seeking within the file
void handle_seek(int fd) {
    int offset, whence;  

    printf("Enter an offset value: ");
    if (scanf("%d", &offset) != 1) {
        printf("Invalid offset.\n");
        return;
    }

    printf("Enter a value for whence (p.s. this language is archaic)  >>\n (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): ");
    if (scanf("%d", &whence) != 1 || (whence < 0 || whence > 2)) {
        printf("Invalid value for whence. You should LEARN TO COUNT PROPERLY!!! :P\n");
        return;
    }

    printf("Seeking to offset %d using whence (still archaic) %d...\n", offset, whence);

    off_t new_pos = lseek(fd, offset, whence);
    
    if (new_pos == (off_t)-1) {
        printf("Error seeking in file: %s\n", strerror(errno));
    } else {
        printf("New file position: %ld\n", (long)new_pos);
    }
}

// Main function to handle user input loop
int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[1], O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        printf("Error opening file '%s': %s\n", argv[1], strerror(errno));
        return 1;
    }

    printf("File '%s' opened successfully.\n", argv[1]);

    char option;
    while (1) {
        printf(" -------------------------------------------------\n|\tWhat would you like to do \t\t  |\n -------------------------------------------------\n -> R/r for read\n -> W/w for write\n -> S/s for seek\n -> CRTL + D to Exit\nPlease enter your choice:  ");
        
        int read_status = scanf(" %c", &option);

        if (read_status == EOF) {
            printf("\nCTRL+D key press has been detected, exiting the program.\nGoodbye Forever.\n");
            break;
        }

        option = tolower(option); 

        switch (option) {
            case 'r':
                handle_read(fd);
                break;
            case 'w':
                handle_write(fd);
                break;
            case 's':
                handle_seek(fd);
                break;
            default:
                printf("Invalid option. Please try again.\nUnless this is your second try in which case Look carefully.");
        }
    }

    close(fd);
    return 0;
}

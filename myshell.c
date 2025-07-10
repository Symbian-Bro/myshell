// Mason Berry
// CSE-3320-001
// Lab 1
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define MAX_FILES 1024
#define NAME_MAX 255
#define ITEMS_PER_PAGE 5

// Struct to represent a file or directory entry, storing its name, type (file or directory), size, and modification time
typedef struct {
    char name[NAME_MAX];      // Name of the file or directory
    int is_directory;         // 1 if the entry is a directory, 0 if it is a file
    off_t size;               // Size of the file (not relevant for directories)
    time_t mtime;             // Last modification time of the file or directory
} FileEntry;

// Function declarations
void display_menu();
void list_directory(const char *path, FileEntry items[], int *total_count);
void handle_command(char command, FileEntry items[], int total_count, int *current_page);
void display_page(FileEntry items[], int total_count, int current_page);

// Sorting function declarations
int compare_by_size(const void *a, const void *b);
int compare_by_mtime(const void *a, const void *b);

int main(int argc, char *argv[]) {
    FileEntry items[MAX_FILES];      // Array to store files and directories in the current directory
    int total_count = 0;             // Total number of files and directories in the current directory
    int current_page = 0;            // Current page number for paginated display
    char current_dir[NAME_MAX];      // Buffer to store the current directory path
    char command;

    // If a starting directory is provided as a command-line argument, change to that directory
    if (argc > 1) {
        strcpy(current_dir, argv[1]);
        chdir(current_dir);
    } else {
        // Otherwise, get the current working directory
        getcwd(current_dir, NAME_MAX);
    }

    while (1) {
        // Display current directory and list files/directories
        getcwd(current_dir, NAME_MAX);
        printf("Current Directory: %s\n", current_dir);
        time_t now = time(NULL);
        printf("It is now: %s", ctime(&now));

        // List directory contents and reset the total count of files/directories
        list_directory(current_dir, items, &total_count);

        // Display the current page of items
        display_page(items, total_count, current_page);

        // Display the command menu
        display_menu();

        // Get the user's command input
        printf("Enter command: ");
        scanf(" %c", &command);  // The space before %c consumes any leftover whitespace characters

        // Handle the command entered by the user
        handle_command(command, items, total_count, &current_page);

        // Clear the screen before the next loop (optional)
        system("clear");
    }

    return 0;
}

// Display the available commands to the user
void display_menu() {
    printf("\nOperations:\n");
    printf("N - Next Page\n");
    printf("P - Previous Page\n");
    printf("E - Edit File\n");
    printf("R - Run Program\n");
    printf("C - Change Directory\n");
    printf("S - Sort Directory Listing\n");
    printf("Q - Quit\n");
}

// List the contents of the given directory and store information in the FileEntry array
void list_directory(const char *path, FileEntry items[], int *total_count) {
    DIR *d;
    struct dirent *de;
    struct stat file_stat;
    char full_path[NAME_MAX * 2];  // Increased buffer size to handle longer paths

    *total_count = 0;  // Reset total count for the new directory

    d = opendir(path);
    if (d == NULL) {
        perror("Failed to open directory");
        return;
    }

    // Add ".." to allow moving up one directory
    strcpy(items[*total_count].name, "..");
    items[*total_count].is_directory = 1;  // Mark as directory
    items[*total_count].size = 0;
    items[*total_count].mtime = 0;
    (*total_count)++;

    // Read each entry in the directory
    while ((de = readdir(d)) != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", path, de->d_name);
        stat(full_path, &file_stat);

        if (de->d_type == DT_DIR) {  // Directory
            if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
                strcpy(items[*total_count].name, de->d_name);
                items[*total_count].is_directory = 1;
                items[*total_count].size = file_stat.st_size;
                items[*total_count].mtime = file_stat.st_mtime;
                (*total_count)++;
            }
        } else if (de->d_type == DT_REG) {  // Regular file
            strcpy(items[*total_count].name, de->d_name);
            items[*total_count].is_directory = 0;
            items[*total_count].size = file_stat.st_size;
            items[*total_count].mtime = file_stat.st_mtime;
            (*total_count)++;
        }
    }

    closedir(d);

    // Default sorting: files first, sorted by size
    qsort(items, *total_count, sizeof(FileEntry), compare_by_size);
}

// Display a paginated list of files and directories
void display_page(FileEntry items[], int total_count, int current_page) {
    int start = current_page * ITEMS_PER_PAGE;
    int end = start + ITEMS_PER_PAGE;

    if (start >= total_count) {
        printf("No more items to display.\n");
        return;
    }

    // Display the items on the current page
    for (int i = start; i < end && i < total_count; i++) {
        if (items[i].is_directory) {
            printf("[DIR]  %d. %s\n", i, items[i].name);
        } else {
            printf("[FILE] %d. %s\n", i, items[i].name);
        }
    }
}

// Handle user commands: pagination, editing, running programs, sorting, changing directories, etc.
void handle_command(char command, FileEntry items[], int total_count, int *current_page) {
    int index;
    char cmd[NAME_MAX + 10];  // To hold commands like "nano <filename>"
    char sort_choice;

    switch (command) {
        case 'N':  // Next page of the directory listing
            if ((*current_page + 1) * ITEMS_PER_PAGE < total_count) {
                (*current_page)++;
            } else {
                printf("You are on the last page.\n");
            }
            break;
        case 'P':  // Previous page of the directory listing
            if (*current_page > 0) {
                (*current_page)--;
            } else {
                printf("You are on the first page.\n");
            }
            break;
        case 'S':  // Sort the directory listing by size or date
            printf("Sort by (s)ize or (d)ate? ");
            scanf(" %c", &sort_choice);

            if (sort_choice == 's') {
                qsort(items, total_count, sizeof(FileEntry), compare_by_size);
            } else if (sort_choice == 'd') {
                qsort(items, total_count, sizeof(FileEntry), compare_by_mtime);
            } else {
                printf("Invalid choice. Sorting aborted.\n");
            }

            // Redisplay the sorted list from the first page
            *current_page = 0;
            display_page(items, total_count, *current_page);
            break;
        case 'E':  // Edit a file using nano
            printf("Enter file number to edit: ");
            scanf("%d", &index);
            if (index >= 0 && index < total_count && !items[index].is_directory) {
                snprintf(cmd, sizeof(cmd), "nano %s", items[index].name);
                system(cmd);
            } else {
                printf("Invalid file number\n");
            }
            break;
        case 'R':  // Run an executable file
            printf("Enter file number to run: ");
            scanf("%d", &index);

            if (index >= 0 && index < total_count && !items[index].is_directory) {
                struct stat file_stat;
                if (stat(items[index].name, &file_stat) == 0 && file_stat.st_mode & S_IXUSR) {
                    // The file is executable
                    char args[NAME_MAX];
                    printf("Enter arguments (leave blank for none): ");
                    fgets(args, NAME_MAX, stdin);
                    args[strcspn(args, "\n")] = 0;  // Remove trailing newline

                    // Fork a new process to run the executable
                    pid_t pid = fork();
                    if (pid == 0) {
                        // Child process: run the executable
                        char exec_path[NAME_MAX * 2];  // Increased buffer size for the path
                        snprintf(exec_path, sizeof(exec_path), "./%s", items[index].name);  // Prepend ./ for relative execution

                        if (strlen(args) > 0) {
                            char *argv[] = {exec_path, args, NULL};
                            execvp(exec_path, argv);
                        } else {
                            char *argv[] = {exec_path, NULL};
                            execvp(exec_path, argv);
                        }

                        // If execvp fails
                        perror("execvp failed");
                        exit(1);
                    } else if (pid > 0) {
                        // Parent process: wait for the child process to finish
                        int status;
                        wait(&status);
                        printf("Program exited with status %d\n", WEXITSTATUS(status));
                    } else {
                        perror("Failed to fork");
                    }
                } else {
                    printf("Selected file is not executable\n");
                }
            } else {
                printf("Invalid file number or not a file\n");
            }
            break;
        case 'C':  // Change to a different directory
            printf("Enter directory number to change to: ");
            scanf("%d", &index);
            if (index >= 0 && index < total_count && items[index].is_directory) {
                if (chdir(items[index].name) == 0) {
                    printf("Changed to directory: %s\n", items[index].name);
                } else {
                    perror("Failed to change directory");
                }
            } else {
                printf("Invalid directory number\n");
            }
            break;
        case 'Q':  // Quit the shell program
            printf("Quitting...\n");
            exit(0);
        default:
            printf("Unknown command: %c\n", command);
            break;
    }

    // Clear screen after the command is executed (Optional)
    //system("clear");
}

// Sorting functions

// Compare function for sorting by file size, with files before directories
int compare_by_size(const void *a, const void *b) {
    const FileEntry *fa = (const FileEntry *)a;
    const FileEntry *fb = (const FileEntry *)b;

    // Ensure files are listed before directories
    if (fa->is_directory != fb->is_directory) {
        return fa->is_directory - fb->is_directory;
    }

    // Sort by size for files, directories remain in their relative order
    return (int)(fa->size - fb->size);
}

// Compare function for sorting by modification time, with files before directories
int compare_by_mtime(const void *a, const void *b) {
    const FileEntry *fa = (const FileEntry *)a;
    const FileEntry *fb = (const FileEntry *)b;

    // Ensure files are listed before directories
    if (fa->is_directory != fb->is_directory) {
        return fa->is_directory - fb->is_directory;
    }

    // Sort by modification time for files, directories remain in their relative order
    return (int)(fa->mtime - fb->mtime);
}

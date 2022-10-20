#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROJECT_NAME "nvim-config-manager"

#define INVALID_ARGS_ERR 22

#define BUFFER_SIZE 1000

#define CONFIG_PATH "/home/green726/.config/nvim/nvim_config_manager.txt"

int runNeovim(char **commands);
int addConfig(char **commands);
int removeConfig(char **commands);
int printHelp();
void checkArgCount(int desiredNum, int argc);
int listArgs();
int isNumber(char s[]);
FILE *deleteLine(FILE *src, const int line);
void removeChars(char *s, char c);

int main(int argc, char **argv) {

    checkArgCount(2, argc);

    if (!strcmp(argv[1], "add")) {
        checkArgCount(4, argc);
        return addConfig(argv);
    } else if (!strcmp(argv[1], "remove")) {
        checkArgCount(3, argc);
        return removeConfig(argv);
    } else if (!strcmp(argv[1], "help")) {
        return printHelp();
    } else if (!strcmp(argv[1], "open")) {
        checkArgCount(3, argc);
        return runNeovim(argv);
    } else if (!strcmp(argv[1], "list")) {
        return listArgs();
    }

    return 0;
}

int listArgs() {
    FILE *configsTxtPtr = fopen(CONFIG_PATH, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int numExpected = 1;

    while ((read = getline(&line, &len, configsTxtPtr)) != -1) {
        if (numExpected) {
            removeChars(line, *"\n");
            printf("Config Number: <%s> ", line);
        } else {
            printf("Links to Config File: <%s>", line);
        }
        numExpected = !numExpected;
    }

    if (len == 0) {
        printf("No Configured Configs Found\n");
    }

    return 0;
}

void checkArgCount(int desiredNum, int argc) {
    if (argc < desiredNum) {
        printf(
            "\nError: Invalid Number of Arguments: %d Desired but %d Given\n",
            desiredNum - 1, argc - 1);
        exit(INVALID_ARGS_ERR);
    }
}

int printHelp() {
    printf("Commands:\n");
    printf("Open [config number(int)]: Launches neovim with the "
           "configuration linked to the specified config number\n");
    printf("Add [config number(int)] [config path(string)]: Links the "
           "specified config path to the config number and allows the future "
           "usage of that configuration\n");
    printf("Remove [config number(int)]: Removes the config path linked to the "
           "specified config number\n");
    printf("List: Lists the added neovim configs\n");
    printf("Help: Prints out this help menu\n");
    return 0;
}

int removeConfig(char **commands) {
    if (!isNumber(commands[2])) {
        printf("\nError: Non Number Passed as Argument to Remove Command\n");
        exit(INVALID_ARGS_ERR);
    }

    FILE *configsTxtPtr = fopen(CONFIG_PATH, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int lineNumber = 0;
    int foundConfig = 0;

    while ((read = getline(&line, &len, configsTxtPtr)) != -1) {
        if (strcmp(line, commands[2])) {
            foundConfig = 1;
            break;
        }
        lineNumber++;
    }

    if (!foundConfig) {
        printf("\nError: Specified Config Number %s Was Not Found\n",
               commands[2]);
        exit(INVALID_ARGS_ERR);
    }

    configsTxtPtr = deleteLine(configsTxtPtr, lineNumber);
    configsTxtPtr = deleteLine(configsTxtPtr, lineNumber + 1);

    fclose(configsTxtPtr);
    if (line)
        free(line);

    return 0;
}

int addConfig(char **commands) {
    FILE *configsTxtPtr = fopen(CONFIG_PATH, "a");
    fputs(commands[2], configsTxtPtr);
    fputs("\n", configsTxtPtr);
    fputs(commands[3], configsTxtPtr);
    fputs("\n", configsTxtPtr);
    fclose(configsTxtPtr);
    return 0;
}

int runNeovim(char **commands) {
    FILE *configsTxtPtr = fopen(CONFIG_PATH, "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int lineFound = 0;

    while ((read = getline(&line, &len, configsTxtPtr)) != -1) {
        if (lineFound) {
            break;
        }
        if (strcmp(line, commands[2])) {
            lineFound = 1;
        }
    }

    int lengthOfPath = strlen(line);
    char *neovimCommand = malloc(sizeof(char[lengthOfPath + 10]));

    strcpy(neovimCommand, "nvim -u ");
    strcat(neovimCommand, line);

    system(neovimCommand);

    free(neovimCommand);
    return 0;
}

int isNumber(char s[]) {
    for (int i = 0; s[i] != '\0'; i++) {
        if (isdigit(s[i]) == 0)
            return 0;
    }
    return 1;
}

FILE *deleteLine(FILE *src, const int line) {
    FILE *temp = fopen("/home/green726/.config/nvim/delete.tmp", "w");
    char buffer[BUFFER_SIZE];
    int count = 1;
    while ((fgets(buffer, BUFFER_SIZE, src)) != NULL) {
        if (line != count)
            fputs(buffer, temp);
        count++;
    }
    fclose(src);
    remove(CONFIG_PATH);
    rename("/home/green726/.config/nvim/delete.tmp", CONFIG_PATH);
    return temp;
}

void removeChars(char *s, char c) {
    int writer = 0, reader = 0;

    while (s[reader]) {
        if (s[reader] != c) {
            s[writer++] = s[reader];
        }

        reader++;
    }

    s[writer] = 0;
}

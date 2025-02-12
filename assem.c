#define MAX_WORDS 128
#define COMMENT_CHAR ';'

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct asmLines {
    char* tag;
    char* operator;
    char* operand;
    char* comment;
} asmLines;

// Добавляем собственную реализацию strdup, если она не доступна
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

// Если strdup не определен в стандартной библиотеке
#ifndef strdup
char* strdup(const char* str) {
    size_t len = strlen(str) + 1;
    char* copy = (char*)malloc(len);
    if (copy) {
        memcpy(copy, str, len);
    }
    return copy;
}
#endif

char** split(char* input) {
    char** words = (char**)malloc(MAX_WORDS * sizeof(char*));
    if (!words) {
        fprintf(stderr, "%s", "Memory allocation error.");
        exit(EXIT_FAILURE);
    }

    char* temp_input = strdup(input);
    if (!temp_input) {
        fprintf(stderr, "%s", "Memory allocation error.");
        exit(EXIT_FAILURE);
    }

    char* token = strtok(temp_input, " \t\n\r");
    int count = 0;
    while (token != NULL && count < MAX_WORDS) {
        words[count] = (char*)malloc((strlen(token) + 1) * sizeof(char));
        if (!words[count]) {
            fprintf(stderr, "Memory allocation error.");
            exit(EXIT_FAILURE);
        }
        strcpy(words[count], token);
        token = strtok(NULL, " \t\n\r");
        count++;
    }

    free(temp_input);
    return words;
}

void getComment(char *str, asmLines *model) {
    for (size_t i = 0; i < strlen(str); i++) {
        if (COMMENT_CHAR == str[i]) {
            model->comment = malloc(strlen(str) - i);
            strncpy(model->comment, str + i + 1, strlen(str) - i - 1);
            model->comment[strlen(str) - i - 1] = '\0';
            return;
        }
    }
    model->comment = strdup("");
}

void getTag(char* str, asmLines* model) {
    char** words = split(str);
    for (size_t i = 0; i < strlen(str); i++) {
        if (str[i] == ':') {
            model->tag = malloc(i + 1);
            strcpy(model->tag, words[0]);
            model->tag[i] = '\0';
            return;
        }
    }
    model->tag = strdup("");
}

void getOperator(char *str, asmLines *model) {
    char **words = split(str);

    if (strcmp(model->tag, words[0]) != 0 && words[0][0] != COMMENT_CHAR) {
        model->operator = strdup(words[0]);
        free(words);
        return;
    }

    if (strcmp(model->tag, words[0]) == 0 && COMMENT_CHAR != words[1][0]) {
        model->operator = strdup(words[1]);
        free(words);
        return;
    }

    model->operator = strdup("");
    free(words);
}

void getOperand(char* str, asmLines* model) {
    char** words = split(str);
    model->operand = strdup("");
    int comment_index = -1;
    for (int i = 0; words[i] != NULL; i++) {
        if (strcmp(words[i], "*") == 0) {
            comment_index = i;
            break;
        }
    }
    
    int operator_index = -1;
    for (int i = 0; words[i] != NULL; i++) {
        if (strcmp(words[i], model->operator) == 0) {
            operator_index = i;
            break;
        }
    }
    
    if (operator_index != -1) {
        int operand_start_index = operator_index + 1;
        int operand_end_index = (comment_index != -1) ? comment_index : MAX_WORDS;
        size_t operand_length = 0;
        
        for (int i = operand_start_index; i < operand_end_index && words[i] != NULL; i++) {
            operand_length += strlen(words[i]) + 1;
        }
        
        model->operand = (char*)malloc((operand_length + 1) * sizeof(char));
        strcpy(model->operand, "");
        
        for (int i = operand_start_index; i < operand_end_index && words[i] != NULL; i++) {
            strcat(model->operand, words[i]);
            strcat(model->operand, " ");
        }
        model->operand[operand_length] = '\0';
    }
    
    free(words);
}

void print(asmLines model) {
    printf("Метка: %20s, Оператор: %20s, Операнд: %20s, Комментарий: %s\n",
           model.tag, model.operator, model.operand, model.comment);
}

void process(char* str, asmLines* model) {
    getTag(str, model);
    getComment(str, model);
    getOperator(str, model);
    getOperand(str, model);
    print(*model);

    free(model->tag);
    free(model->comment);
    free(model->operator);
    free(model->operand);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <assembly_file>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    asmLines model;
    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }
    
    char str[512];
    while (fgets(str, sizeof(str), file)) {
        process(str, &model);
    }
    
    fclose(file);
    return EXIT_SUCCESS;
}


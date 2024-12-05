#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define WHITESPACE " \t\n"
#define LABEL_LENGTH 31
#define MAX_LINE 81
#define MAX_MACRO_CONTENT_LINES 20



enum state{
    mcr_definition,
    endmcr_definition,
    mcr_call,
    other_line
};

typedef struct macro {
    char name[LABEL_LENGTH];
    char mcr_content[MAX_MACRO_CONTENT_LINES][MAX_LINE];
    int num_of_line;
} MACRO;

MACRO *findMacro(MACRO *mcr_table, int mcr_table_size, const char *mcr_name);
void advance_after_space(char **ptr);
enum state which_line(char *line, MACRO **mcr_ptr, MACRO *mcr_table, int *mcr_table_size);
char *pre_ass(char *name_code);





#pragma once
#include "word.h"
#include "first_pass.h"

struct ext_table{
    char ext_name[LABEL_LENGTH];
    int value; 
    int ext_count;
};

struct bin_code_table{
    short int *code_table;
    short int *data_table;
};

int secondPass(struct LABELS *lab_ptr,struct bin_code_table *bin_code_ptr, struct ext_table **ext_ptr,char *amFileName, FILE *amFile);

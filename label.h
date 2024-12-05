#pragma once
#include "pre_assem.h"


struct LABELS {
    char name[LABEL_LENGTH];
    int value; /*The address/value of the symbol.*/
    int type; /*1-code, 2-define, 3-data, 4-external, 5-entry, 6-entryCode, 7-entryData*/
    int IC;
    int DC;
};

void add_label(struct LABELS **lab_ptr, char *lab_name, int lab_value, int *lab_num, int lab_type);



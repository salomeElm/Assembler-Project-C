#pragma once
#include "label.h"
#include "help.h"

struct ent_table{
    char ent_name[LABEL_LENGTH];
    int value;
    int ent_count;
};

int firstPass(struct LABELS **lab_ptr, struct ent_table **ent_ptr, char *amFileName, FILE *amFile);
struct LABELS *search_label(struct LABELS *lab_ptr,int label_count,char *name);

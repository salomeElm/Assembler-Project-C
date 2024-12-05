#pragma once
#include "second_pass.h"


void print_ob_file(char *file_name, struct bin_code_table *bin_code_ptr,struct LABELS *label_table);

void print_ent_file(char *file_name,struct ent_table *ent_ptr);

void print_ext_file(char *file_name,struct ext_table *ext_ptr);

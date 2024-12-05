#pragma once
#include <string.h>
#define SIZE_ENCODED_4 7

struct myWord{
    unsigned short int word: 14;
};

void ARE(struct myWord *curr, short int num);

void DESTENY(struct myWord *curr, short int num);

void SOURCE(struct myWord *curr, short int num);

void OPCODE(struct myWord *curr, short int num);

void reset_word(struct myWord *curr);

unsigned short int twoComplement(unsigned short int num);

void writeBinary_12(struct myWord *curr, unsigned short int num);

void writeBinary2_4(struct myWord *curr, short int num);

void writeBinary5_7(struct myWord *curr, short int num);

void extern_01(struct myWord *curr);

void entry_10(struct myWord *curr);

char *translation_4(struct myWord *curr, char *trans_res);

void short_int_to_myWord(struct myWord *curr, short int code_line);

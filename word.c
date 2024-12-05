#include "word.h"
#include <stdio.h>

/*Filling two bits in ARE according to the received number as a parameter. */
void ARE(struct myWord *curr, short int num)
{
    if(num>=0 && num<=2)
        curr->word = (curr->word)|num;
}

/*Filling two bits in the destination operand according to the received number as a parameter.*/
void DESTENY(struct myWord *curr, short int num) {
    unsigned short int value;
    if(num>=0 && num<=3)
    {
        value = num<<2;
        curr->word = (curr->word)|value;
    }
}

/*Filling two bits in the destination operand according to the received number as a parameter.*/
void SOURCE(struct myWord *curr, short int num){
    unsigned short int value;
    if(num>=0 && num<=3)
    {
        value = num<<4;
        curr->word = (curr->word)|value;
    }
}

/*Filling four bits in the opcode field according to the received number as a parameter.*/
void OPCODE(struct myWord *curr, short int num){
   unsigned short int value;
    if(num>=0 && num<=15)
    {
        value = num<<6;
        curr->word = (curr->word)|value;
    }
}

/*Each time before updating a new word, resetting it (before inserting the data).*/
void reset_word(struct myWord *curr) {
    curr->word = 0;
}

/*Two's complement method for 2.*/
unsigned short int twoComplement(unsigned short int num) {
    return (~num) + 1;
}

/*For the additional word of immediate addressing/index of an array/pointer - translating a number to binary with a width of 12 bits in addition to the two rightmost bits.*/
void writeBinary_12(struct myWord *curr, unsigned short int num) {
	unsigned short int binaryNum;
    if (num<0) {
        num = twoComplement(-num); /* convert to unsigned before taking two's complement */
    }
    binaryNum = num << 2; /* shift the least significant 14 bits to the left by 2 positions */
    curr->word = binaryNum | curr->word;
}

/*For the additional word of a register - translating a register number to binary and entering it into bits 2-4.*/
void writeBinary2_4(struct myWord *curr, short int num) {
	short int binaryNum;
    binaryNum = num << 2;
    curr->word = (curr->word)|binaryNum;
}

/*For the additional word of a register - translating a register number to binary and entering it into bits 5-7.*/
void writeBinary5_7(struct myWord *curr, short int num) {
	short int binaryNum;
    binaryNum = num << 5;
    curr->word = (curr->word)|binaryNum;
}

/*Updating the last 2 bits to 01 when the address is external - the address value is unknown, so it is reset.*/
void extern_01(struct myWord *curr)
{
    curr->word = 1;
}

/*Updating the last 2 bits to 10 when the address is internal, in addition to the previous value already set there.*/
void entry_10(struct myWord *curr)
{
    curr->word = (curr->word)|2;
}

/*Accepts a word in binary and an empty string, translates the word to 4 encoded characters, and returns the updated string.*/
char *translation_4(struct myWord *curr, char *trans_res)
{
    char encoded_4[]={'*','#','%','!'};
    char result[SIZE_ENCODED_4 + 1] = {'\0'};

    int i;
    unsigned short int digit;
    for(i=0; i<(SIZE_ENCODED_4*2); i= i+2)
    {
       /*Each time, comparing the two desired bits in the original word with the number 3, where its last two bits are turned on.*/
        digit = ((curr->word) >> i) & 3;
        result[SIZE_ENCODED_4 - (i/2) -1] = encoded_4[digit]; /*Entering the array from the end to the beginning (right to left) with the encoded number in base 4.*/
    }
    strcpy(trans_res,result);
    return trans_res;
}

/*Function that accepts a short int and inserts its rightmost 14 bits into struct myWord.*/
void short_int_to_myWord(struct myWord *curr, short int code_line)
{
    reset_word(curr);
    curr->word = code_line & 0x3FFF; /*0x3FFF is a constant where its rightmost 14 bits are turned on.*/
}

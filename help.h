#pragma once
#include "pre_assem.h"

#define MAX_NUMBER 2047
#define MIN_NUMBER (-2048)
#define MAX_NUMBER_LENGTH 5

/*Structure that contains the syntax of each type of line in assembly language*/
struct syntax{
    char *error[200];
    char labelName[LABEL_LENGTH];

    enum{
        inst_sentence, /*Instruction sentence*/
        dir_sentence, /*Directive sentence*/
        define_sentence, /*Constant definition sentence*/
        empty_sentence /*Empty line or comment*/
    }syntax_type;

    union{
        struct{
            char label[LABEL_LENGTH];
            int number;
        }define;

        struct{
            /*Type of directive sentence*/
            enum{
                extern_sentence,
                entry_sentence,
                string_sentence,
                data_sentence
            }dir_type;
            union{
                char label[LABEL_LENGTH]; /*In the case of extern/entry, there is a label name*/
                char *string; /*In the case of string*/
                struct data_arr{ /*In the case of data, it can be either a number or a label*/
                    enum{
                        data_label,
                        data_number
                    }data_type;
                    union{
                        char label[LABEL_LENGTH];
                        int number;
                    }data_options;
                }*data; 
            }dir_option;
        }dir;

        struct{
            /*Type of operand*/
            enum{
                mov,
                cmp,
                add,
                sub,
                not,
                clr,
                lea,
                inc,
                dec,
                jmp,
                bne,
                red,
                prn,
                jsr,
                rts,
                hlt
            }inst_type;
            struct operands_struct{ /*An array of two structures because each operation has a maximum of two operands*/
                enum{ /*Type of operand*/
                    immediate, /*Immediate addressing - an integer*/
                    direct_label, /*Direct addressing - a label*/
                    fixed_index, /*Fixed index addressing - a label which is the array address and an array index*/
                    reg, /*Register addressing - register name*/
                    no_operand /*Does not receive an operand*/
                }operand_type;
                union{
                    union{ /*Immediate addressing receives a number or a constant*/
                        int number; /*Number*/
                        char label[LABEL_LENGTH]; /*Constant*/
                    }immed;
                    char dir_label[LABEL_LENGTH]; /*Direct addressing receives a label*/
                    struct{
                        char label[LABEL_LENGTH]; /*Fixed index addressing receives a label which is the starting address of the array*/
                        union{ /*Fixed index addressing also receives a number or a label which is the array index*/
                            int number; /*Number*/
                            char label[LABEL_LENGTH]; /*Label*/
                        }index_option;
                    }fixed_index_options;
                    int reg; /*Register addressing receives a register name*/
                }operand_option;
            }operands[2];
        }inst;
    }syntax_option;
};

int is_fixed_index(char *opernd,char *label,char *num_label);
char *is_immed_label(char *immed,char *label_str);
int is_immed_num(char *immed);
int is_reg(char *reg);
int which_operand(char *line,int index,int opcode,struct operands_struct *operands);
int is_opcode(char *opcode);
int entry_extern_check(char *line,int index,char *label);
int define_check(char *line,int index,char *label,int *number);
int string_check(char *line,int index,char *string);
struct syntax get_syntax_from_line(char *line);
char *without_spaces(char *line);
int numbers_arr(char *line,int index,struct data_arr **dataArr);
int comma_check(char *line, int index);
int skip_space(char *word, int index);
int is_number(char *str, int max, int min, int *result);
int is_label(char *str);

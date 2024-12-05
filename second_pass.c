#include "second_pass.h"

/*The function receives pointers to the symbols table, encoding table, external table, the file name, and the file itself. In the second pass, we'll iterate through the file again, encoding it in the encoding table using the symbols table. Additionally, we'll build the external table.*/
int secondPass(struct LABELS *lab_ptr,struct bin_code_table *bin_code_ptr, struct ext_table **ext_ptr,char *amFileName, FILE *amFile)
{
    char line[MAX_LINE] = {0};
    int errorFlag=0;
    struct syntax line_syntax = {0};
    struct LABELS *labelFind;
    struct myWord curr_word = {0};
    struct ext_table *temp_ext_ptr;
    int i;
    int count_code = 0;
    int count_data = 0;
    int ic=0;
	char *p;
    int num_of_labels = (lab_ptr[0].DC)+(lab_ptr[0].IC)-100;
    (*ext_ptr)[0].ext_count = 0;


    bin_code_ptr->code_table = (short int *)malloc(((lab_ptr[0].IC)-100)*sizeof(short int));
    if(bin_code_ptr->code_table == NULL)
    {
        printf("No space\n");
        errorFlag = 1;
        return errorFlag;
    }
    bin_code_ptr->data_table = (short int *)malloc((lab_ptr[0].DC)*sizeof(short int));
    if(bin_code_ptr->data_table == NULL)
    {
        printf("No space\n");
        errorFlag = 1;
        return errorFlag;
    }

    while(fgets(line,MAX_LINE,amFile))
    {
        p = strchr(line,'\n');
        if(p!=NULL)
            *p = '\0';
        line_syntax = get_syntax_from_line(line);
        /*If it's a string directive line, encode the ASCII code of each character in the string in the encoded data table.*/
        if(line_syntax.syntax_type == dir_sentence && line_syntax.syntax_option.dir.dir_type == string_sentence )
        {
            for(i=0; i<strlen(line_syntax.syntax_option.dir.dir_option.string)+1;i++)
            {
                char ch = line_syntax.syntax_option.dir.dir_option.string[i];
                bin_code_ptr->data_table[count_data*sizeof(short int)] = (short int)ch;
                count_data++;
            }
        }
        /*If it's a data directive line, encode the value of each of its elements in the encoded data table.*/
        if(line_syntax.syntax_type == dir_sentence && line_syntax.syntax_option.dir.dir_type == data_sentence )
        {
            i=0;
            while(line_syntax.syntax_option.dir.dir_option.data[i].data_options.label[0] != '\0')
            {
                if(line_syntax.syntax_option.dir.dir_option.data[i].data_type == data_number)
                {
                    bin_code_ptr->data_table[count_data*sizeof(short int)] = (short int)line_syntax.syntax_option.dir.dir_option.data[i].data_options.number;
                    count_data++;
                }
                else /*data_label*/
                {
                    labelFind=search_label(lab_ptr,num_of_labels,line_syntax.syntax_option.dir.dir_option.data[i].data_options.label);
                    if(labelFind == NULL)
                    {
                        printf("file:%s error:undefined label: %s\n",amFileName,line_syntax.syntax_option.dir.dir_option.data[i].data_options.label);
                        errorFlag = 1;
                        break;
                    }
                    bin_code_ptr->data_table[count_data*sizeof(short int)] = (short int)labelFind->value;
                    count_data++;
                }
                i++;
            }
        }
        /*If it's an instruction line, encode the corresponding lines in the encoded instructions table.*/
        if(line_syntax.syntax_type == inst_sentence)
        {
            /*Encode the first word.*/
            short int source_operand = line_syntax.syntax_option.inst.operands[0].operand_type;
            short int dest_operand = line_syntax.syntax_option.inst.operands[1].operand_type;
            short int opcode = line_syntax.syntax_option.inst.inst_type;
            reset_word(&curr_word);
            ARE(&curr_word,0);
            SOURCE(&curr_word,source_operand);
            DESTENY(&curr_word,dest_operand);
            OPCODE(&curr_word,opcode);
            bin_code_ptr->code_table[count_code*sizeof(short int)] = curr_word.word;
            count_code++;


            /*If there is a register in both the source and destination operands.*/
            if(line_syntax.syntax_option.inst.operands[0].operand_type == reg && line_syntax.syntax_option.inst.operands[1].operand_type == reg)
            {
                short int source_reg = line_syntax.syntax_option.inst.operands[0].operand_option.reg;
                short int dest_reg = line_syntax.syntax_option.inst.operands[1].operand_option.reg;
                reset_word(&curr_word);
                writeBinary5_7(&curr_word,source_reg);
                writeBinary2_4(&curr_word,dest_reg);
                bin_code_ptr->code_table[count_code*sizeof(short int)] = curr_word.word;
                count_code++;
            }
            else /*The line is an instruction line.*/
            {
                /*The loop iterates over the destination and source operands of the line if they exist.*/
                for(i=0;i<2;i++)
                {
                    if(line_syntax.syntax_option.inst.operands[i].operand_type==no_operand) /*If there is no operand (source/destination).*/
                        continue;
                    reset_word(&curr_word);
                    /*Register addressing.*/
                    if(line_syntax.syntax_option.inst.operands[i].operand_type == reg)
                    {
                        short int num_of_reg = line_syntax.syntax_option.inst.operands[i].operand_option.reg;
                        if(i==0) /*Source operand.*/
                           writeBinary5_7(&curr_word,num_of_reg);
                        else /*Destination operand.*/
                            writeBinary2_4(&curr_word,num_of_reg);
                        bin_code_ptr->code_table[count_code*sizeof(short int)] = curr_word.word;
                        count_code++;
                        continue;
                    }
                    /*Immediate addressing / Indexed addressing with a constant offset (only the label).*/
                    if(line_syntax.syntax_option.inst.operands[i].operand_type == direct_label
                                ||line_syntax.syntax_option.inst.operands[i].operand_type == fixed_index)
                    {
                        char *label_name;
                        if(line_syntax.syntax_option.inst.operands[i].operand_type == direct_label)
                            label_name = line_syntax.syntax_option.inst.operands[i].operand_option.dir_label;
                        else
                            label_name = line_syntax.syntax_option.inst.operands[i].operand_option.fixed_index_options.label;
                        labelFind=search_label(lab_ptr,num_of_labels,label_name);
                        if(labelFind == NULL) /*There is a reference to a symbol that does not exist/defined.*/
                        {
                            if(line_syntax.syntax_option.inst.operands[i].operand_type == direct_label)
                                printf("file:%s error:undefined label: %s\n",amFileName,line_syntax.syntax_option.inst.operands[i].operand_option.dir_label);
                            else
                                printf("file:%s error:undefined label: %s\n",amFileName,line_syntax.syntax_option.inst.operands[i].operand_option.fixed_index_options.label);
                            errorFlag = 1;
                            continue;
                        }
                        if(labelFind->type == 4) /*External address.*/
                        {
                            extern_01(&curr_word);
                            /*We fill the external table.*/
                            strcpy((*ext_ptr)[(*ext_ptr)[0].ext_count].ext_name,labelFind->name);
                            (*ext_ptr)[(*ext_ptr)[0].ext_count].value = ic+100;
                            (*ext_ptr)[0].ext_count++;
                            temp_ext_ptr = realloc((*ext_ptr),((*ext_ptr)[0].ext_count+1)*sizeof(struct ext_table));
                            if(temp_ext_ptr == NULL)
                            {
                                printf("No space \n");
                                errorFlag = 1;
                                free((*ext_ptr));
                                return errorFlag;
                            }
                            *ext_ptr = temp_ext_ptr;
                        }
                        else /*Internal address.*/
                        {
                            entry_10(&curr_word);
                            writeBinary_12(&curr_word,labelFind->value);
                        }
                        bin_code_ptr->code_table[count_code*sizeof(short int)] = (short int)curr_word.word;
                        count_code++;
                    }
                    /*Immediate addressing / Indexed addressing with a constant offset (only the number)(#).*/
                    if(line_syntax.syntax_option.inst.operands[i].operand_type == immediate
                                ||line_syntax.syntax_option.inst.operands[i].operand_type == fixed_index)
                    {
                        char def_label[LABEL_LENGTH]= {0};
                        reset_word(&curr_word);
                        ARE(&curr_word,0);
                        if(line_syntax.syntax_option.inst.operands[i].operand_type == immediate)
                            strcpy(def_label,line_syntax.syntax_option.inst.operands[i].operand_option.immed.label);
                        else
                            strcpy(def_label,line_syntax.syntax_option.inst.operands[i].operand_option.fixed_index_options.index_option.label);
                        if(isalpha(def_label[0])!=0 || isdigit(def_label[0])!=0) /*The symbol exists - meaning the operand is a constant.*/
                        {
                            labelFind=search_label(lab_ptr,num_of_labels,def_label);
                            if(labelFind == NULL) /*There is a reference to a symbol that does not exist/defined.*/
                            {
                                if(line_syntax.syntax_option.inst.operands[i].operand_type == immediate)
                                    printf("file:%s error:undefined label: %s\n",amFileName,line_syntax.syntax_option.inst.operands[i].operand_option.immed.label);
                                else
                                    printf("file:%s error:undefined label: %s\n",amFileName,line_syntax.syntax_option.inst.operands[i].operand_option.fixed_index_options.index_option.label);
                                errorFlag = 1;
                                continue;
                            }
                            writeBinary_12(&curr_word,labelFind->value);
                        }
                        else/*The operand is a number.*/
                        {
                            short int num;
                            if(line_syntax.syntax_option.inst.operands[i].operand_type == immediate)
                                num = line_syntax.syntax_option.inst.operands[i].operand_option.immed.number;
                            else
                                num = line_syntax.syntax_option.inst.operands[i].operand_option.fixed_index_options.index_option.number;
                            writeBinary_12(&curr_word,num);
                        }
                        bin_code_ptr->code_table[count_code*sizeof(short int)] = curr_word.word;
                        count_code++;
                    }
                }
            }
            ic++;
        }
    }
    
    lab_ptr[0].IC = count_code;
    lab_ptr[0].DC = count_data;

    return errorFlag;
}

#include "first_pass.h"

/*The function receives a pointer to the symbols table, the file name, and the file itself. In the first pass, we'll traverse the file to build the symbols table.*/
int firstPass(struct LABELS **lab_ptr, struct ent_table **ent_ptr, char *amFileName, FILE *amFile){
    char line[MAX_LINE] = {0};
    int ic=100, dc=0;
    int errorFlag=0;
    struct syntax line_syntax;
    int line_count = 1;
    int label_count = 0;
    struct LABELS *labelFind;
    struct ent_table *temp_ent_ptr;
    int i,j=0;
    char *p;
    (*ent_ptr)[0].ent_count = 0;


    while(fgets(line,MAX_LINE,amFile))
    {
        p = strchr(line,'\n');
        if(p!=NULL)
            *p = '\0';
        line_syntax = get_syntax_from_line(line);
        /*Checking for errors in each line, printing an error message if found, and continuing to the next line.*/
        if(line_syntax.error[0] != NULL){
            while(line_syntax.error[j] != NULL)
            {
                printf("line:%d file:%s error:%s \n",line_count,amFileName,line_syntax.error[j]);
                j++;
            }
            line_count++;
            errorFlag=1;
            continue;
        }

        /*Checking if the statement is a constant definition statement. */
        if(line_syntax.syntax_type == define_sentence)
        {
            int value = line_syntax.syntax_option.define.number;
            add_label(lab_ptr,line_syntax.syntax_option.define.label,value,&label_count,2);
			continue;
        }

        /*Checking if there is a label in the line, and if so, checking if the statement is an instruction or a directive statement of data type data or string. */
        if(line_syntax.labelName[0] != '\0' &&
             (line_syntax.syntax_type==inst_sentence ||
                (line_syntax.syntax_type==dir_sentence&&(line_syntax.syntax_option.dir.dir_type==data_sentence||line_syntax.syntax_option.dir.dir_type==string_sentence))))
            {

                labelFind = search_label(*lab_ptr,label_count,line_syntax.labelName); /*Checking if the symbol is found in the symbols table.*/
                if(labelFind == NULL) /*The symbol does not appear in the symbols table, so we will add it.*/
                {
                    /*We'll check if it's an instruction or a directive statement and add the symbol and its properties accordingly.
If it's an instruction statement, the value of the symbol will be its address according to the instruction counter (IC).
If it's a directive statement, its value will be according to the data counter (DC).
Reminder:
1 - code, 2 - define, 3 - data, 4 - external, 5 - entry, 6 - entryCode, 7 - entryData.*/
                    if(line_syntax.syntax_type == inst_sentence)
                        add_label(lab_ptr,line_syntax.labelName,ic,&label_count,1);
                    if(line_syntax.syntax_type == dir_sentence)
                        add_label(lab_ptr,line_syntax.labelName,dc,&label_count,3);
                }
                else /*The symbol is already in the symbols table.*/
                {
                    /*In case there is a prior declaration of a symbol of type 'entry' and only then its definition.*/
                    if(labelFind->type == 5) /*If the symbol type is 'entry'.*/
                    {
                        /*If it's an instruction statement, we'll initialize the symbol type to 'entryCode'.*/
                        if(line_syntax.syntax_type == inst_sentence)
                            labelFind->type = 6;
                        /*If it's a directive statement, we'll initialize the symbol type to 'entryData'.*/
                        if(line_syntax.syntax_type == dir_sentence)
                            labelFind->type = 7;
                    }
                    else{ /*Error - there is a symbol that has been defined twice.*/
                        printf("line:%d file:%s error:redefinition of label: %s",line_count,amFileName,line_syntax.labelName);
                        errorFlag=1;
                    }
                }
            }


            /*If it's an instruction line, increment the instruction counter (IC) according to the conditions.*/
            if(line_syntax.syntax_type==inst_sentence)
            {
                int source_type = line_syntax.syntax_option.inst.operands[0].operand_type; /*The type of the source operand.*/
                int dest_type = line_syntax.syntax_option.inst.operands[1].operand_type; /*The type of the destination operand.*/
                ic++; /*The first word.*/
                /*If the operand is of one of the three addressing methods, there is one additional word.
If the operand is of the fixed index addressing method, add 2 additional words.*/
                if(source_type==immediate || source_type==direct_label || source_type==reg)
                    ic++;
                else if(source_type==fixed_index)
                    ic += 2;
                if(dest_type==immediate || dest_type==direct_label || dest_type==reg)
                    ic++;
                else if(dest_type==fixed_index)
                    ic += 2;
            }
            /*Otherwise, if it's a data directive line, increment the data counter (DC) according to the conditions.*/
            else if(line_syntax.syntax_type==dir_sentence && line_syntax.syntax_option.dir.dir_type == data_sentence)
            {
                int count = 0;
                i =0;
                while(line_syntax.syntax_option.dir.dir_option.data[i].data_type == data_label || line_syntax.syntax_option.dir.dir_option.data[i].data_type == data_number){
                    count++;
                    i++;
                }
                dc += count;
            }


            /*Otherwise, if it's a string directive line, increment the data counter (DC) according to the conditions.*/
            else if(line_syntax.syntax_type==dir_sentence && line_syntax.syntax_option.dir.dir_type == string_sentence)
            {
                int count = strlen(line_syntax.syntax_option.dir.dir_option.string)+1; /*Number of characters including null.*/
                dc += count;
            }

            /*Otherwise, if it's a line of type 'entry' or 'extern'.*/
            else if(line_syntax.syntax_type==dir_sentence && line_syntax.syntax_option.dir.dir_type <= entry_sentence)
            {
                labelFind = search_label(*lab_ptr,label_count,line_syntax.syntax_option.dir.dir_option.label); /* Checking if the symbol is found in the symbols table.*/
                /*If the symbol is already in the symbols table and is now being defined as 'entry'.*/
                if((labelFind != NULL) && (line_syntax.syntax_option.dir.dir_type == entry_sentence))
                {
                    if(labelFind->type == 1) /*If the symbol is of type 'code'.*/
                        labelFind->type = 6; /*We'll change the symbol to be of type 'entryCode'.*/
                    else if(labelFind->type == 3) /*If the symbol is of type 'data'.*/
                        labelFind->type = 7; /*We'll change the symbol to be of type 'entryData'.*/
                    else{ /*Error - there is a symbol that has been defined twice.*/
                    printf("line:%d file:%s error:redefinition of label: %s",line_count,amFileName,line_syntax.syntax_option.dir.dir_option.label);
                    errorFlag=1;
                    }
                }
                /*The symbol is not found in the symbols table.*/
                else if(labelFind == NULL)
                {
                    if(line_syntax.syntax_option.dir.dir_type == entry_sentence)
                        add_label(lab_ptr,line_syntax.syntax_option.dir.dir_option.label,ic,&label_count,5);
                    if(line_syntax.syntax_option.dir.dir_type == extern_sentence)
                        add_label(lab_ptr,line_syntax.syntax_option.dir.dir_option.label,ic,&label_count,4);
                }
                else /*Error - there is a symbol that has been defined twice.*/
                {
                    printf("line:%d file:%s error:redefinition of label: %s",line_count,amFileName,line_syntax.syntax_option.dir.dir_option.label);
                    errorFlag=1;
                }
            }
    }


    /*After filling the symbols table, we'll iterate through it with a loop.*/
    for(i=0; i<label_count; i++)
    {
        /*The data is required to appear after all instructions, so we'll update their value accordingly.*/
        if((*lab_ptr)[i].type == 3 || (*lab_ptr)[i].type == 7)
        {
            (*lab_ptr)[i].value += ic;
        }
        /*Checking if there is a symbol of type 'entry' remaining without definition.*/
        if((*lab_ptr)[i].type == 5)
        {
            printf("file:%s error:The label %s of type entry was declared but it was not defined",amFileName,(*lab_ptr)[i].name);
                errorFlag=1;
        }
        /*We'll fill the internal variables table.*/
        if((*lab_ptr)[i].type == 6 || (*lab_ptr)[i].type == 7)
        {
            strcpy((*ent_ptr)[(*ent_ptr)[0].ent_count].ent_name,(*lab_ptr)[i].name);
            (*ent_ptr)[(*ent_ptr)[0].ent_count].value = (*lab_ptr)[i].value;
            (*ent_ptr)[0].ent_count++;
            temp_ent_ptr = (struct ent_table *)realloc(*ent_ptr,((*ent_ptr)[0].ent_count+1)*sizeof(struct ent_table));
            if(temp_ent_ptr == NULL)
            {
                printf("No space \n");
                errorFlag = 1;
                free((*ent_ptr));
                return errorFlag;
            }
            (*ent_ptr) = temp_ent_ptr;
        }
    }

    (*lab_ptr)[0].IC = ic;
    (*lab_ptr)[0].DC = dc;

    return errorFlag;
}

/*The function receives a pointer to the symbols table, the number of symbols, and a symbol name. It checks if the symbol is already in the symbols table. If it is, it returns a pointer to it; otherwise, it returns null.*/
struct LABELS *search_label(struct LABELS *lab_ptr,int label_count,char *name)
{
    int i;
    for(i=0;i<label_count;i++){
        if(strcmp(lab_ptr[i].name,name) == 0)
            return &lab_ptr[i];
    }
    return NULL;
}

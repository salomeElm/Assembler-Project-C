#include "help.h"

#define NUM_OF_OPCODE 16

struct legal_methods{
    char *name;
    int opcode;
    char *source;
    char *dest;
}legal_methods_table[]={
    {"mov",0,"0123","123"},
    {"cmp",1,"0123","0123"},
    {"add",2,"0123","123"},
    {"sub",3,"0123","123"},
    {"not",4,"","123"},
    {"clr",5,"","123"},
    {"lea",6,"12","123"},
    {"inc",7,"","123"},
    {"dec",8,"","123"},
    {"jmp",9,"","13"},
    {"bne",10,"","13"},
    {"red",11,"","123"},
    {"prn",12,"","0123"},
    {"jsr",13,"","13"},
    {"rts",14,"",""},
    {"hlt",15,"",""}
};


/*The function accepts a line of assembly code and returns the appropriate syntactic structure for that line.
 * It updates all relevant fields in the structure according to the type of line it receives.*/
struct syntax get_syntax_from_line(char *line) {
    struct syntax result = {0}; /*The structure to be returned*/
    char *ptr = line;
    int i = 0;
    int j = 0;
    char *word;
    int error_index = 0;

    if(line[0] == ';') /*Comment line*/
    {
        result.syntax_type = empty_sentence;
        return result;
    }

    while(isspace(*ptr)) /*Advance pointer to skip initial spaces*/
        ptr++;

    if(*ptr == '\n') /*If we reach the end of the string, it's an empty line*/
    {
        result.syntax_type = empty_sentence;
        return result;
    }


    word = (char *)malloc(sizeof(char)); /*First word in the line*/
    if(word == NULL) /*Check if allocation was successful*/
    {
        result.error[error_index] = "No space\n";
        return result;
    }
    i = skip_space(line, i); /*The first index in the string that is not a space*/
    while(line[i] != '\0' && isspace(line[i]) == 0)  /*A loop that gets the first word if it is a label*/
    {
        if(line[i] == ':') { /*If it's a label*/
            break;
        }
        word = (char *)realloc(word, sizeof(char)*(j+1));
        if(word == NULL) /*Check if memory allocation succeeded.*/
        {
            result.error[error_index] = "No space\n";
            return result;
        }
        word[j] = line[i]; /*Inserting the character from the line into its place in the word*/
        i++;
        j++;
    } /* end of while*/
    word = (char *)realloc(word, j+1);
    if(word == NULL) /*Check if memory allocation succeeded.*/
    {
        result.error[error_index] = "No space\n";
        return result;
    }
    word[j] = '\0'; /*End of the first word*/


    if(strncmp(word,".define", 7) == 0) /*We will check if the first word is of a constant definition*/
    {
        char label[LABEL_LENGTH]={0};
		int number;
		int define_result;
        memset(label, 0, LABEL_LENGTH);
        if(strlen(word) > 7) /*No space immediately after ".define"*/
        {
            result.error[error_index] = "There is no space immediately after '.define'\n";
            error_index++;
        }
		result.syntax_type = define_sentence;
        define_result=define_check(line,i,label,&number);
        if(define_result != 0)
        {
            if(define_result == 1)
                result.error[error_index] = "Missing the equals symbol in define sentence\n";
            else if(define_result == 2)
                result.error[error_index] = "Invalid label in define sentence\n";
            else if(define_result == 3)
                result.error[error_index] = "The number in define sentence is out of range\n";
            else /*if(define_result == 4)*/
                result.error[error_index] = "The number in define sentence is not a real number\n";
        }
        else /*The definition of the constant theorem is valid*/
        {
            strcpy(result.syntax_option.define.label,label);
            result.syntax_option.define.number = number;
        }
        return result;
    }

    if(strncmp(word,".entry", 6) == 0 || strncmp(word,".extern", 7) == 0) /*Check if the first word is for defining an entry or an extern*/
    {
        char label[LABEL_LENGTH]= {0};
	int check_res;
		if(strlen(word) > 6 && strncmp(word,".entry", 6) == 0) /*אין There is no space immediately after ".entry"*/
        {
            result.error[error_index] = "There is no space immediately after '.entry'\n";
            error_index++;
        }
        else if(strlen(word) > 7 && strncmp(word,".extern", 7) == 0) /*איןThere is no space immediately after ".extern"*/
        {
            result.error[error_index] = "There is no space immediately after '.extern'\n";
            error_index++;
        }
        result.syntax_type = dir_sentence;
        if(strncmp(word,".entry", 6) == 0) /*Entry definition sentence*/
            result.syntax_option.dir.dir_type = entry_sentence;
        else /*Extern definition sentence*/
            result.syntax_option.dir.dir_type = extern_sentence;
        
        memset(label, 0, LABEL_LENGTH);
        check_res = entry_extern_check(line,i,label);
        if(check_res != 0)
        {
            if(result.syntax_option.dir.dir_type == entry_sentence)
            {
                if(check_res == 1)
                    result.error[error_index] = "Invalid label in entry sentence\n";
                else /*check_res == 2*/
                    result.error[error_index] = "There are extra characters that are not a space after the symbol in the entry sentence\n";
            }
            else /*We are in an "extern" sentence.*/
            {
                if(check_res == 1)
                    result.error[error_index] = "Invalid label in extern sentence\n";
                else /*check_res == 2*/
                    result.error[error_index] = "There are extra characters that are not a space after the symbol in the extern sentence\n";
            }
        }
        else /*The sentence is correct.*/
            strcpy(result.syntax_option.dir.dir_option.label,label);
        return result;
    }


    if(line[i] == ':') /*Let's check if the first word is a label.*/
    {
		int valid_label = is_label(word);
        if(valid_label == 1) /*Invalid label name.*/
        {
            /*result.error[error_index]= word;
            strcat(result.error[error_index], " Is invalid label\n");*/
			result.error[error_index] = "Invalid labe\n";
            error_index++;
            /*free(word);*/
        }
        else /*Valid label.*/
        {
            /*result.labelName[0] = *word;*/
            strcpy(result.labelName,word);

            /*We'll find the second word.*/
            i++;
			i = skip_space(line, i); /*The first index that is not a space after the label name.*/
            j=0;
            while(line[i] != '\0' && isspace(line[i]) == 0)  /*A loop that receives the second word.*/
            {
                word = (char *)realloc(word, sizeof(char)*(j+1));
                if(word == NULL) /*Check if memory allocation succeeded.*/
                {
                    result.error[error_index] = "No space\n";
                    return result;
                }
                word[j] = line[i]; /*Inserting the character from the line into its position in the word.*/
                i++;
                j++;
            } /* end of while*/
            word = (char *)realloc(word, j+1);
            if(word == NULL) /*Check if memory allocation succeeded.*/
            {
                result.error[error_index] = "No space\n";
                return result;
            }
            word[j] = '\0'; /*End of the word.*/

            /*Analysis of the second word*/
            if(word[0] == '.') /*Checking if this is a directive line for data or string.*/ {
                result.syntax_type = dir_sentence;
                if(strncmp(word,".data", 5) == 0)
                {
                    int numbers_arr_result;
					if(strlen(word) > 5) /*No space after the directive word.*/
                    {
                        result.error[error_index] = "There is no space immediately after '.data'\n";
                        error_index++;
                    }

                    result.syntax_option.dir.dir_type = data_sentence;
                    result.syntax_option.dir.dir_option.data = (struct data_arr *)malloc(sizeof(struct data_arr));
                    if(result.syntax_option.dir.dir_option.data == NULL)
                    {
                        result.error[error_index] = "No space\n";
                        return result;
                    }


                    if(comma_check(line,i)!=0) /*Invalid commas.*/
                    {
                        if(comma_check(line,i) == 1)
                            result.error[error_index] = "There is a comma immediately after '.data'\n";
                        else if(comma_check(line,i) == 2)
                            result.error[error_index] = "There is more than one comma in a sequence\n";
                        else
                            result.error[error_index] = "There is a comma at the end of the data instruction\n";
                        error_index++;
                    }
                    numbers_arr_result = numbers_arr(line,i,&result.syntax_option.dir.dir_option.data); /*Filling the data array.*/
                    if(numbers_arr_result !=0) /*The numbers/constants are not valid.*/
                    {
                        if(numbers_arr_result==1)
                            result.error[error_index] = "Invalid label\n";
                        else if(numbers_arr_result==2)
                            result.error[error_index] = "The number is out of range\n";
                        else
                            result.error[error_index] = "No space\n";
                        /*free(result.syntax_option.dir.dir_option.data);*/
                    }
                }
                else if(strncmp(word,".string", 7) == 0)
                {
					char *string;
					int string_result;
					string = (char *)malloc((i+1)*sizeof(char));
                    if(strlen(word) > 7) /*There is no space after the directive word.*/
                    {
                        result.error[error_index] = "There is no space immediately after '.string'\n";
                        error_index++;
                    }
                    string_result = string_check(line, i, string); /*The code checks if the string is valid, and if it is, it updates the string.*/
                    if(string_result != 0)
                    {
                        if(string_result == 1)
                            result.error[error_index] = "There is a continuation after the second double quotes\n";
                        else if(string_result == 2)
                            result.error[error_index] = "The string does not start with double quotes\n";
                        else if(string_result == 3)
                            result.error[error_index] = "There are no second double quotes\n";
                        else /*string_result == 4*/
                            result.error[error_index] = "No space\n";
                        error_index++;
                    }
                    else /*The string is valid.*/
                    {
                        int len;
			result.syntax_option.dir.dir_type = string_sentence;
                        len = strlen(string);
                        result.syntax_option.dir.dir_option.string = (char *)malloc(len*sizeof(char));
                        strcpy(result.syntax_option.dir.dir_option.string , string); /*Updating in the structure.*/
                    }
                }
            }
        }
    }
    /* Let's check if this is an instruction line.
Note that "word" will be the first word in the sentence if there is no label, or the second word if there is a label.*/
    if(is_opcode(word) != -1)
    {
        int opcode;
	int comma_res;
	result.syntax_type = inst_sentence;
        opcode = is_opcode(word);
        result.syntax_option.inst.inst_type = opcode;
        comma_res = comma_check(line,i);
        if(comma_res != 0) /*The commas are not valid.*/
        {
            if(comma_res == 1)
                result.error[error_index] = "There is a comma after the opcode\n";
            if(comma_res == 2)
                result.error[error_index] = "There is more than one comma in a sequence\n";
            if(comma_res == 3)
                result.error[error_index] = "There is a comma at the end of the instruction sentence\n";
            error_index++;
        }
        else /*The commas are valid.*/
        {
            int operands_check = which_operand(line,i,opcode,result.syntax_option.inst.operands);
            if(operands_check != 0)
            {
                if(operands_check == 1)
                    result.error[error_index] = "The source operand does not match the command\n";
                if(operands_check == 2)
                    result.error[error_index] = "The target operand does not match the command\n";
                if(operands_check == 3)
                    result.error[error_index] = "The number of operands does not match the command name\n";
                if(operands_check == 4)
                    result.error[error_index] = "No space\n";
                if(operands_check == 5)
                    result.error[error_index] = "Missing comma between operands\n";
                if(operands_check == 6)
                    result.error[error_index] = "There are extra characters after the operands\n";
                error_index++;
            }
        }
    }
	/*free(word);*/
    return result;
}


/*The function receives a line, index and action code and checks if the instruction line is correct to execute the action code
   * She will also update the array of buildings
   *0- everything is fine
   *1- The source operand does not fit the command
   *2- The target operand does not match the command
*3-The number of operands does not match the name of the command
*4-There is not enough memory
*5- Missing a comma between the operands
*6- There are extra characters after the operands
*/
int which_operand(char *line,int index,int opcode,struct operands_struct *operands)
{
    int i=0;
    char *operand1 = NULL;
    char *operand2 = NULL;
    index = skip_space(line,index); /* The first index after the command name that is not a space */
    while(isspace(line[index])==0 && line[index] != ',' && line[index]!='\0') /*Finding the first operand*/
    {
        operand1 = (char *)realloc(operand1, (i+1)*sizeof(char));
        if(operand1 == NULL) /*Check if memory allocation succeeded.*/
            return 4;
        operand1[i]=line[index];
        i++;
        index++;
    }
    operand1 = (char *)realloc(operand1, (i+1)*sizeof(char));
    if(operand1 == NULL) /*Check if memory allocation succeeded.*/
        return 4;
    operand1[i] = '\0'; /*The first word*/

    if(isspace(line[index]) != 0) /*If we finished the loop because we reached a space*/
    {
        index = skip_space(line,index); /*The first non-space index after the first operand*/
		if(line[index] != ',' && line[index] != '\0' && line[index] != '\n') /*There is a character after the first operand that is not a comma*/
		    return 5;
    }
    else if(line[index] == ',') /*If we finished the loop because we reached a comma*/
        index++; /*Advance the index that comes after the comma.*/
     /*Find the second word.*/
    i=0;
    index = skip_space(line,index); /*The first index that is not a space after the comma*/
    while(isspace(line[index])==0 && line[index]!='\0') /*Find the second operand.*/
    {
        operand2 = (char *)realloc(operand2, (i+1)*sizeof(char));
        if(operand2 == NULL) /*Check if memory allocation succeeded.*/
            return 4;
        operand2[i]=line[index];
        i++;
        index++;
    }
    operand2 = (char *)realloc(operand2, (i+1)*sizeof(char));
    if(operand2 == NULL) /*Check if memory allocation succeeded.*/
        return 4;
    operand2[i] = '\0'; /*The second word*/

    index = skip_space(line,index); /*The first index that is not a space after the second word.*/
    if(line[index] != '\0') /*If we found a character after the second operand*/
        return 6; /*There are extra characters after the operands.*/


    if(strcmp(legal_methods_table[opcode].source , "") == 0 && operand2[0] == '\0') /*If there's only one operand or no operands at all*/
    {
        operands[0].operand_type = no_operand;
        if(strcmp(legal_methods_table[opcode].dest , "") == 0 && (opcode == 14 || opcode==15) && operand1[0] == '\0') /*There are no operands.*/
        {
            operands[1].operand_type = no_operand;
            return 0;
        }
        if(operand1[0] != '\0') /*If there's only one operand, and it's the destination operand*/ {
            if ((strcmp(legal_methods_table[opcode].dest, "13") == 0 &&
                    (opcode == 9 || opcode == 10 || opcode == 13)) ||
                (strcmp(legal_methods_table[opcode].dest, "123") == 0 &&
                    (opcode == 4 || opcode == 5 || opcode == 7 || opcode == 8 || opcode == 11)) ||
                (strcmp(legal_methods_table[opcode].dest, "0123") == 0 && opcode == 12)) /*Destination operand type 1 or 3.*/
            {
                if(is_label(operand1) == 0) /*If the destination operand is a label, meaning it's immediate addressing.*/
                {
                    operands[1].operand_type = direct_label;
                    strcpy(operands[1].operand_option.dir_label ,operand1);
                    return 0;
                }
                if(is_reg(operand1) != -1) /*If the destination operand is a register, meaning it's register addressing.*/
                {
                    operands[1].operand_type = reg;
                    operands[1].operand_option.reg = is_reg(operand1);
                    return 0;
                }
            }
            if (((strcmp(legal_methods_table[opcode].dest, "123")) == 0 &&
                        (opcode == 4 || opcode == 5 || opcode == 7 || opcode == 8 || opcode == 11))||
                      (strcmp(legal_methods_table[opcode].dest, "0123") == 0 && opcode == 12)) /*If the destination operand is a label, meaning it's direct addressing.*/
            {
                char label[LABEL_LENGTH]={0}; /*The label.*/
                char num_label[LABEL_LENGTH]={0}; /*Optional numerical constant.*/
                if(is_fixed_index(operand1,label,num_label)!=3000)
                {
                    operands[1].operand_type = fixed_index;
                    strcpy(operands[1].operand_option.fixed_index_options.label , label);
                    if(is_fixed_index(operand1,label,num_label) == 4000) /*The array index is a constant.*/
                        strcpy(operands[1].operand_option.fixed_index_options.index_option.label , num_label);
                    else /*The array index is a number.*/
                        operands[1].operand_option.fixed_index_options.index_option.number = is_fixed_index(operand1,label,num_label);
                    return 0;
                }
            }
            if (strcmp(legal_methods_table[opcode].dest, "0123") == 0 && opcode == 12) /*The destination operand is of type 0.*/
            {
                char label_str[LABEL_LENGTH]={0};
                if(is_immed_num(operand1) != 3000) /*The destination operand is #number -a direct addressing.*/ {
                    operands[1].operand_type = immediate;
                    operands[1].operand_option.immed.number = is_immed_num(operand1);
                    return 0;
                }
                if(is_immed_label(operand1,label_str) != NULL) /*The target operand is #fixed -a direct addressing.*/
                {
                    operands[1].operand_type = immediate;
                    strcpy(operands[1].operand_option.immed.label,label_str);
                    return 0;
                }
            }
            else /*The destination operand does not match the command name.*/
                return 2;
        }

    }
    else if(operand1[0] != '\0' && operand2[0] != '\0')/*There are two operands*/
    {
        char label_str[LABEL_LENGTH]={0}; /*Optional constant in direct addressing*/
        char label[LABEL_LENGTH]={0}; /*The label in fixed index*/
        char num_label[LABEL_LENGTH]={0}; /*Optional numeric constant in fixed index*/
        int fixed_index_result;

        if(strcmp(legal_methods_table[opcode].source, "0123") == 0) /*Source operand accepts any type of operand*/
        {
            /*Update the source operand accordingly*/

            if(is_immed_num(operand1) != 3000) /*Source operand is a #number - immediate addressing*/
            {
                operands[0].operand_type = immediate;
                operands[0].operand_option.immed.number = is_immed_num(operand1);
            }
            else if(is_immed_label(operand1,label_str) != NULL) /*Source operand is a #constant - immediate addressing*/
            {
                operands[0].operand_type = immediate;
                strcpy(operands[0].operand_option.immed.label,label_str);
            }
            else if(is_reg(operand1) != -1) /*Source operand is a register - register addressing*/
            {
                operands[0].operand_type = reg;
                operands[0].operand_option.reg = is_reg(operand1);
            }
            else if(is_label(operand1) == 0) /*If the source operand is a label - direct addressing*/
            {
                operands[0].operand_type = direct_label;
                strcpy(operands[0].operand_option.dir_label,operand1);

            }
            else if((fixed_index_result=is_fixed_index(operand1,label,num_label))!=3000) /*Source operand is fixed index - type 2*/
            {
                operands[0].operand_type = fixed_index;
                strcpy(operands[0].operand_option.fixed_index_options.label , label);
                if(fixed_index_result == 4000) /*Array index is constant*/
                    strcpy(operands[0].operand_option.fixed_index_options.index_option.label , num_label);
                else /*Array index is a number*/
                    operands[0].operand_option.fixed_index_options.index_option.number = fixed_index_result;
            }
            else /*Source operand does not match the command*/
                return 1;
		

            /*Update the destination operand accordingly*/
            if(((strcmp(legal_methods_table[opcode].dest, "123") == 0) &&
                    (opcode==0 || opcode==2 || opcode==3)) ||
                ((strcmp(legal_methods_table[opcode].dest, "0123") == 0) &&
                    (opcode==2))) /*Destination operand type 1 or 2 or 3*/
            {
                if(is_reg(operand2) != -1) /*Destination operand is a register - register addressing - type 3*/
                {
                    operands[1].operand_type = reg;
                    operands[1].operand_option.reg = is_reg(operand2);
                    return 0;
                }
                if(is_label(operand2) == 0) /*If the destination operand is a label - direct addressing - type 1*/
                {
                    operands[1].operand_type = direct_label;
                    strcpy(operands[1].operand_option.dir_label ,operand2);
                    return 0;
                }
				fixed_index_result=is_fixed_index(operand2,label,num_label);
            	if(fixed_index!=3000) /*Destination operand type 2*/
            	{
                	operands[1].operand_type = fixed_index;
                	strcpy(operands[1].operand_option.fixed_index_options.label , label);
                    	if(fixed_index_result == 4000) /*The array index is fixed*/
                        	strcpy(operands[1].operand_option.fixed_index_options.index_option.label , num_label);
                	else /*The array index is a number*/
                    	operands[1].operand_option.fixed_index_options.index_option.number = fixed_index_result;
               	 return 0;
            	}
                else /*Destination operand does not match the command*/
                    return 2;
            }
            else if((strcmp(legal_methods_table[opcode].dest, "0123") == 0) && (opcode==1)) /*Destination operand type 0*/
            {
                memset(label_str, 0, sizeof(label_str));
                if(is_immed_num(operand2) != 3000) /*Destination operand is a #number - immediate addressing*/
                {
                    operands[1].operand_type = immediate;
                    operands[1].operand_option.immed.number = is_immed_num(operand2);
                    return 0;
                }
                if(is_immed_label(operand2,label_str) != NULL) /*אופרנדDestination operand is a #constant - immediate addressing*/
                {
                    operands[1].operand_type = immediate;
                    strcpy(operands[1].operand_option.immed.label,label_str);
                    return 0;
                }
                else /*Destination operand does not match the command*/
                    return 2;
            }
        }
        else if(strcmp(legal_methods_table[opcode].source, "12") == 0
            && strcmp(legal_methods_table[opcode].dest, "123") == 0
            && opcode==6) /*The last possibility left if there are two operands*/
        {
            char label[LABEL_LENGTH]={0}; /*The label in fixed index*/
            char num_label[LABEL_LENGTH]={0}; /*Optional numeric constant in fixed index*/
            /*Update the source operand accordingly*/
            if(is_label(operand1) == 0) /*If the source operand is a label - direct addressing*/
            {
                operands[0].operand_type = direct_label;
                strcpy(operands[0].operand_option.dir_label ,operand1);
            }
            else if(is_fixed_index(operand1,label,num_label)!=3000) /*Source operand is fixed index - type 2*/
            {
                operands[0].operand_type = fixed_index;
                strcpy(operands[0].operand_option.fixed_index_options.label , label);
                if(is_fixed_index(operand1,label,num_label) == 4000) /*Array index is constant*/
                    strcpy(operands[0].operand_option.fixed_index_options.index_option.label , num_label);
                else /*Array index is a number*/
                    operands[0].operand_option.fixed_index_options.index_option.number = is_fixed_index(operand1,label,num_label);
                return 0;
            }
            else /*Source operand does not match the command*/
                return 1;

            /*Update the destination operand accordingly*/
            if(is_label(operand2) == 0) /* אם אופרנד היעד הוא תווית -כלומרIf the destination operand is a label - direct addressing - type 1*/
            {
                operands[1].operand_type = direct_label;
                strcpy(operands[1].operand_option.dir_label ,operand2);
                return 0;
            }
            if(is_fixed_index(operand1,label,num_label)) /*Destination operand type 2*/
            {
                operands[1].operand_type = fixed_index;
                strcpy(operands[1].operand_option.fixed_index_options.label , label);
                if(is_fixed_index(operand1,label,num_label) == 4000) /*Array index is constant*/
                    strcpy(operands[1].operand_option.fixed_index_options.index_option.label , num_label);
                else /*Array index is a number*/
                    operands[1].operand_option.fixed_index_options.index_option.number = is_fixed_index(operand1,label,num_label);
                return 0;
            }
            if(is_reg(operand2) != -1) /*Destination operand is a register - register addressing - type 3*/
            {
                operands[1].operand_type = reg;
                operands[1].operand_option.reg = is_reg(operand2);
                return 0;
            }
            else /*Destination operand does not match the command*/
                return 2;
        }
    }
    else /*Number of operands does not match the command name*/
        return 3;

    free(operand1);
    free(operand2);
    return 0;
}


/*The function receives an operand and two empty strings (one for the label and one for the optional constant) and checks if the operand is of fixed index addressing type
 * If so, the function will update the label string to be the label and return the array index
 * If the array index is constant, it will update the received constant and return 4000
 *If invalid, it will return 3000
 */
int is_fixed_index(char *opernd,char *label,char *num_label)
{
	int i=0;
	int j=0;
    char num_str[MAX_NUMBER_LENGTH+1];
    int num;

    if(strchr(opernd,'[')==NULL || strchr(opernd,']')==NULL) /*No square brackets*/
        return 3000;
    while(opernd[i] != '[' && opernd[i]!='\0') /*Loop that retrieves the label*/
    {
        if(i>=LABEL_LENGTH) /*The label is too long*/
            return 3000;
        label[i] = opernd[i];
        i++;
    }
    label[i] = '\0';
    if(is_label(label) ==  1) /*Invalid label name*/
        return 3000;
    if(opernd[i] != '[') /*No opening bracket after the label*/
        return 3000;
    i++;
    while(opernd[i] != ']' && opernd[i]!='\0') /*Loop that retrieves the number*/
    {
        if(j>=MAX_NUMBER_LENGTH) /*The number is too long*/
            return 3000;
        num_str[j] = opernd[i];
		j++; /*The counter for the length of the number*/
        i++;
    }
    num_str[j] = '\0';
    if(opernd[i] != ']') /*No closing bracket after the number*/
        return 3000;
    if(is_label(num_str) == 0) /*If the number is a constant*/
    {
        strcpy(num_label,num_str);
        return 4000;
    }
    if(is_number(&num_str[0],MAX_NUMBER,MIN_NUMBER,&num)!=0) /*Check if the number is valid*/
        return 3000;
    return num;
}


/*The function receives a full string and an empty string and checks if the operand is an immediate constant
 *If valid, the function will fill the empty string and return the symbol name without the #
 *If invalid, it will return NULL
 */
char *is_immed_label(char *immed, char *label_str)
{
    int i=0;
    if(immed[0] != '#')
        return NULL;
    if(strlen(immed) > LABEL_LENGTH+1) /*The constant (including the #) is too long*/
        return NULL;
    while(i<LABEL_LENGTH && immed[i+1] != '\0')
    {
        label_str[i] = immed[i+1];
        i++;
    }
    label_str[i] = '\0';
    if(is_label(label_str) ==  1) /*Invalid label name*/
        return NULL;
    return label_str;
}

/*The function receives a string and checks if it is an immediate number operand
 *If valid, the function will return the operand's number
 *If invalid, it will return 3000
 */
int is_immed_num(char *immed)
{
    int i=0;
    int num;
    char num_str[MAX_NUMBER_LENGTH+1];
    if(immed[0] != '#')
        return 3000;
    if(strlen(immed) > MAX_NUMBER_LENGTH+2) /*The number (including the #) is too long*/
        return 3000;
    while(i<=MAX_NUMBER_LENGTH && immed[i+1] != '\0')
    {
        num_str[i] = immed[i+1];
        i++;
    }
    num_str[i] = '\0';
    if(is_number(&num_str[0],MAX_NUMBER,MIN_NUMBER,&num)!=0) /*Check if the number is valid*/
        return 3000;
    return num;
}


/*The function receives a string (without spaces) and checks if it is a register
 *0-7 is the register number
 *Minus 1 if not a register
 */
int is_reg(char *reg)
{
    int reg_num;
    if(strlen(reg)!=2 || reg[0] != 'r' || is_number(&reg[1],7,0,&reg_num) != 0) /*Invalid register*/
        return -1;
    return reg_num;
}


/*The function receives a string and checks if the opcode name is valid
 If valid, it returns the opcode number
 If invalid, it returns minus 1
 */
int is_opcode(char *opcode)
{
    int i=0;
    while(i<NUM_OF_OPCODE)
    {

        if(strcmp(opcode,legal_methods_table[i].name)== 0)
            return legal_methods_table[i].opcode;
        i++;
    }
    return -1;
}

/*The function receives a line and an index and checks if the extern or entry definition is valid
If valid, it updates the symbol
0 - all is valid
1 - invalid symbol
2 - extra characters that are not spaces after the symbol*/
int entry_extern_check(char *line,int index,char *label)
{
    int i = 0;
    index = skip_space(line, index); /*The first non-space index*/
    while(line[index] != ' ' && line[index] != '\t' && line[index] != '\0')
    {
        if(index>=LABEL_LENGTH) /*The symbol is too long*/
            return 1;
        label[i] = line[index];
        i++;
        index++;
    }
    label[i] = '\0';
    if(is_label(label)==1) /*Invalid symbol name*/
        return 1;
	/*A loop that checks that there are no unnecessary characters after the symbol - for some reason does not work*/
    /*while(line[index] != '\n' && line[index] != '\0' && line[index] != EOF)
    {
        if(isspace(line[index]) == 0) 
            return 2;
        index++;
    }*/

    return 0;
}

/*The function receives a line and an index and checks if the constant definition is valid.
If valid, it updates the constant's symbol and value (number).
0 - all is valid
1 - missing equal sign
2 - invalid constant name
3 - number out of range
4 - the number is a real number*/
int define_check(char *line,int index,char *label,int *number)
{
    int i = 0;
    char num_arr[MAX_NUMBER_LENGTH+1] = {0};
	int is_number_res;

    index = skip_space(line, index); /*The first non-space index after the "define"*/
    while(line[index] != ' ' && line[index] != '\t' && line[index] != '\0')
    {
        if(line[index] == '=')
            break;
		if(index>=LABEL_LENGTH) /*The constant is too long*/
            return 2;
        label[i] = line[index];
        i++;
        index++;
    }
    label[i] = '\0';
    if(is_label(label) == 1) /*Invalid constant name*/
        return 2;
    index = skip_space(line, index);
    if(line[index] != '=')
        return 1;
    index++;
    index = skip_space(line, index);
    i=0;
    while(line[index] != ' ' && line[index] != '\t' && line[index] != '\0')
    {
        if(i>MAX_NUMBER_LENGTH) /*The number is too long*/
            return 3;
        num_arr[i] = line[index];
        i++;
        index++;
    }
    num_arr[i] = '\0';
    is_number_res = is_number(num_arr,MAX_NUMBER,MIN_NUMBER,number);
    if(is_number_res == 1) /*The number is a real number*/
        return 4;
    else if(is_number_res == 2) /*The number is out of range*/
        return 3;
    return 0;
}

/*The function inserts the string within the quotes into the provided string.
0 - All is well.
1 - There are additional characters after the closing quotation marks.
2 - The sentence does not start with quotation marks.
3 - No closing quotation marks.
4 - Memory allocation error.*/
int string_check(char *line, int index, char *string)
{
    int i = 0;
    char *temp_string;
    index = skip_space(line, index); /*The first non-space index after the "string".*/
    if(line[index] != '"') /*The sentence does not start with quotation marks.*/
        return 2;
    index++;
    while(line[index] != '"' && line[index] != '\0')
    {
        temp_string = (char *)realloc(string,(i+1)*sizeof(char));
        if(temp_string==NULL) /*Check if memory allocation succeeded.*/
        {
            free(string);
            return 4;
        }
        string = temp_string;
        string[i] = line[index]; /*Copy each character within the quotes into the string.

*/
        i++;
        index++;
    }
    temp_string = (char *)realloc(string,(i+1)*sizeof(char));
    if(temp_string==NULL) /*Check if memory allocation succeeded.*/
    {
        free(string);
        return 4;
    }
    string = temp_string;
    string[i] = '\0';
    if(line[index]!='"') /*No closing quotation marks.*/
        return 3;
    index++;
	index = skip_space(line, index); /*The first non-space index.*/
    if(line[index] != '\0') /*There are additional characters after the closing quotation marks.*/
        return 1;
    return 0;
}


/*The function receives a string, index, and an array of pointers to appropriate structures.
It will insert the numbers/constants into the structure array accordingly.
0 - All is well.
1 - Invalid constant.
2 - Number out of range.
3 - Memory allocation failed.*/
int numbers_arr(char *line, int index, struct data_arr **dataArr)
{
    char *ptr;
    char *line_from_index = &line[index];
    char *line_without_space = without_spaces(line_from_index); /*Remove spaces.*/
    struct data_arr *temp_dataArr;
    char *comma = ",";
    int number;
    int i=0;
	int valid_num;
    ptr = strtok(line_without_space,comma);
    valid_num = is_number(ptr,MAX_NUMBER,MIN_NUMBER,&number); /*Check if the number is valid.*/

    temp_dataArr = (struct data_arr *)realloc(*dataArr,(i+1)*sizeof(struct data_arr));
    if(temp_dataArr == NULL) /*Check if memory allocation succeeded.*/
    {
        free(*dataArr);
        return 3;
    }
    *dataArr = temp_dataArr;

    if(valid_num == 0) /*Valid number.*/
    {
        (*dataArr)[i].data_type = data_number;
        (*dataArr)[i].data_options.number = number;
        i++;
    }
    else if(valid_num == 1) /*No number - so probably permanent*/
    {
        if(is_label(ptr)==1)
        {
            free(dataArr);
            return 1;
        }
        (*dataArr)[i].data_type = data_label;
        strcpy((*dataArr)[i].data_options.label,ptr);
        i++;
    }
    else /*Number out of range.*/
    {
        free(dataArr);
        return 2;
    }

    while((ptr=strtok(NULL,comma)) != NULL)
    {
        valid_num = is_number(ptr,MAX_NUMBER,MIN_NUMBER,&number);

        temp_dataArr = (struct data_arr *)realloc(*dataArr,(i+1)*sizeof(struct data_arr));
        if(temp_dataArr == NULL) /*Check if memory allocation succeeded.*/
        {
            free(*dataArr);
            return 3;
        }
        *dataArr = temp_dataArr;

        if(valid_num == 0) /*Valid number.*/
        {
            (*dataArr)[i].data_type = data_number;
            (*dataArr)[i].data_options.number = number;
        }
        else if(valid_num == 1) /*No number - so probably permanent*/
        {
            if(is_label(ptr)==1)
                return 1;
            (*dataArr)[i].data_type = data_label;
            strcpy((*dataArr)[i].data_options.label,ptr);
        }
        else /*Number out of range.*/
        {
            free(dataArr);
            return 2;
        }
        i++;
    }
    temp_dataArr = (struct data_arr *)realloc(*dataArr,(i+1)*sizeof(struct data_arr));
    if(temp_dataArr == NULL) /*Check if memory allocation succeeded.

*/
    {
        free(*dataArr);
        return 3;
    }
    *dataArr = temp_dataArr;
    
    (*dataArr)[i].data_options.label[0] = '\0';
    return 0;
}
/*The function receives a string and returns a copy without spaces.*/
char *without_spaces(char *line)
{
    int len = strlen(line);
    char *result = (char *)malloc(len*sizeof(char));
	int i=0,j=0;
    if (result == NULL) /*Check if memory allocation succeeded.*/
        return NULL;
    

    while(line[i] != '\0') /*Copy characters from the input string to the result string, excluding spaces and tabs.*/
    {
        if(line[i] != ' ' && line[i] != '\t')
        {
            result[j] = line[i];
            j++;
        }
        i++;

    }
    result[j] = '\0';
    return result;
}



/*The function receives a string and an index and checks for valid commas starting from the given index.
0 - Valid.
1 - Comma before the first number.
2 - More than one comma between two numbers.
3 - Comma after the last number.*/
int comma_check(char *line, int index)
{
    index = skip_space(line,index); /*The first non-space index.*/
    if(line[index] == ',') /*Comma before the first number.*/
    {
        return 1;
    }
    while(line[index] != '\0') /*Iterate through the entire string.*/
    {
        if(line[index] == ',')
        {
			index++;
            index = skip_space(line,index);
            if(line[index] == ',') /*Found a double comma.*/
                return 2;
            else if(line[index] == '\0') /*Comma after the last number.*/
            {
                return 3;
            }
        }
        index++;
    }
    return 0;
}



/*The function receives a string and an index and returns the first non-space index after it.*/
int skip_space(char *word, int index) {
    while(word[index] != '\0' && isspace(word[index])!=0) /*Skip spaces.*/
        index++;
    return index;
}

/*The function receives a string representing a number and checks if it is a real number within the range.
0 - Number.
1 - Not a number.
2 - Number out of range.*/
int is_number(char *str, int max, int min, int *result) /*Check if the string is a valid number.*/
{
    char *p;
	long int num;

    if (str == NULL || *str == '\0') 
        return 1;

    num = strtol(str, &p, 10);
    if(*p!='\0')
        return 1;
    if(num<min || num>max) /*Check if the number is within range.*/
        return 2;
    *result = (int)num;
    return 0;
}

/*The function receives a string and checks if it is a valid label.
0 - Label.
1 - Invalid label.*/
int is_label(char *str)
{
    int i=1;
    if (str == NULL || *str == '\0') 
        return 1;
    if(strlen(str) > LABEL_LENGTH) /*The label length is invalid.*/
        return 1;
    if(isalpha(str[0]) == 0) /*The label does not start with an alphabetic character.*/
        return 1;
    while (str[i] != '\0') { /*The label contains invalid characters (not a letter or digit).*/
        if(isalpha(str[i]) == 0 && isdigit(str[i])==0)
            return 1;
        i++;
    }
    return 0;
}

#include "pre_assem.h"

/*
 main function to check this file:
int main(int argc,char *argv[])
{
    printf("%s \n",pre_ass(argv[1]));
    return 0;
}
*/


/*The function receives a pointer to the callers table, the table's size, and the macro name being searched.
*If the macro name is found in the callers table, it returns a pointer to the corresponding macro; otherwise, it returns NULL.*/
MACRO *findMacro(MACRO *mcr_table, int mcr_table_size, const char *mcr_name)
{
    int i;
    for(i=0;i<mcr_table_size;i++)
    {
        if(strcmp(mcr_table[i].name,mcr_name)==0)
            return &mcr_table[i];
    }
    return NULL;
}

void advance_after_space(char **ptr) {
    /*As long as the current character is a space, we'll move to the next character.*/
    while ((**ptr) != '\0' && isspace(**ptr)) {
        (*ptr)++;
    }
}


/*The function receives a line, a pointer to a macro, a pointer to the macro table, and the size of the macro table.
 *The function will return the current state (macro definition/start of macro definition/macro call/other line).
 *If we are in the process of defining the content of a macro, the function will set the pointer to be the pointer to the current macro; otherwise, it will reset it.
 *If we are in a macro call, the function will initialize the pointer to point to the corresponding macro in the macro table.
 */
enum state which_line(char *line, MACRO **mcr_ptr, MACRO *mcr_table, int *mcr_table_size)
{
    char *c1=line, *c2;
    MACRO *f;

    c1 = strstr(line,"endmcr");
    if(c1!=NULL)
    {
        *mcr_ptr = NULL;
        return endmcr_definition;
    }

    c1 = strstr(line,"mcr");
    if(c1!=NULL)
    {
        c1 +=3; /*We'll advance the pointer to point to the character after "mcr".*/
        advance_after_space(&c1);  /*The pointer will point to the next character that is not a space, i.e., the beginning of the macro name.*/
        c2=strpbrk(c1,WHITESPACE); /*c2 points to the first non-space character after the macro name.*/
        if(c2!=NULL) /*So we found the first non-space character.*/
        {
            *c2='\0'; /*We add to the end of the macro name to form a valid string.*/
        }
        strcpy(mcr_table[*mcr_table_size].name,c1);
        *mcr_ptr = &mcr_table[*mcr_table_size]; /*Now the pointer points to the current macro in the macro table.*/
        (*mcr_table_size)++;
        return mcr_definition;
    }

    f= findMacro(mcr_table, *mcr_table_size, line);
    if(f!=NULL)
    {
        (*mcr_ptr) = f;
        return mcr_call;
    }

    return other_line;
}


/*The function receives the name of the original file and returns the name of the new file after macro expansion.*/
char *pre_ass(char *name_code)
{
    char line[MAX_LINE] = {0};
    MACRO *mcr_table;
    int mcr_count=0;
    FILE *am_file;
    FILE *as_file;
    MACRO *mcr_ptr = NULL;
    int name_size = (int)strlen(name_code)+4; /*The file name + .as*/
    char *amFileName;
    char *asFileName;
	char *p;
    int i=0;

    mcr_table = (MACRO *)malloc((mcr_count+1)*sizeof(MACRO));
    if(mcr_table == NULL)
    {
        printf("No space\n");
        return NULL;
    }

    amFileName = (char *)malloc(sizeof(char) * name_size);
    if(amFileName == NULL)
    {
        printf("No space\n");
        return NULL;
    }
    asFileName = (char *)malloc(sizeof(char) * name_size);
    if(asFileName == NULL)
    {
        printf("No space\n");
        return NULL;
    }

    strcpy(amFileName, name_code);
    strcat(amFileName, ".am");
    strcpy(asFileName, name_code);
    strcat(asFileName, ".as");

    am_file = fopen(amFileName, "w");
    as_file = fopen(asFileName, "r");

    if(am_file==NULL || as_file==NULL)
    {
        printf("Cannot open file %s \n",name_code);
        return NULL;
    }


    while(fgets(line,MAX_LINE,as_file) != NULL)
    {
        p = strchr(line,'\n');
        if(p!=NULL)
            *p = '\0';
		switch(which_line(line,&mcr_ptr,&mcr_table[0],&mcr_count))
        {
            case endmcr_definition: /*If we finished reading a macro definition, we reset the pointer.*/
                mcr_ptr = NULL;
            break;

            case mcr_definition: /*If we reached a macro definition line, we increase the macro array by 1 for the next macro.*/
                mcr_ptr->num_of_line = 0;
            break;

            case other_line:
                printf("in case other_line\n");
                if(mcr_ptr!=NULL) /*That means we are in the process of defining the content of a macro.*/
                {
                    strcpy(mcr_ptr->mcr_content[mcr_ptr->num_of_line],line);
                    (mcr_ptr->num_of_line)++;
                }
                else /*Any other line.*/
                {
                    fprintf(am_file,"%s\n",line);
                }
            break;

            case mcr_call:
		        while(mcr_ptr->num_of_line>0)
		        {
		            fprintf(am_file,"%s\n",mcr_ptr->mcr_content[i]);
		            mcr_ptr->num_of_line--;
					i++;
		        }
                mcr_ptr = NULL;
            break;
        }
    }

    fclose(as_file);
	fclose(am_file);
	free(asFileName);
    free(mcr_table);
    return amFileName;
}





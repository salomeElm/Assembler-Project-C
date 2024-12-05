#include "files.h"

/*The function receives a file name, the encoded lines table, and the symbols table.
The function creates an object file for the received file that contains the code encoded in 4 encrypted.*/
void print_ob_file(char *file_name, struct bin_code_table *bin_code_ptr,struct LABELS *label_table)
{
    char *ob_file_name;
    FILE *ob_file;
    int i,k; 
    int name_size = (int)strlen(file_name)+4; /*The file name + .ob */
    struct myWord word_14={0};

    printf("files.c check- IC= %d\n",label_table[0].IC);
    printf("files.c check- DC= %d\n",label_table[0].DC);

    ob_file_name = (char *)malloc(sizeof(char) * name_size);
    if(ob_file_name == NULL)
    {
        printf("No space\n");
        return;
    }
    strcpy(ob_file_name, file_name);
    strcat(ob_file_name, ".ob");
    
    ob_file = fopen(ob_file_name, "w");
    
    if(ob_file!=NULL) /*Check if the file opening succeeded.*/
    {
    	fprintf(ob_file,"\t%d %d\n",label_table[0].IC,label_table[0].DC); /* The header - number of data and instruction lines. */
        for(i=0;i<label_table[0].IC;i++)
        {
            char empty[SIZE_ENCODED_4 + 1] = {0}; /*An empty string of length 8 (the length of the string in encrypted 4 plus null).*/
            short int code_line = bin_code_ptr->code_table[i*sizeof(short int)]; /*The number in the encoded instructions table whose binary value is the encoded code.*/
            short_int_to_myWord(&word_14,code_line); /*Insert the rightmost 14 bits into word_14.*/
            printf("files.c check- in bits (code):");
            for (k = 13; k >= 0; k--) {
                printf("%d", (bin_code_ptr->code_table[i*sizeof(short int)] >> k) & 1);
            }
            printf("\n");
            printf("files.c check- translation: %d: %s\n",i+100,translation_4(&word_14,empty));
            fprintf(ob_file,"%04d\t%s\n",i+100,translation_4(&word_14,empty));
        }
        for(i=0;i<label_table->DC;i++)
        {
            char empty[SIZE_ENCODED_4 + 1] = {0}; /*An empty string of length 8 (4 characters in encrypted form plus null).*/
            short int code_line = bin_code_ptr->data_table[i*sizeof(short int)]; /*The number in the encoded data table whose binary value is the encoded code.*/
            short_int_to_myWord(&word_14,code_line); /*Insert the rightmost 14 bits into word_14.*/
            printf("files.c check- in bits (data):");
            for (k = 13; k >= 0; k--) {
                printf("%d", (bin_code_ptr->data_table[i*sizeof(short int)] >> k) & 1);
            }
            printf("\n");
            printf("files.c check- translation: %d: %s\n",(label_table->IC)+i+100,translation_4(&word_14,empty));
            fprintf(ob_file,"%04d\t%s\n",(label_table->IC)+i+100,translation_4(&word_14,empty));
        }
        fclose(ob_file);
    }
    free(ob_file_name);
}


/* The function receives a file name and the internal symbols table.
The function creates a corresponding ent file constructed from text lines according to the project instructions.*/
void print_ent_file(char *file_name,struct ent_table *ent_ptr)
{
    char *ent_file_name;
    FILE *ent_file;
    int i;
    int name_size = (int)strlen(file_name)+4; /*The file name + .ent*/

    ent_file_name = (char *)malloc(sizeof(char) * name_size);
    if(ent_file_name == NULL)
    {
        printf("No space\n");
        return;
    }
    strcpy(ent_file_name, file_name);
    strcat(ent_file_name, ".ent");

    ent_file = fopen(ent_file_name, "w");

    if(ent_file!=NULL) /*Check if the file opening succeeded.*/
    {
        for(i=0;i<ent_ptr->ent_count;i++)
        {
            fprintf(ent_file,"%s\t%04d\n",ent_ptr[i].ent_name,ent_ptr[i].value);
        }
        fclose(ent_file);
    }
    free(ent_file_name);
}


/*The function receives a file name and the external symbols table. The function creates a corresponding ext file constructed from text lines according to the project instructions.*/
void print_ext_file(char *file_name,struct ext_table *ext_ptr)
{
    char *ext_file_name;
    FILE *ext_file;
    int i;
    int name_size = (int)strlen(file_name)+4; /*The file name + .ext*/

    ext_file_name = (char *)malloc(sizeof(char) * name_size);
    if(ext_file_name == NULL)
    {
        printf("No space\n");
        return;
    }
    strcpy(ext_file_name, file_name);
    strcat(ext_file_name, ".ext");

    ext_file = fopen(ext_file_name, "w");

    if(ext_file!=NULL) /*Check if the file opening succeeded.*/
    {
        for(i=0;i<ext_ptr->ext_count;i++)
        {
            fprintf(ext_file,"%s\t%04d\n",ext_ptr[i].ext_name,ext_ptr[i].value);
        }
        fclose(ext_file);
    }
    free(ext_file_name);
}

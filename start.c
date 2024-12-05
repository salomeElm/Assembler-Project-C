#include "start.h"


int main(int argc, char *argv[])
{
    char *amFileName;
    FILE *am_file;
    struct LABELS *lab_ptr;
    struct ext_table *ext_ptr;
    struct ent_table *ent_ptr;
    struct bin_code_table *bin_code_ptr;
    int i;

    lab_ptr= (struct LABELS *)malloc(sizeof(struct LABELS)); /*The symbols table.*/
    if(lab_ptr == NULL)
    {
        printf("No space /n");
        return 0;
    }
    ext_ptr= (struct ext_table *)malloc(sizeof(struct ext_table)); /*The external symbols table.*/
    if(ext_ptr == NULL)
    {
        printf("No space /n");
        return 0;
    }
    ent_ptr= (struct ent_table *)malloc(sizeof(struct ent_table)); /*The internal symbols table.*/
    if(ent_ptr == NULL)
    {
        printf("No space /n");
        return 0;
    }
    bin_code_ptr= (struct bin_code_table *)malloc(sizeof(struct bin_code_table)); /*The encoded instructions and data table.*/
    if(bin_code_ptr == NULL)
    {
        printf("No space /n");
        return 0;
    }

    if(argc==1)
    {
        printf("No files received\n");
        return 0;
    }

    for(i=1;i<argc;i++)
    {
        reset_structs(lab_ptr,ext_ptr,ent_ptr,bin_code_ptr); /*Every time a new file is read, all structures will be reset*/
        amFileName = pre_ass(argv[i]);

        if(amFileName != NULL) /*Macro expansion was successful.*/
        {
            am_file = fopen(amFileName,"r");
            if(am_file!=NULL) /*File opening succeeded.*/
            {
				printf("*********************open am_file succes************************ \n");
                if(firstPass(&lab_ptr,&ent_ptr,amFileName,am_file)==0) /*First pass succeeded.*/
                {
					printf("*********************firstPass succes************************ \n");
                    rewind(am_file); /*Rewind to the beginning of the file to perform the second pass.*/
                    if(secondPass(lab_ptr,bin_code_ptr,&ext_ptr,amFileName,am_file)==0) /*Second pass succeeded.*/
                    {
						printf("*********************secondPass succes************************ \n");
                        print_ob_file(argv[i],bin_code_ptr,lab_ptr);
						printf("*********************print_ob_file succes************************ \n");
                        if(ent_ptr[0].ent_count > 0) /*The internal symbols table is not empty.*/
                        {
                            print_ent_file(argv[i],ent_ptr);
                            printf("*********************print_ent_file succes************************ \n");
                        }

                        if(ext_ptr[0].ext_count > 0) /*The external symbols table is not empty.*/
                        {
                            print_ext_file(argv[i],ext_ptr);
                            printf("*********************print_ext_file succes************************ \n");
                        }
                    }
                }
                fclose(am_file);
            }
                free(lab_ptr);
                free(ent_ptr);   
                free(ext_ptr);
                free(bin_code_ptr);
            	free(amFileName);
        }
    }
    return 0;
}


void reset_structs(struct LABELS *lab_ptr,struct ext_table *ext_ptr,struct ent_table *ent_ptr,struct bin_code_table *bin_code_ptr)
{
    /*Reset the symbols table.*/
    memset(lab_ptr->name, 0, sizeof(lab_ptr->name));
    lab_ptr->value = 0;
    lab_ptr->type = 0;
    lab_ptr->IC = 0;
    lab_ptr->DC = 0;

    /*Reset the external symbols table.*/
    memset(ext_ptr->ext_name, 0, sizeof(ext_ptr->ext_name));
    ext_ptr->value=0;
    ext_ptr->ext_count=0;

    /*Reset the internal symbols table.*/
    memset(ent_ptr->ent_name, 0, sizeof(ent_ptr->ent_name));
    ent_ptr->value=0;
    ent_ptr->ent_count=0;

    /*Reset the coded data and instruction table.*/
    if (bin_code_ptr->code_table != NULL) /*Reset the instructions table.*/
    {
        free(bin_code_ptr->code_table);
    }
    bin_code_ptr->code_table = NULL;
    if (bin_code_ptr->data_table != NULL) /*Reset the data table.*/
    {
        free(bin_code_ptr->data_table);
    }
    bin_code_ptr->data_table = NULL;
}

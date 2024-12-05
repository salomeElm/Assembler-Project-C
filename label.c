#include "label.h"


/*The function adds a new symbol (its name and value) to the symbols array at the last position. */
void add_label(struct LABELS **lab_ptr, char *lab_name, int lab_value, int *lab_num, int lab_type) {
    struct LABELS *temp_lab_ptr;
    temp_lab_ptr = realloc(*lab_ptr, (*lab_num + 1) * sizeof(struct LABELS)); /*Increase the size of the symbols array by 1.*/
    if (temp_lab_ptr == NULL) {
        printf("No space\n");
        return;
    }
    *lab_ptr=temp_lab_ptr;
    if(strlen(lab_name)>LABEL_LENGTH)
    {
        printf("The label %s is too long /n",lab_name);
        return;
    }
    strcpy((*lab_ptr)[*lab_num].name, lab_name);
    (*lab_ptr)[*lab_num].value = lab_value; /*Update the value of the symbol in the corresponding field in the last item in the array.*/
    (*lab_ptr)[*lab_num].type = lab_type; /*Update the type (attribute) of the symbol in the corresponding field in the last item in the array.*/
    (*lab_num)++; /*Increase the number of symbols by 1.*/
}



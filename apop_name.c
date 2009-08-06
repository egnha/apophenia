/** \file apop_name.c

Copyright (c) 2006--2007 by Ben Klemens.  Licensed under the modified GNU GPL v2; see COPYING and COPYING2.  */

#include "db.h" //just for apop_opts.verbose.
#include "types.h"
#include <stdio.h>
#include <regex.h>

/** \defgroup names apop_names: a structure and functions to handle column names for matrices. 
\ingroup types */

/** Allocates a name structure
\return	An allocated, empty name structure.
\ingroup names
*/
apop_name * apop_name_alloc(void){
    apop_name	* init_me = malloc(sizeof(apop_name));
    apop_assert(init_me, NULL, 0, 's', "malloc failed. Probably out of memory.");
    *init_me = (apop_name){ };
	init_me->title[0]   = '\0';
	return init_me;
}

/** Adds a name to the \ref apop_name structure. Puts it at the end of the given list.

\param n 	An existing, allocated \ref apop_name structure.
\param add_me 	A string. If NULL, do nothing; return -1.
\param type 	'r': add a row name<br>
'c': add a column name<br>
't': add a text category name<br>
'h': add a title (or a header. 't' is taken).<br>
'v': add (or overwrite) the vector name<br>
\return 	Returns the number of rows/cols/depvars after you have added the new one.
\ingroup names
*/
int apop_name_add(apop_name * n, char *add_me, char type){
    if (!add_me)
        return -1;
	if (type == 'h'){
        if (add_me){
            snprintf(n->title, 100, add_me);
		    return 1;
        } else return 0;
	} 
	if (type == 'v'){
        if (add_me){
		n->vector	= realloc(n->vector,  strlen(add_me) + 1);
		strcpy(n->vector, add_me);
		return 1;
        } else return 0;
	} 
	if (type == 'r'){
		(n->rowct)++;
		n->row	= realloc(n->row, sizeof(char*) * n->rowct);
		n->row[n->rowct -1]	= malloc(strlen(add_me) + 1);
		strcpy(n->row[n->rowct -1], add_me);
		return n->rowct;
	} 
	if (type == 't'){
		(n->textct)++;
		n->text	= realloc(n->text, sizeof(char*) * n->textct);
		n->text[n->textct -1]	= malloc(strlen(add_me) + 1);
		strcpy(n->text[n->textct -1], add_me);
		return n->textct;
	}
	//else assume (type == 'c'){
        if (type != 'c' && apop_opts.verbose)
            apop_error(2,'c',"%s: You gave me >%c<, I'm assuming you meant c; "
                             " copying column names.\n", __func__, type);
		(n->colct)++;
		n->column	= realloc(n->column, sizeof(char*) * n->colct);
		n->column[n->colct -1]	= malloc(strlen(add_me) + 1);
		strcpy(n->column[n->colct -1], add_me);
		return n->colct;
	//} 
}

/** Prints the given list of names to STDOUT
\param n	the \ref apop_name structure
\ingroup names
*/
void  apop_name_print(apop_name * n){
int		i;
	if (n->vector){
		printf("\t\t\tvector:");
			printf("\t%s", n->vector);
		printf("\n");
	}
	if (n->colct > 0){
		printf("\t\t\tcolumn:");
		for (i=0; i < n->colct; i++)
			printf("\t%s", n->column[i]);
		printf("\n");
	}
	if (n->textct > 0){
		printf("\t\t\ttext:");
		for (i=0; i < n->textct; i++)
			printf("\t%s", n->text[i]);
		printf("\n");
	}
	if (n->rowct > 0){
		printf("\t\t\trow:");
		for (i=0; i < n->rowct; i++)
			printf("\t%s", n->row[i]);
		printf("\n");
	}
}
	
/** Erases an \ref apop_name structure.
\ingroup names 	*/
void  apop_name_free(apop_name * free_me){
int		i;
	for (i=0; i < free_me->colct; i++)
		free(free_me->column[i]);
	for (i=0; i < free_me->textct; i++)
		free(free_me->text[i]);
	for (i=0; i < free_me->rowct; i++)
		free(free_me->row[i]);
    if (free_me->vector);
        free(free_me->vector);
	free(free_me->column);
	free(free_me->text);
	free(free_me->row);
	free(free_me);
}

/** Append one list of names to another.

Notice that if the first list is NULL, then this is a copy function. If the second is NULL, it is a no-op.

\param  n1      The first set of names (no default, must not be \c NULL)
\param  nadd      The second set of names, which will be appended after the first. (no default, if \c NULL, a no-op)
\param type1     Either 'c', 'r', 't', or 'v' stating whether you are merging the columns, rows, or text. If 'v', then ignore \c typeadd and just overwrite the target vector name with the source name. (default = 'r')
\param typeadd     Either 'c', 'r', 't', or 'v' stating whether you are merging the columns, rows, or text. If 'v', then overwrite the target with the source vector name. (default = type1)
\ingroup names */
APOP_VAR_HEAD void  apop_name_stack(apop_name * n1, apop_name *nadd, char type1, char typeadd){
    apop_name * apop_varad_var(n1, NULL);
    apop_assert_void(n1, 0, 'c', "Can't stack onto a NULL data set (which n1 is).");
    apop_name * apop_varad_var(nadd, NULL);
    char  apop_varad_var(type1, 'r');
    char  apop_varad_var(typeadd, type1);
    apop_name_stack_base(n1, nadd, type1, typeadd);
APOP_VAR_ENDHEAD
  int     i;
    if (!nadd)
        return;
    if (typeadd == 'v')
        apop_name_add(n1, nadd->vector, 'v');
      else if (typeadd == 'r'){
        for (i=0; i< nadd->rowct; i++)
            apop_name_add(n1, nadd->row[i], type1);
    } else if (typeadd == 't'){
        for (i=0; i< nadd->textct; i++)
            apop_name_add(n1, nadd->text[i], type1);
    } else if (typeadd == 'c'){
        for (i=0; i< nadd->colct; i++)
            apop_name_add(n1, nadd->column[i], type1);
    } else if (apop_opts.verbose)
         printf (">%c< sent to apop_name_stack, but the only valid options are r t c v. Doing nothing.\n",type1);
}

/** Deprecated. Use \ref apop_name_stack.  \ingroup names */
void  apop_name_cross_stack(apop_name * n1, apop_name *nadd, char type1, char typeadd){
    printf("apop_name_cross_stack is deprecated. Just use apop_name_stack, which I will run for you now.\n");
    apop_name_stack(n1, nadd, type1, typeadd);
}

/** Copy one \ref apop_name structure to another. That is, all data is duplicated. Usage:

\code
apop_name *out  = apop_name_copy(in);
\endcode
 
    \param in    the input names
    \return       a structure that this function will allocate and fill
    \ingroup names
  */
apop_name * apop_name_copy(apop_name *in){
  apop_name *out = apop_name_alloc();
    apop_name_stack(out, in, 'v');
    apop_name_stack(out, in, 'c');
    apop_name_stack(out, in, 'r');
    apop_name_stack(out, in, 't');
    snprintf(out->title, 100, in->title);
    return out;
}

/** Finds the position of an element in a list of names.

The function uses case-insensitive regular expressions to search. 

For example, "p.val.*" will match "P value", "p.value", and "p values".

\param n        the \ref apop_name object to search.
\param in       the name you seek; see above.
\param type     'c', 'r', or 't'. Default is 'c'.
\return         The position of \c findme. If not found, returns -1.
\ingroup names
  */
int  apop_name_find(apop_name *n, char *in, char type){
  regex_t   re;
  char      **list;
  int       i, listct;
    if (type == 'r'){
        list    = n->row;
        listct  = n->rowct;
    }
    else if (type == 't'){
        list    = n->text;
        listct  = n->textct;
    }
    else { // default: (type == 'c')
        list    = n->column;
        listct  = n->colct;
    }
    regcomp(&re, in, REG_EXTENDED + REG_ICASE);
    for (i = 0; i < listct; i++){
        if (!regexec(&re, list[i], 0, NULL, 0)){
            regfree(&re);
            return i;
        }
    }
    regfree(&re);
    return -1;
}

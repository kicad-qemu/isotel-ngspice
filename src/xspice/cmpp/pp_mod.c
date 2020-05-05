/*============================================================================
FILE  pp_mod.c

MEMBER OF process cmpp

Copyright 1991
Georgia Tech Research Corporation
Atlanta, Georgia 30332
All Rights Reserved

PROJECT A-8503

AUTHORS

    9/12/91  Steve Tynor

MODIFICATIONS

    <date> <person name> <nature of modifications>

SUMMARY

    This file contains the top-level driver function for preprocessing the
    "cfunc.mod" file.  First, the "ifspec.ifs" file is opened and parsed to
    get the data that will be needed in the .mod to .c translation  (See
    read_ifs.c).  Then the .mod file is translated.  Most of the work of the
    translation is handled by the UNIX 'lex' and 'yacc' utilities.  This
    translation is begun at the call to mod_yyparse() below.  See also files:

        mod_lex.l
        mod_yacc.y

    Note that to allow lex/yacc to be used twice (once for the ifspec.ifs
    file, and then again for the cfunc.mod file), the functions created by
    lex/yacc for the latter are translated using the UNIX text editor 'sed'
    under the direction of the Makefile and the following 'sed scripts':

        mod_lex.sed
        mod_yacc.sed

    Hence the call to 'mod_yyparse()' rather than 'yyparse()' below.

INTERFACES

    preprocess_mod_file()

REFERENCED FILES

    None.

NON-STANDARD FEATURES

    None.

============================================================================*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include  "mod_yacc_y.h"

extern int mod_yyparse(void);
extern void mod_yyrestart (FILE*);

extern FILE *mod_yyin;
extern FILE *mod_yyout;
extern int   mod_yylineno;
extern char *mod_yytext;

extern int   mod_num_errors;

extern Ifs_Table_t *mod_ifs_table;

extern char *current_filename;
extern char *prog_name;

/*---------------------------------------------------------------------------*/
/* Allocate and build a file name from the input filename by changing its
 * extension (text after the last '.') in filename to ext or append .ext if
 * the filename has no extension */
static char *change_extension(const char *filename, const char *ext)
{
   const char * const p = strrchr(filename, '.');
   const size_t prefix_len = p ? (size_t) (p - filename) : strlen(filename);
   const size_t ext_len = strlen(ext);
   char * const new_filename = malloc(prefix_len + ext_len + 2);
                                                /* +1 for '.' +1 for '\0' */
   if (new_filename == (char *) NULL) {
       return (char *) NULL;
   }

   {
       char *p_cur = (char *) memcpy(new_filename, filename, prefix_len) +
            prefix_len;
       *p_cur++ = '.';
       (void) memcpy(p_cur, ext, ext_len + 1);
   }

   return new_filename;
} /* end of function change_extension */

/*---------------------------------------------------------------------------*/

/*
preprocess_mod_file

Function preprocess_mod_file is the top-level driver function for
preprocessing a code model file (cfunc.mod).  This function calls
read_ifs_file() requesting it to read and parse the Interface
Specification file (ifspec.ifs) and place the information
contained in it into an internal data structure.  It then calls
mod_yyparse() to read the cfunc.mod file and translate it
according to the Interface Specification information.  Function
mod_yyparse() is automatically generated by UNIX lex/yacc
utilities.
*/


void preprocess_mod_file (
        const char *filename)         /* The file to read */
{
    Ifs_Table_t     ifs_table;   /* info read from ifspec.ifs file */
    int status;      /* Return status */
    char *output_filename = (char *) NULL; /* .mod file being written */

   /*
    * Read the entire ifspec.ifs file and load the data into ifs_table
    */
   
   status = read_ifs_file (IFSPEC_FILENAME, GET_IFS_TABLE, &ifs_table);
   
   if (status != 0) {
      exit(1);
   }

    /* Open the cfunc.mod file defining the code model function */
    if ((current_filename = gen_filename(filename, "r")) == (char *) NULL) {
        print_error("ERROR - Unable to build mod file name");
        exit(1);
    }
    if ((mod_yyin = fopen(current_filename, "r")) == (FILE *) NULL) {
        print_error("ERROR - Unable to open mod file \"%s\": %s",
                current_filename, strerror(errno));
        exit(1);
    }

    {
        char *output_filename_base = (char *) NULL;
        if ((output_filename_base = change_extension(
                filename, "c")) == (char *) NULL) {
            print_error("ERROR - Could not change extension of "
                    "\"%s\".", filename);
            exit(1);
        }

        if ((output_filename = gen_filename(
                output_filename_base, "w")) == (char *) NULL) {
            print_error("ERROR - Unable to build output file name");
            exit(1);
        }

        free(output_filename_base);
    }

    if ((mod_yyout = fopen(output_filename, "w")) == (FILE *) NULL) {
        /* .c file could not be opened */
        print_error("ERROR - Could not open output .c file\"%s\": %s",
                output_filename, strerror(errno));
        exit(1);
    }

   mod_ifs_table = &ifs_table;
   mod_num_errors = 0;

/* Define DEBUG_WITH_MOD_FILE to have the compiler use the cfunc.mod file for
 * deugging instead of the cfunc.c file. */
#ifdef DEBUG_WITH_MOD_FILE
   fprintf (mod_yyout, "#line 1 \"%s\"\n", current_filename);
#endif
   fprintf (mod_yyout, "#include \"ngspice/cm.h\"\n");
   fprintf (mod_yyout, "extern void %s(Mif_Private_t *);\n",
          ifs_table.name.c_fcn_name);
#ifdef DEBUG_WITH_MOD_FILE
   fprintf (mod_yyout, "#line 1 \"%s\"\n", current_filename);
#endif

   mod_yylineno = 1;

    if (mod_yyparse() || (mod_num_errors > 0)) {
        print_error("Error parsing .mod file: \"%s\"", current_filename);
        unlink(output_filename);
        exit(1);
    }

    if (fclose(mod_yyout) != 0) {
        print_error("Error closing output file \"%s\": %s",
                current_filename, strerror(errno));
        unlink(output_filename);
        exit(1);
    }

    rem_ifs_table(&ifs_table);
    mod_yyrestart(NULL);
    free(output_filename);
    free(current_filename);
} /* end of function preprocess_mod_file */



/*---------------------------------------------------------------------------*/
void
mod_yyerror (char *str)
{
   fprintf (stderr, "%s: Error: \"%s\": line %d (near \'%s\'):\n\t%s.\n",
            prog_name, current_filename, mod_yylineno, mod_yytext, str);
}


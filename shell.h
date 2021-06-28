/* Do not modify this file */

#ifndef SHELL_H
#define SHELL_H

#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdbool.h>
#include<errno.h>
#include<signal.h>

#include "logging.h"

#define MAXLINE 100 /* the max number of chars in one command line */
#define MAXARGS 50 /* the max number of arguments in one command line */

typedef struct cmd_aux_struct{
    char *in_file;
    char *out_file;
    int is_append;
    int is_bg;
}Cmd_aux;

/* The record that keeps additional information about the user command.
 * - in_file: input file name specified by user command (with <); 
 *            it should be NULL if no redirection (with <);
 * - out_file: output file name specified by user command (with > or >>), 
 *            it should be NULL if no redirection (with > or >>);  
 * - is_append: int flag to specify how to change the output file:
 *               --  1 : the output file should be appended
 *               --  0 : the output file should be erased
 *               --  -1: no output file specified
 * - is_bg: int flag to speficy whether the job is background:
 *           -- 1 : background job
 *           -- 0 : foreground job
 */


void parse(char *cmd_line, char *argv[], Cmd_aux *aux);
/* The required function to parse the user command into useful pieces.
 * - cmd_line: the line typed in by user WITHOUT the ending \n.
 * - argv: the array of NULL terminated char pointers. 
 *   	-- argv[0] should be either a built-in command, or the name of the
 *   	   program to be loaded and executed;
 *   	-- the remainder of argv[] should be the list of arguments used 
 *   	   to run the command/file
 *   	-- argv[] must have NULL as its last member after all arguments
 * - aux: the pointer to a Cmd_aux record which should be filled with
 *        the other information regarding the task as defined for Cmd_aux 
 */

#endif /*SHELL_H*/

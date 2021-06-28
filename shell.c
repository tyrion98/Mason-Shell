/* This is the only file you should update and submit. */
/*
 * Name: DeJone' Williamson
 * Project 3
 * GNumber:G01119038
 */

#include "logging.h"
#include "shell.h"

/* Constants */
static const char *shell_path[] = {"./", "/usr/bin/", NULL};
static const char *built_ins[] = 
			{"quit", "help", "kill", "jobs", "fg", "bg", NULL};

/* global int var to retrieve certin cmd names */
int cmd_int = 0;
// int var
int inwards = 0, outwards = 0;
// job add
void job_add(int pid, char *the_cmd);
// job remove
void job_remove(int pid);
/* status change method */
void statusChange(int pid, char c);
/* make a global cmd for bg handler */
//char glob_cmd[MAXLINE];
int isRedirect(char *argv[]);
/* Feel free to define additional functions and update existing lines */
void nonBuiltBG(char *argv[], char *cmd);
// runs the non built in programs
void nonBuilt(char *argv[], char *cmd);
// sets up aux for non built in vars
int nonAux(Cmd_aux **aux, char *argv[]);
// does the redirection function stuff
void redirection(Cmd_aux *aux, char *argv[], char *cmd);
// does redirection bg stuff
void redirectionBG(Cmd_aux *aux, char *argv[], char *cmd);
// do diff for redirect
void redirectAux(Cmd_aux **aux, char **argv[]);
/* handler for bg process */
void bghandler(int sig);
// handles built in kill method
void killHandle(int sig_num, pid_t kill_pid);
/* find the cmd associated with the process */
char *findCommand(pid_t pid);
// fg switch function for built in cmd
void fg_switch(int pid, char *cmd);
// bg sigcont
void bg_switch(int pid, char *cmd);
/* stores process id and cmd line */
typedef struct id_cmd{
	int pid;
	char *cmdLine;
}idCmd;
/* create a list of structs that stores job info*/
typedef struct job_info{
	int job_id;
	int process_id;
	char *state;
	char init_cmd_line[MAXLINE];
} Job;
// global job list array
Job job_list[100];
// global for current fg process id
int fg_pid = 0;
// boolean to say if the ctrl z was called
int is_Trigger = -1, isCtrlz = -1;
// ctrl z
struct sigaction ctrlz, old;
void zHandle(int sig){
	// take the signal and print something to show its working
	// FIX LATER
	log_ctrl_z();
	// set ctrlz to 1
	isCtrlz = 1;
}
// control handle
void controlHandle(int sig){
	// take the signal and print something to show its working
	//
	log_ctrl_c();
	is_Trigger = 0;
	if(fg_pid != 0)
		kill(fg_pid, SIGSTOP);
	// send to foreground signal to terminate it
}
// /* main */
/* The entry of your shell program */
int main() 
{
	char cmdline[MAXLINE]; /* Command line */
	/* Intial Prompt and Welcome */
	log_prompt();
	log_help();
	// sig action
	struct sigaction ctrlc;	
	memset(&ctrlc, 0, sizeof(struct sigaction));
	ctrlc.sa_handler = controlHandle;
	sigaction(SIGINT, &ctrlc, NULL);
	// ctrl z	
	memset(&ctrlz, 0, sizeof(struct sigaction));
	ctrlz.sa_handler = zHandle;
	sigaction(SIGTSTP, &ctrlz, &old);
	// called w/o fg process
	// reset trigger
	/* Shell looping here to accept user command and execute */
	while (1) {

		char *argv[MAXARGS]; /* Argument list */
        	Cmd_aux aux; /* Auxilliary cmd info: check shell.h */
   	
		/* Print prompt */
  		log_prompt();
		/* Read a line */
		// note: fgets will keep the ending '\n'
		if (fgets(cmdline, MAXLINE, stdin)==NULL)
		{
		   	if (errno == EINTR)
				continue;
		    	exit(-1); 
		}
	
		if (feof(stdin)) {
		    	exit(0);
		}
	
		/* Parse command line */
    		cmdline[strlen(cmdline)-1] = '\0';  /* remove trailing '\n' */
		parse(cmdline, argv, &aux);		
	
		/* Evaluate command */
		/* add your implementation here */

 	   }

}
// void clean up when exit
void cleanUP(){
	int i = 0;
	char *temp = job_list[0].state;
	// free the structs
	while(job_list[i].state != NULL){
		
		// free temp
		temp = job_list[i].state;

		free(temp);		
		// increment
		i++;
	}
}

/* func that extracts all the args */
void allParsed(char *cmd_line, char **argv[]){
	// define
	char *tok;
	// inc var
	int i = 0;
	// assign first
	tok = strtok(cmd_line," ");
	while(tok != NULL){
		// set
		(*argv)[i] = tok;
		// ADD TO LIST
		tok = strtok(NULL, " ");
		// inc i
		i++;

	}
	// last should be null 
	(*argv)[i] = NULL;
}

/* built in code to aux */
Cmd_aux *built_in_aux(Cmd_aux *aux){
	// so we do not have to mallo
	Cmd_aux *final = aux;

	// change
	// if built in then in file is null,
	// out file is null
	// is append is -1
	// is bg is 0
	final -> in_file =  NULL;
	final -> out_file = NULL;
	final -> is_bg = 0;
	final -> is_append = -1;
	//finally set in and out to null
	//final -> in_file = NULL;
	//final -> out_file = NULL;	



	// return pointer
	return final;

}
void printAllJobs(){
	// inc var
	int i = 0;
	// for loop that prints all jobs
	while(job_list[i].state != NULL){
		
		log_job_details(job_list[i].job_id, job_list[i].process_id, job_list[i].state, job_list[i].init_cmd_line);
		i++;
	}




}
/* end main */

/* required function as your staring point; 
 * check shell.h for details
 */

void parse(char *cmd_line, char *argv[], Cmd_aux *aux){
	
	// int vals
	int kill_pid = 0, sig_num = 0, job_id = 0;
	int record = 0, redirect = 0;
	char backup[MAXLINE];
	// if empty ignore
	// empty meaning its not null but has no chars in it either
	if((cmd_line != NULL) && (cmd_line[0] == '\0')){
		return;
	}
	// try to replicate simple commands like:
	// ls, pwd, help
	strcpy(backup, cmd_line);	
	/* create a simple function that reads in all the strs */
	allParsed(cmd_line, &argv);	
	// BUILT IN COMMANDS WITH NO ARGS 
	// quit
	if(strcmp(argv[0], built_ins[0]) == 0){
		// QUIT 
		// code to aux
		aux = built_in_aux(aux);
		// display quit message
		log_quit();
		// clean up
		// cleanUP();
		// LATER EXIT ALL FG PROCESSES THEN EXIT	
		// free(backup);
		// then exit
		exit(1);
	}
	else if(strcmp(argv[0], built_ins[1]) == 0){
		// HELP
		//code to aux
		aux = built_in_aux(aux);
		// display help message
		log_help();
		// thats it
		return; 
	}
	else if(strcmp(argv[0], built_ins[3]) == 0){
		// JOBS
		// code to aux
		aux = built_in_aux(aux);
		// first report how many jobs are alive
		log_job_number(cmd_int);
		// index find 
		printAllJobs();
		// return ?
		return;
	}
	else if(strcmp(argv[0], built_ins[2]) == 0){
		// KILL
		// code to aux
		aux = built_in_aux(aux);
		// sig num
		
		sig_num = atoi(argv[1]);
		kill_pid = atoi(argv[2]);
		// kill built in command
		log_kill(sig_num, kill_pid);
		// takes you to a function that handles what ever 
		// signal you send to a process
		killHandle(sig_num, kill_pid);

		// return
		return;

	}else if(strcmp(argv[0], built_ins[4]) == 0){
		//FG COMMAND
		// switches specified bg job to the fg
		// then wait until it completes
		// if stopped resume its execution after moving
		// back to fg
		job_id = atoi(argv[1]);
		// if wutever number it is
		// send necessary info to a function
		fg_switch(job_id, backup);

		// return 
		return;

	}else if(strcmp(argv[0], built_ins[5]) == 0){
		//BG COMMAND
		// your shell will resume execution of bg job
		job_id = atoi(argv[1]);
		// with specified id
		bg_switch(job_id, backup);

		// return at the end
		return;
	}
	// separate stuff for redirect stuff
	redirect = isRedirect(argv);
	// if 0 then send to redirect aux func
	if(redirect == 0){
		redirectAux(&aux, &argv);
		// check bg
		if(aux -> is_bg){
			redirectionBG(aux, argv, backup);
			return;
	
		}
		// do the redirect with the info given 
		redirection(aux, argv, backup);	

	}else{
		/* FOR NON BUILT IN CMDS */
		// no built in aux set up 
		record = nonAux(&aux, argv);
		// if bg not 0
		if(aux -> is_bg != 0){
			argv[record] = NULL;
			nonBuiltBG(argv, backup);
			/* run parsing and then check if fg or bg
 			*  depending on which send to diff funcs */	
		}
		else{
			nonBuilt(argv, backup);
		}
	}
	// free backup since done 
	//free(backup);
}

// finds pid based off a job id 
int findPID(int job_id){
	// pid
	pid_t pid = -1;
	int i = 0;
	
	// incr
	while(job_list[i].state != NULL){

		// find pid
		if(job_id == job_list[i].job_id){

			pid = job_list[i].process_id;
			break;
		}
	}
	// return pid
	return pid;

}
// find out the state of a process given a pid
char *getState(int pid){
	// inc
	int i = 0;
	char *ret = NULL;
	while(job_list[i].state != NULL){

		if(job_list[i].process_id == pid){

			ret = job_list[i].state;			

		}
	
		// inc
		i++;
	}

	return ret;

}
// built in cmd fg
void bg_switch(int job_id, char *cmd){
	// yup
	pid_t pid = 0;
	char *state = NULL;
	// a function that checks for a pid
	pid = findPID(job_id);
	// if none found or its equal to -1
	// then it doesnt exist
	// print error msg and return
	if(pid == -1){
		// not found
		log_jobid_error(job_id);
		// return
		return;
	}
	// find out the state of the program
	state = getState(pid);

	// if stopped then do this else return
	if(strcmp(state, "Stopped") == 0){
		// log
		log_job_bg(pid, cmd);
		// just sigcont
		killHandle(18, pid);
	}
	else{
		// log
		log_job_bg(pid, cmd);

		return;
	}	
}

// built in cmd fg
void fg_switch(int job_id, char *cmd){
	// yup
	pid_t pid = 0;
	char *newCmd;
	char *state = NULL;
	// a function that checks for a pid
	pid = findPID(job_id);
	// if none found or its equal to -1
	// then it doesnt exist
	// print error msg and return
	if(pid == -1){
		// not found
		log_jobid_error(job_id);
		// return
		return;
	}
	// get job cmd
	newCmd = findCommand(pid);
	// make it switch to ya
	// stop the command 
	kill(SIGSTOP, pid);
	kill(SIGCONT, pid);
	// finally print success
	log_job_fg(pid, cmd);
	// stopped process
	state = getState(pid);	
	if(strcmp(state, "Stopped") == 0)
		log_job_fg_cont(pid, cmd);	

	waitpid(-1, NULL, WUNTRACED);
	//kill(SIGCONT, pid);
	log_job_fg_term(pid, newCmd);
	// continue when switched
	// remove from list
	job_remove(pid);
	// quick fix?
	//cmd_int--;
	// finally print success
	//log_job_fg(pid, cmd);
}

/* find the cmd associated with the process */
char *findCommand(pid_t pid){

	// define
	// int var
	int i = 0;
	// while loop that goes thru the job lists
	while(job_list[i].state != NULL){

		// find the process with the process id
		if(job_list[i].process_id == pid){

			// return cmd
			return job_list[i].init_cmd_line;

		}		
	}

	// else
	// return null
	return NULL;
}
// handles built in kill method
void killHandle(int sig_num, pid_t kill_pid){


	char *cmd;
	// func that looks for process id
	cmd = findCommand(kill_pid); 
	// returns the cmd of that process
	// if returns null then process doesnt exit!
	if(cmd == NULL) return;
	// HAVE TO DO IT FOR FG PROCESSES TOO
	// for background processes
	// lets use switch statements
	switch(sig_num){

		// 2(SIGINT)
		case 2:
			// log it
			// send the signal
			// update info for job list 
			// kill(pid, signal);
			// if kill returns -1 then it has failed
			break;
		// 9(SIGKILL)
		case 9:
			// log it
			log_job_bg_term_sig(kill_pid, cmd);
			// send the signal
			kill(SIGKILL, kill_pid);
			// update status in job list
			statusChange(kill_pid, 't');
	
			// break

			break;
		// 18(SIGCONT)
		case 18:

			// only should be continued if its state is stopped
			// log
			log_job_bg_cont(kill_pid, cmd);
			// send the signal
			kill(SIGCONT, kill_pid);
			// update status in job list
			statusChange(kill_pid, 'c');
	
			// break
			break;
		// 19(SIGSTOP)
		case 19:
			// log
			log_job_bg_stopped(kill_pid, cmd);

			// send the signal
			kill(SIGSTOP, kill_pid);
			// update status
			statusChange(kill_pid, 's');
			// break
			break;
		// else statement
		default:
			// just return?
			return;

	}
	// prints out different things based off the signal 

}
/* status change method */
void statusChange(int pid, char c){
	// based on the char
	int i = 0;
	
	// change the status of that process
	while(job_list[i].state != NULL){

		// find the pid
		if(job_list[i].process_id == pid){

			//if char is c
			if(c == 'c'){
				job_list[i].state = (char *) malloc(MAXLINE);	
				//printf("Running\n");
				strcpy(job_list[i].state , "Running");
			}
			//if char is s
			else if(c == 's'){
				job_list[i].state = (char *) malloc(MAXLINE);
				//printf("Stopped\n");
				strcpy(job_list[i].state , "Stopped");
			}
			//if c is t
			else if(c == 't'){
				job_list[i].state = (char *) malloc(MAXLINE); 
				//printf("Terminated\n");
				strcpy(job_list[i].state , "Terminated");
			}
		}
		i++;
	}
}
// does redirection bg stuff
void redirectionBG(Cmd_aux *aux, char *argv[], char *cmd){
	// FILE DESC VARIABLE
	//int status = 0; 
	int file_d = 0, file_d2;
	pid_t pid = 0;
	int i = 0;
	// path stuff
	char *fullPath = malloc(sizeof(MAXLINE));
	strcpy(fullPath, shell_path[1]);
	strcat(fullPath, argv[0]);
	// int s
	int s = 0;
	// open the file
	char *secretArgv[MAXARGS];
	// check if there is any flags
	while(argv[i] != NULL){
		secretArgv[i] = argv[i];
		// incre
		i++;
	}
	secretArgv[i++] = aux -> in_file;
	secretArgv[i] = aux -> out_file;


	// fork
	pid = fork();
	// create a process
	if(pid == 0){
		//check for in <
		if(inwards){
			// fd
			file_d = open(aux -> in_file, O_RDONLY, 0600);
			// check if file_d failed
			if(file_d < 0){
				// err msg
				log_file_open_error(aux -> in_file);
				// exit
				exit(0);
			}else{
				// it succeeded
				dup2(file_d, STDIN_FILENO);
				// then close
				close(file_d);
			}
			// resest inwards
			inwards = -1;
		}
		// check for out >
		if(outwards){
			// fd
			file_d2 = creat(aux -> out_file, 0600);
			// check if file_d failed
			if(file_d2 < 0){ 
				// err msg
				log_file_open_error(aux -> in_file);
				// exit
				exit(0);
			}else{
				// it succeeded
				dup2(file_d2, STDOUT_FILENO);
				// then close
				close(file_d2);
			}
			// reset outwards
			outwards = -1;
		}
		// check for >>
		if(aux -> is_append){
			// fd
			file_d2 = open(aux -> out_file, O_RDWR | O_APPEND, 0600);
			// check if file_d failed
			if(file_d2 < 0){ 
				// err msg
				log_file_open_error(aux -> in_file);
				// exit
				exit(0);
			}else{
				// it succeeded
				dup2(file_d2, STDOUT_FILENO);
				// then close
				close(file_d2);
			}
					
		}

		// run exec
		s = execv(fullPath, secretArgv);
		// do other
		// /usr/bin path
		if(s == -1){
			strcpy(fullPath, shell_path[1]);
			strcat(fullPath, argv[0]);
			s = execv(fullPath, secretArgv);}
		// check again
		if(s == -1){
			log_command_error(cmd);	
			exit(-1);
		}

	}else if(pid > 0){
		// state that fg is being executed
		log_start_bg(pid, cmd);
		// add to struct
		job_add(pid, cmd);
		// wait	
		// sig action struct
		struct sigaction act;	
		memset(&act, 0, sizeof(struct sigaction));
		act.sa_handler = bghandler;


		// call a signal handler
		sigaction(SIGCHLD, &act, NULL);


	}else{


		// error msg
		log_command_error(cmd);
		// exit
		exit(0);


	}
	// file is created by open
	// use 0600 as third argument of open
	// then use dup2() to change io if call to open succeeded
	//free(fullPath);
	//free(secretArgv);
	// state that it terminated

}
// does the redirection function stuff
void redirection(Cmd_aux *aux, char *argv[], char *cmd){
	// FILE DESC VARIABLE
	int status = 0, file_d = 0, file_d2;
	pid_t pid = 0;
	// path stuff
	char fullPath[MAXLINE];
	strcpy(fullPath, shell_path[1]);
	strcat(fullPath, argv[0]);
	// int s
	int s = 0;
	// define	
	// open the file
	char *secretArgv[MAXARGS];
	secretArgv[0] = argv[0];
	secretArgv[1] = aux -> in_file;
	secretArgv[2] = aux -> out_file;

	// fork
	pid = fork();
	// create a process
	if(pid == 0){
		//check for in <
		if(inwards){
			// fd
			file_d = open(aux -> in_file, O_RDONLY, 0600);
			// check if file_d failed
			if(file_d < 0){
				// err msg
				log_file_open_error(aux -> in_file);
				// exit
				exit(0);
			}else{
				// it succeeded
				dup2(file_d, STDIN_FILENO);
				// then close
				close(file_d);
			}
			// resest inwards
			inwards = -1;
		}
		// check for out >
		if(outwards){
			// fd
			file_d2 = open(aux -> out_file, O_RDWR| O_CREAT, 0600);
			// check if file_d failed
			if(file_d2 < 0){ 
				// err msg
				log_file_open_error(aux -> in_file);
				// exit
				exit(0);
			}else{
				// it succeeded
				dup2(file_d2, STDOUT_FILENO);
				// then close
				close(file_d2);
			}
			// reset outwards
			outwards = -1;
		}
		// check for >>
		if(aux -> is_append){
			// fd
			file_d2 = open(aux -> out_file, O_CREAT | O_RDWR | O_APPEND, 0600);
			// check if file_d failed
			if(file_d2 < 0){ 
				// err msg
				log_file_open_error(aux -> in_file);
				// exit
				exit(0);
			}else{
				// it succeeded
				dup2(file_d2, STDOUT_FILENO);
				// then close
				close(file_d2);
			}
					
		}
		// run exec
		s = execv(fullPath, secretArgv);
		// do other
		// /usr/bin path
		if(s == -1){
			strcpy(fullPath, shell_path[1]);
			strcat(fullPath, argv[0]);
			s = execv(fullPath, secretArgv);}
		// check again
		if(s == -1){
			log_command_error(cmd);	
			exit(-1);
		}

	}else if(pid > 0){
		// parent process
		// sig action
		struct sigaction ctrl;	
		memset(&ctrl, 0, sizeof(struct sigaction));
		ctrl.sa_handler = controlHandle;

		sigaction(SIGINT, &ctrl, NULL);
		fg_pid = pid;
		// process grp
		setpgid(0,0);
		sigaction(SIGSTOP, &ctrlz, NULL);
		// state that fg is being executed
		log_start_fg(pid, cmd);
		// add to struct
		job_add(pid, cmd);
		// since its fg should have dummy job id num of 0
		job_list[cmd_int - 1].job_id = 0;

		// wait
		waitpid(pid, &status, 0);
			
		// IF CTRL Z PRESSED
		if(isCtrlz >= 0){
			// call kill handle
			log_job_fg_stopped(pid, cmd);
			// send the signal
			kill(19, pid);
			// update status
			statusChange(pid, 's');

		}

		// check if signal was called
		if(is_Trigger == -1 && isCtrlz == -1)
			// print successs
			log_job_fg_term(pid, cmd);
		else if(is_Trigger == 0 && isCtrlz == -1)
			log_job_fg_term_sig(pid, cmd);

		// DOOP		
		if(isCtrlz == -1)	
			// remove job from list
			job_remove(pid);

		// reset the vals
		is_Trigger = -1;
		isCtrlz = -1;
		fg_pid = 0;

	}else{


		// error msg
		log_command_error(cmd);
		// exit
		exit(0);


	}
	// file is created by open
	// use 0600 as third argument of open
	// then use dup2() to change io if call to open succeeded
	// state that it terminated
	//free(fullPath);
}

// find out if the user is tryna to do redirection
int isRedirect(char *argv[]){
	// int return
	int return_val = -1;
	int i = 0;
	// iterate
	while(argv[i] != NULL){
		// if one of the append things mark true 
		if((strcmp(argv[i], "<") == 0) || (strcmp(argv[i], ">") == 0) || (strcmp(argv[i], ">>") == 0)){
			return_val = 0;
			break;

		}
		// incremenet
		i++;
	}

	// return
	return return_val;
}
/* job add func */
// add to struct
void job_add(int pid, char the_cmd[MAXLINE]){


	// define and add
	job_list[cmd_int].job_id = cmd_int;
	job_list[cmd_int].process_id = pid;
	job_list[cmd_int].state = malloc(sizeof(MAXLINE));
	// malloc check
	if(job_list[cmd_int].state == NULL){

		printf("Malloc failed.");
		exit(0);

	}
	// state
	job_list[cmd_int].state = "Running";
	//job_list[cmd_int].init_cmd_line = malloc(sizeof(MAXLINE));
	strcpy(job_list[cmd_int].init_cmd_line, the_cmd);

	// inc job_id
	job_list[cmd_int].job_id++;
	// add
	cmd_int++;
}
/* a job remove function */
void job_remove(int pid){
	// inc
	int i = 0;
	// remove from job list
	while(job_list[i].state != NULL){
		
		// if pid found remove
		if(job_list[i].process_id == pid){

			job_list[i].job_id = 0;
			job_list[i].process_id = 0;
			job_list[i].state = NULL;
			strcpy(job_list[i].init_cmd_line, "");	
			break;
		}
	
		//inc i
		i++;
	}
	// free state and cmd list
	// subtract cmd int by one
	cmd_int--;
}
// for non built in commands, to make parse cleaner
void nonBuiltBG(char *argv[], char *cmd){
	// int child pid
	pid_t pid = 0;
	//int status = 0;
	int success = 0;
	// concat to end of the path
	char fullPath[MAXLINE];
	strcpy(fullPath, shell_path[0]);
	strcat(fullPath, argv[0]);
	// attempt the sleep non built in command
	// need to fork
	pid = fork();

	// do 
	if(pid == 0){
			
		// ./ path			  
		// execv
		success = execv(fullPath, argv);
		// check if successful
		// do other
		// /usr/bin path
		if(success == -1){
			strcpy(fullPath, shell_path[1]);
			strcat(fullPath, argv[0]);
			success = execv(fullPath, argv);}
		// check again
		if(success == -1){
			log_command_error(cmd);	

		}
	
		exit(0);
			
	}else if(pid > 0){
		// state that fg is being executed
		log_start_bg(pid, cmd);
		// add to struct
		job_add(pid, cmd);
		// wait
		// sig action struct
		struct sigaction act;	
		memset(&act, 0, sizeof(struct sigaction));
		act.sa_handler = bghandler;
		// call a signal handler
		sigaction(SIGCHLD, &act, NULL);
	}
	else{
		// failed to fork
		// log error 
		log_command_error(cmd);
		// exit
		exit(0); 		

	}
	
	// free
	//free(fullPath);	
	
	
	
}
/* finds index of the job info*/
int findIndex(int pid){
	int i = 0;	
	int found = 0;
	// for loop that prints all jobs
	while(job_list[i].state != NULL){
		// if found return that num
		if(job_list[i].process_id == pid){
			found = i;
			break;
		}
		i++;
	}
	return found;

}
/* bg handler */
void bghandler(int sig){
	pid_t pid;
	int index = 0, status = 0, count = 0;
	char *cmd;
	//cmd = findCommand(pid);
	while(((pid = waitpid(-1, &status, WNOHANG))> 0) && count < cmd_int){

		index = findIndex(pid);
		cmd = job_list[index].init_cmd_line;
		// log
		log_job_bg_term(pid, cmd);

		// remove
		//job_remove(pid)
		count++;
	}

	// remove from job list
	job_remove(pid);
}

// for non built in commands, to make parse cleaner
void nonBuilt(char *argv[], char *cmd){
	// int child pid
	pid_t pid = 0;
	int status = 0;
	int success = 0;
	// concat to end of the path
	char fullPath[MAXLINE];
	strcpy(fullPath, shell_path[0]);
	strcat(fullPath, argv[0]);
	// attempt the sleep non built in command
	// need to fork
	pid = fork();
	// do 
	if(pid == 0){
			
		// ./ path  
		// execv
		success = execv(fullPath, argv);
		// check if successful
		// do other
		// /usr/bin path
		if(success == -1){
			strcpy(fullPath, shell_path[1]);
			strcat(fullPath, argv[0]);
			success = execv(fullPath, argv);
			}
		// check again
		if(success == -1){
			log_command_error(cmd);	

		}
		exit(0);	
	}else if(pid > 0){
		// signal stuff
		// sig action
		struct sigaction ctrl;	
		memset(&ctrl, 0, sizeof(struct sigaction));
		ctrl.sa_handler = controlHandle;

		sigaction(SIGINT, &ctrl, NULL);
		fg_pid = pid;
		// gogo
		setpgid(0,0);
		sigaction(SIGTSTP, &ctrlz, NULL);
		// state that fg is being executed
		log_start_fg(pid, cmd);
		// add to struct
		job_add(pid, cmd);
		// since its fg should have dummy job id num of 0
		//job_list[cmd_int - 1].job_id = 0;
		// wait
		waitpid(pid, &status, 0);	

		// IF CTRLZ PRESSED	
		if(isCtrlz >= 0){
			// call kill handle
			log_job_fg_stopped(pid, cmd);
			// send the signal
			kill(19, pid);
			// update status
			statusChange(pid, 's');
			// SHOULD NOW BE A BACKGROUND JOB?
			// need to continue the process
		}

		// check if signal was called and ctrl z wasnt called
		if(is_Trigger == -1 && isCtrlz == -1)
			// print successs
			log_job_fg_term(pid, cmd);
		else if(is_Trigger == 0 && isCtrlz -1)
			log_job_fg_term_sig(pid, cmd);

		if(isCtrlz == -1)// remove from job list
			job_remove(pid);


		// reset the vals
		is_Trigger = -1;
		isCtrlz = -1;
		fg_pid = 0;
	}
	else{
		// failed to fork
		// log error 
		log_command_error(cmd);
		// exit
		exit(0); 		

	}
	
	//free(fullPath);

}

// sets up aux for non built in vars w no redirect
int nonAux(Cmd_aux **aux, char *argv[]){
	// set up the non aux vals
	int i = 0;
	// record
	int record = 0;
	// go thru all of the args to check if something is bg
	while(argv[i] != NULL){
		// if
		if(strcmp(argv[i], "&") == 0){
			(*aux) -> is_bg = 1;
			record = i;
		}
		else{
			// should be last var so should work
			(*aux) -> is_bg = 0;
		}
		// incr
		i++;
	}
	// error check
	if(record != i - 1){(*aux) -> is_bg = 0;}
	// set everything else to null and -1
	(*aux) -> in_file = NULL;
	(*aux) -> out_file = NULL;
	(*aux) -> is_append = -1;

	// return the func
	return record;

}
// do diff for redirect
void redirectAux(Cmd_aux **aux, char **argv[]){
	// create an in variable and out variable
	char *in = NULL;
	char *out = NULL;
	// int var
	int i = 0;
	int append_val = -1;
	// make is_bg 0 by default
	(*aux) -> is_bg = 0;
	// make a separate args list with correct vals
	char *newArgs[MAXARGS];
	// while loop
	while((*argv)[i] != NULL){
				
		// will probably fix later
		if((i < 2) && (strcmp((*argv)[i], ">") != 0) && (strcmp((*argv)[i], "<") != 0)){

			newArgs[i] = (*argv)[i];


		}
		// if > var the object after is the out file
		if((strcmp((*argv)[i], ">") == 0) && (*argv)[i+1] != NULL){
			// set the outfile
			out = (*argv)[i+1];
			outwards = 1;	
			// set append val
			append_val = 0;
			// set arg val to null
			//(*argv)[i] = NULL;	
			//(*argv)[i+1] = NULL;

		}
		else if((strcmp((*argv)[i], "<") == 0) && (*argv)[i+1] != NULL){

			// in file
			in = (*argv)[i+1];
			inwards = 1;
			// set append val
			append_val = 0;
			// set arg val to null
			//(*argv)[i] = NULL;	
			//(*argv)[i+1] = NULL;

		}
		else if((strcmp((*argv)[i], ">>") == 0) && (*argv)[i+1] != NULL){
			// set append
			out = (*argv)[i+1];
			// set val
			append_val = 1;
			// set arg val to null
			//(*argv)[i] = NULL;
			//(*argv)[i+1] = NULL;	
		}
		// check if bg found
		if(strcmp((*argv)[i], "&") == 0){
			// is a bg and should be set to null
			// set up if its bg or not
			(*aux) -> is_bg = 1;
			(*argv)[i] = NULL;		
	
		}
		// increment i
		i++;
	}
	// set everything else to null if needed
	(*aux) -> in_file = in;
	(*aux) -> out_file = out;
	(*aux) -> is_append = append_val;

	// finally change the value of the old args to the new one
	(*argv) = newArgs;

	// after used then free
	// for loop
	// free(newArgs);
}


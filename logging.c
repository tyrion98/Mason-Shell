/* Do Not Modify This File */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "logging.h"

#define shell_log(s) printf("\033[1;31m%s%s\n\033[0m",log_head, s); fflush(stdout)

#define shell_write(s) char output[255] = {0}; sprintf(output,"\033[1;31m%s%s\033[0m", log_head, s); write(STDOUT_FILENO, output, strlen(output));

const char *log_head = "[MALOG]";
/* Outputs the Help: All the Built-in Commands */
void log_help() { 
  shell_log("Welcome to MASH (MAson SHell)!");
  shell_log("Built-in Commands: fg, bg, jobs, kill, quit, help.");
  shell_log("\tkill SIGNAL PID");
  shell_log("\tfg JOBID");
  shell_log("\tbg JOBID");
}

/* Outputs the message after running quit */
void log_quit(){
  shell_log("Thanks for using MASH! Good-bye!");
}

/* Outputs the prompt */
void log_prompt() {
  printf("MASH>> ");
  fflush(stdout);
}


/* Output when the command is not found
 * eg. User typed in ls instead of /bin/ls and exec returns an error
 */ 
void log_command_error(char *line) {
  char buffer[255] = {0};
  sprintf(buffer, "Error: %s: Command Cannot Load", line);
  shell_log(buffer);
}

/* Output when starting a foreground process */
void log_start_fg(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer, "Foreground Process %d: %s Started", pid, cmd);
  shell_log(buffer);
} 
/* Output when starting a background process */
void log_start_bg(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer, "Background Process %d: %s Started", pid, cmd);
  shell_log(buffer);
} 

/* Output when using bg on a process */
void log_job_bg(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer, "Built-in Command bg for Process %d: %s", pid, cmd);
  shell_log(buffer);
}

/* Output when the given job id is not found */
void log_jobid_error(int job_id) {
  char buffer[255] = {0};
  sprintf(buffer, "Error: Job ID %d Not Found in Background Job List", job_id);
  shell_log(buffer);
}

/* Output when the job is moved to the foreground with fg */
void log_job_fg(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer, "Built-in Command fg for Process %d: %s", pid, cmd);
  shell_log(buffer);
}

/* Output when kill command is used */
void log_kill(int signal, int pid) {
  char buffer[255] = {0};
  sprintf(buffer, "Built-in Command kill for Sending Signal %d to Process %d", signal, pid);
  shell_log(buffer);
}

/* Output when ctrl-c is received */
void log_ctrl_c() {
  shell_log("Keyboard Combination control-c Received");
}

/* Output when ctrl-z is received */
void log_ctrl_z() {
  shell_log("Keyboard Combination control-z Received");
}


/* Output when a foreground job terminated normally.
 * (Signal Handler Safe Outputting)
 */
void log_job_fg_term(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer,"Foreground Process %d: %s Terminated Normally\n",pid,cmd);
  shell_write(buffer);
}

/* Output when a foreground job terminated due to a signal
 * (Signal Handler Safe Outputting)
 */
void log_job_fg_term_sig(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer,"Foreground Process %d: %s Terminated by Signal\n",pid,cmd);
  shell_write(buffer);
}

/* Output when a foreground job was continued.
 * (Signal Handler Safe Outputting)
 */
void log_job_fg_cont(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer,"Foreground Process %d: %s Continued\n", pid, cmd);
  shell_write(buffer);
}

/* Output when a background job terminated normally.
 * (Signal Handler Safe Outputting)
 */
void log_job_bg_term(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer,"Background Process %d: %s Terminated Normally\n",pid,cmd);
  shell_write(buffer);
}

/* Output when a background job terminated by a signal.
 * (Signal Handler Safe Outputting)
 */
void log_job_bg_term_sig(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer,"Background Process %d: %s Terminated by Signal\n",pid,cmd);
  shell_write(buffer);
}

/* Output when a background job was continued.
 * (Signal Handler Safe Outputting)
 */
void log_job_bg_cont(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer,"Background Process %d: %s Continued\n", pid, cmd);
  shell_write(buffer);
}

/* Output when a foreground job was stopped.
 * (Signal Handler Safe Outputting)
 */
void log_job_fg_stopped(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer,"Foreground Process %d: %s Stopped\n", pid, cmd);
  shell_write(buffer);
}

/* Output when a background job was stopped.
 * (Signal Handler Safe Outputting)
 */
void log_job_bg_stopped(int pid, char *cmd) {
  char buffer[255] = {0};
  sprintf(buffer,"Background Process %d: %s Stopped\n", pid, cmd);
  shell_write(buffer);
}

/* Output on file open errors */
void log_file_open_error(char *file_name) {
  char buffer[255] = {0};
  sprintf(buffer, "Error: Cannot Open File %s", file_name);
  shell_log(buffer);
}

/* Output to list the job counts */
void log_job_number(int num_jobs){
  char buffer[255] = {0};
  sprintf(buffer, "%d Job(s)", num_jobs);
  shell_log(buffer);
}

/* Output to detail a single job */
void log_job_details(int job_id, int pid, char *state, char *cmd){
  char buffer[255] = {0};
  sprintf(buffer, "Job %d: Process %d: %s %s", job_id, pid, state, cmd);
  shell_log(buffer);
}

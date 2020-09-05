/*
 * main.c
 *
 * A simple program to illustrate the use of the GNU Readline library
 */
#define _GNU_SOURCE
#include <sys/wait.h>
#include <bsd/string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#define MAX_LINE_LENGTH 500
typedef struct child_process {
	char* process;  
	pid_t cpid; 
	char* directory; 
	char status[20]; 
	char delete_status[20]; 
} child_process;
void string_to_array(char **string_array, char *string )
{
	for(int i = 0; i < 16; i++)
    {
		//set all values to NULL 	
		string_array[i] = NULL; 
	}
		
	int check = 0;
    //strip commands from string and place in array
	char *token = strtok(string, " "); 
	while (token != NULL)
	{
		if(check == 15)
		{
			printf("something is wrong\n"); 
				
		}
		else{ 
			//values need to now be set so malloc first 
			string_array[check] = malloc(MAX_LINE_LENGTH * sizeof(*string_array[check]));
			//copy the token into the array
			strncpy(string_array[check], token, strlen(token)+2);   
			
			token = strtok(NULL, " "); 
			check++; 
		}
	}
}

int main (int argc, char * argv[])
{
	int status; 
	pid_t pid; 
	struct child_process** child_list; 
	child_list = malloc(10 * sizeof(*child_list)); 
	for(int i = 0; i < 10; i++){
		child_list[i] = malloc(MAX_LINE_LENGTH * sizeof(*child_list[i])); 
		child_list[i]->process = malloc(MAX_LINE_LENGTH * sizeof(char)); 
		child_list[i]->directory = malloc(MAX_LINE_LENGTH * sizeof(char)); 
		strncpy(child_list[i]->delete_status, "D", 20);
		
	}
	 
	for (;;)
	{
		char dir_buf[300]; 
		char* dir_ptr = getcwd(dir_buf,257); 
		strncat(dir_ptr, ">", 257); 
		char *cmd = readline (dir_ptr);
		char **string_array; 
		//create an array to splice readline into
		string_array = malloc (16 * sizeof(*string_array));
		string_to_array(string_array, cmd); 
		if(string_array[0]  == NULL ){
			printf("invalid call of NULL\n"); 
		}
		// check if array[0] is cd
		else if(strcmp(string_array[0], "cd") == 0)
		{
			int value = chdir(string_array[1]); 
			if(value == -1)
			{
				printf("error has occured in cd\n"); 
			}
		}
		//check if array[0] is pwd
		else if(strcmp(string_array[0], "pwd") == 0)
		{
			char buf[257];
			char *ptr;
			ptr = getcwd(buf,257); 
			printf("%s\n", ptr); 
		}
		//check if array[0] is bgkill check the child_list for "A"ctive or "D"eleted
		//Delete if active else do nothing
		else if(strcmp(string_array[0], "bgkill") == 0){
			if(string_array[1] == NULL){
				printf("NO VALUE GIVEN\n"); 
			}			
			else{
			int number = atoi(string_array[1]);
				if((strcmp(child_list[number]->status,"R") == 0) && (strcmp(child_list[number]->delete_status, "A") == 0) ){ 
					strncpy(child_list[number]->delete_status,"D",20);
					kill(child_list[number]->cpid, 15); 
					strncpy(child_list[number]->status, "Terminated", 20); 
				}
				else{
					printf("can't terminate process is suspended or doesn't exist\n"); 
				}
			}
		}
		//iterate through the child_list and if the delete_status of the iteration is "A"ctive then print out 
		else if(strcmp(string_array[0], "bglist") == 0){
			for(int i = 0; i < 10; i++){
				if(strcmp(child_list[i]->delete_status, "A") == 0){
					printf("%d:", i); 
					printf("[%s]",child_list[i]->status); 
					printf("%s",child_list[i]->directory); 
					printf("%s\n",child_list[i]->process);
					
				}	

			}
		}	
		//check if stop and then suspend process and set the status to S for suspended
		//once a process is suspended SIGTERM cannot terminate it until it runs again
		else if(strcmp(string_array[0],"stop") == 0){
			if(string_array[1] == NULL){
				printf("NO VALUE GIVEN\n"); 
			}			
			else{
				int number = atoi(string_array[1]);
				if(strcmp(child_list[number]->status,"R") == 0){
					strncpy(child_list[number]->status, "S", 10);
					kill(child_list[number]->cpid, 19); 
				}

				else if(strcmp(child_list[number]->status,"S") == 0){
					printf("error: Process already suspended\n"); 
				}
			}
		}
		//check if start and then start the process and set the status to R for running 
		else if(strcmp(string_array[0], "start") == 0){
			if(string_array[1] == NULL){
				printf("NO VALUE GIVEN\n"); 
			}
			else{
				
				int number = atoi(string_array[1]);
				if(strcmp(child_list[number]->status, "S") == 0){ 
				strncpy(child_list[number]->status, "R", 10);
				kill(child_list[number]->cpid, 18); 	
				}
				else if(strcmp(child_list[number]->status, "R") == 0){
					printf("error: Process already running\n");
				}
			}
		}
		else{
			pid = fork(); 
			//if fork fails
			if(pid < 0)
			{
				printf("process id error has occured\n");	
			}
			//child process
			else if(pid == 0)
			{
		//when the bg command is found, a new array is made to be passed off the the execvp 
		//with the first element missing so it can run in the background
				if(strcmp(string_array[0] ,"bg") == 0)
				{
					 
					char **bg_array = malloc(16 * sizeof(*bg_array)); 
					for(int i = 0; i < 16; i++)
					{
						bg_array[i] = NULL; 
					}
					int index = 1; 
					while(string_array[index] != NULL)
					{
						bg_array[index-1] = malloc(MAX_LINE_LENGTH * sizeof(*bg_array[index-1]));
						strncpy(bg_array[index-1], string_array[index], strlen(string_array[index])+1);   
						index++; 
					}
					execvp(bg_array[0], bg_array); 
					printf("error didn't execvp\n"); 
					//exit so that child closes in case of execvp error 
					exit(0); 
					
				}  

		//overwrites current process and creates a new process with the same PID
				else{
				execvp(string_array[0], string_array);
				printf("error didn't execvp\n"); 
				//exit in case of execvp error 
				exit(0);
				} 
			} 	
		
			//parent process 
			else{
				//when bg is called the parent needs to keep track of child process information in the child_list array 
				if(strcmp(string_array[0] ,"bg") == 0){
					//create bg array to store background process info with the first element removed from string array 
					char **bg_array = malloc(16 * sizeof(*bg_array)); 
					for(int i = 0; i < 16; i++)
					{
						bg_array[i] = NULL; 
					}
					int index = 1; 
					while(string_array[index] != NULL)
					{
						bg_array[index-1] = malloc(MAX_LINE_LENGTH * sizeof(*bg_array[index-1]));
						strncpy(bg_array[index-1], string_array[index], strlen(string_array[index])+1);   
						index++; 
					}
					//check if there is an index in the child_list that is "D" so that it can replace that spot in the background processes 
					int child_index = 0; 
					while(strcmp(child_list[child_index]->delete_status,"A")==0){
						child_index++; 
					}
					strncpy(child_list[child_index]->process, bg_array[0], strlen(bg_array[0])+1);  
				
					child_list[child_index]->cpid = pid; 
					strncpy(child_list[child_index]->status, "R", 10);  
					strncpy(child_list[child_index]->delete_status, "A", 10);
					
					char buf[257];
					//get directory information 
					char *ptr = getcwd(buf, 257); 
					strncpy(child_list[child_index]->directory,ptr, strlen(ptr)+1); 
					
					
				}

				pid_t check_term = waitpid(-1,&status,WNOHANG);
				if(check_term > 1){

					int job_number; 
					for(int i = 0; i < 10; i++){
						if(child_list[i]->cpid == check_term){
							strncpy(child_list[i]->delete_status, "D", 20); 
							job_number = i; 
							printf("Background process %d has terminated", job_number);
						}
					}
					
				}

				free(cmd);
			
			}
			
		}	
	
	}
	return 0;
}

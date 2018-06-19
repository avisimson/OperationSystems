//Avi Simson 205789100
#include <stdio.h>
#include <malloc.h>
#include <wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#define INPUT_SIZE 256
#define SIZE 500
#define SUCCESS 1
#define BACKGROUND 1
#define FOREGROUND 2
#define EXIT 3
#define FAIL -1
#define REGULAR 5
#define INSIDESTRING 6
//global buffers for cd command.
char pwd[SIZE], cwd[SIZE];
/*
 * param str is a string
 * function gets string and clear its values.
 */
void ClearString(char str[INPUT_SIZE]) {
    for(int i = 0; i < INPUT_SIZE; i++) {
        str[i] = NULL;
    }
}
/**
* linkned list of struct type job, a job includes action, pid and a pointer to next job.
*/
typedef struct job {
    char *name;
    pid_t pid;
    struct job* next;
} job;
/*
 * function gets a job* and allocate memory to it and initialize as route.
 * param routJob - route of jobs list.
 * return NULL if allocation failed and route if allocation succeed.
 */
job* RouteJob(job* routeJob) {
    routeJob = (job*)malloc(sizeof(job));
    if (routeJob == NULL) {
        //Fail end program.
        printf("memory allocation for jobs failed \n");
        return NULL;
    }
    //allocate space for tha action
    routeJob->name = (char*)malloc(6 * sizeof(char));
    if (routeJob->name == NULL) {
        //Fail end program.
        printf("memory allocation for route job failed \n");
        return NULL;
    }
    //initialize route job.
    strcpy(routeJob->name, "route");
    routeJob->next = NULL;
    return routeJob;
}
// funtion get array from user for command and fill the array with user's input.
void userInput(char inputCommand[INPUT_SIZE]) {
    inputCommand[0] = 0;
    char dummy;
    printf("prompt> ");
    //get the arguments from the user
    scanf("%[^\n]s", inputCommand);
    scanf("%c", &dummy);
}
/*
 * param splitCommand is an array of words splited.
 * param inputCommand is the command itself.
 * function  splits command into words, and checks if its exit or background command.
 */
int SplitAndDefineCommand(char *splitCommand[INPUT_SIZE], char inputCommand[INPUT_SIZE]) {
    int wordsCounter = 0;
    int stringRunner = 0;
    int wordRunner = 0;
    int mode = REGULAR;
    char* pointer = inputCommand;
    char word[INPUT_SIZE];
    int length = strlen(inputCommand);
    while(pointer != NULL && stringRunner <= length) {
        if(pointer[stringRunner] == '"') {
            if(mode == REGULAR) { mode = INSIDESTRING;} //start string handling.
            else{mode = REGULAR;} //stop string handling.
            stringRunner++;
            continue;
        }
        if(mode == REGULAR) {
            if(isspace((int)pointer[stringRunner])) {
                stringRunner++;
                splitCommand[wordsCounter] = (char*)calloc(strlen(word) + 1, 1);
                strcpy(splitCommand[wordsCounter], word);
                wordRunner = 0;
                ClearString(word); //empty word to be able to enter next word.
                wordsCounter++;
            } else {
                word[wordRunner] = pointer[stringRunner];
                wordRunner++;
                stringRunner++;
            }
        } else { //mode is INSIDESTRING.
            word[wordRunner] = pointer[stringRunner];
            wordRunner++;
            stringRunner++;
        }
    }
    splitCommand[wordsCounter] = (char*)calloc(strlen(word) + 1, 1);
    strcpy(splitCommand[wordsCounter], word);
    ClearString(word); //empty word to be able to enter next word.
    splitCommand[wordsCounter + 1] = (char*)calloc(strlen(word) + 1, 1);
    splitCommand[wordsCounter + 1] = NULL; //to know where string of strings ends.
    //exit handling.
    if (0 == strcmp(splitCommand[0], "exit")) {
        printf("%d\n",getpid());
        return EXIT;
    }
    //delete & to background processes.
    if (strcmp(splitCommand[wordsCounter], "&") == 0) {
        splitCommand[wordsCounter] = NULL;
        return BACKGROUND;
    }
    return FOREGROUND;
}
/*
@param route is the head of the jobs list.
 function checks which background jobs ended and remove them from the linked list.
*/
void RemoveFinishedJobs(job* route) {
    int status;
    int previosCheck = 0;
    job* previous = route;
    while (route->next != NULL) {
        route = route->next;
        if(previosCheck > 0) { //make previous job the job before current.
            previous = previous->next;
        }
        //checks if job has finished, and remove it accordingly.
        pid_t pid = waitpid(route->pid, &status, WNOHANG);
        if ((pid == route->pid)) {
            previous->next = route->next;
            free(route->name);
            free(route);
        }
        previosCheck++;
    }
}
/**
 * param route is head of jobs linked list.
 * function deletes all jobs from heap memory in order to exit program.
 */
void FreeAllJobs(job* route)
{
    job* current = route;
    job* next;
    while (current != NULL) {
        next = current->next;
        free(current->name);
        free(current);
        current = next;
    }
}
/*
 * param route is head of jobs linked list.
 * function prints all background jobs.
 */
void PrintJobsList(job* route) {
    job* current = route->next;
    while(current != NULL) {
        printf("%d %s\n", current->pid, current->name);
        current = current->next;
    }
}
/* param splitCommand is the user cd command split to words.
 * function handles the sensitive cd command.
 */
int HandleCdCommand(char* splitCommand[INPUT_SIZE]) {
    char temp[SIZE];
    getcwd(temp, SIZE);
    //handle cd ~
    if((splitCommand[1] == NULL) ||
            ((strcmp(splitCommand[1], "~") == 0 && splitCommand[2] == NULL))){
        //to home directory.
        printf("%d\n",getpid());
        chdir(getenv("HOME"));
        strcpy(pwd, temp);
        getcwd(cwd, SIZE);
        return SUCCESS;
    } else if(strcmp(splitCommand[1], "-") == 0 && splitCommand[2] == 0) { //to previous directory
        printf("%d\n",getpid());
        chdir(pwd);
        strcpy(pwd, cwd);
        printf("%s \n", getcwd(cwd,SIZE));
        return SUCCESS;
    }
    char *cd = (char *)malloc(strlen(splitCommand[1]) + 1);
    if(cd == NULL) {return FAIL;} //memory allocation fail.
    //print pid
    printf("%d\n",getpid());
    //copy whats after cd
    strcpy(cd, splitCommand[1]);
    //cd operation.
    chdir(cd);
    //pwd is pervious path, cwd is current path.
    strcpy(pwd, temp);
    getcwd(cwd, SIZE);
    free(cd);
}
/*
 *param route is head of jobs linked list
 * param command is the name of the command.
 * param pid is the id number of command.
 * function adds new job to linked list.
 */
int AddToList(job* route, char* command, int pid) {
    job *current = route;
    //get to last command in list.
    while (current->next != NULL) {
        current = current->next;
    }
    //allocate memory for a new job and copy new job command to it.
    current->next = (job*)malloc(sizeof(job));
    if(current->next == NULL) { return FAIL;} //if allocation fail end program.
    command[strlen(command) - 1] = 0;
    current->next->name = (char*)malloc(sizeof(command)+ 1);
    if(current->next->name == NULL) { return FAIL;} //if allocation fail end program.
    strcpy(current->next->name, command);
    current->next->pid = pid;
    current->next->next = NULL;
    return SUCCESS;
}
int main() {
    //array of input from user.
    char inputCommand[INPUT_SIZE];
    char copy[INPUT_SIZE];
    //input command after word split.
    char* splitInput[INPUT_SIZE];
    job* routeJob;
    //initialize Head of job linked list and end program if fail to allocate memory.
    routeJob = RouteJob(routeJob);
    if(routeJob == NULL) { return 1;}
    while(1) {
        //get command input from user.
        userInput(inputCommand);
        strcpy(copy, inputCommand);
        //if user input is only enter so get back to start loop again.
        if(inputCommand[0] == 0) { continue;}
        //understand what the command is and split it to words.
        int stateOfCommand = SplitAndDefineCommand(splitInput, copy);
        //if user entered exit command, free all memory from heap and end program.
        if(stateOfCommand == EXIT) { FreeAllJobs(routeJob); return 1;}
        //check which jobs are finish and remove them.
        RemoveFinishedJobs(routeJob);
        //check if command is to show jobs.
        if ((strcmp(splitInput[0], "jobs") == 0) && splitInput[1] == NULL) {
            PrintJobsList(routeJob);
        } else if(strcmp(splitInput[0], "cd") == 0) { //cd special command handle.
            HandleCdCommand(splitInput);
        } else { //All other commands.
            pid_t pid = fork();
            if (pid == FAIL) { //check if could provide new proccess, if not print fail.
                fprintf(stderr, "Couldn't create new process\n");
            } else if (pid > 0) { //father process
                printf("%d\n", pid);
                if (stateOfCommand == BACKGROUND) { //add to list of jobs that work in the background
                    int addStatus = AddToList(routeJob, inputCommand, pid);
                    if(addStatus == FAIL) {FreeAllJobs(routeJob); return 1; } //if fail exit program.
                } else { //dont work on background, wait to son to finish.
                    wait(NULL);
                }
            } else if (pid == 0) { // son process
                char command[INPUT_SIZE] = "/bin/";
                //path to program.
                strcat(command, splitInput[0]);
                //execute the command.
                int execute = execv(command, splitInput);
                if (execute == FAIL) { //if command execution failed end program.
                    fprintf(stderr, "Error in system call\n");
                }
                return 0;
            }
        }
    }
}
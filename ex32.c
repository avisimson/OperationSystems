//Name : Avi Simson
//ID : 205789100
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#define ERRMSG "Error in system call"
#define ERRDIR "could not open working dir."
#define ERRCLS "couldnt do close. Error in system call"
#define TEMP_OUTPUT "output.txt"
#define LENGTH 480
const char* resultFile = "result.csv";
const char* errorFile =  "errors.txt";
//functions decleration.
void CFileFinder(char *path, char *pathToInsideDir, char *cName);
int CFileChecker(char* file);
void DirectoryHandler(char *dirName, char *pathToC,
                      char *outputPath, char *inputPath);
void writeToResult(char* dirName , char* stGrade, char* exp);
void Compiler(char* pathToC);
void Executer(char *inputPath, char* pathToC);
void CompExec(char* dirName, char* outputPath);

int main(int argc, char** argv) {
    int config,result,err;
    if(argc != 2) { //case of input of user to arguments is not as required.
        printf("Wrong number of arguments");
        return 0;
    }
    //create errors file and direct all error strings there.
    if ((err = open(errorFile, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0) {
        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG));
        exit(-1);
    }
    dup2(err, 2);
    close(err);
    const char* configurationPath = argv[1];
    if ((config = open(configurationPath, O_RDONLY)) < 0) {
        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG)); //case of error in opening conf file.
        exit(-1);
    }
    char buffer[LENGTH];
    //read configuration info into buffer
    if (read(config, buffer, LENGTH) < 0) {
        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG));
        exit(-1);
    }
    if(close(config) < 0) { write(STDERR_FILENO, ERRCLS, strlen(ERRCLS)); exit(-1);} //if error occurs.
    //create and open result.csv file.
    if ((result = open(resultFile, O_CREAT, S_IRUSR | S_IWUSR)) < 0) {
        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG)); //if error occurs.
        exit(-1);
    }
    if(close(result) < 0) { write(STDERR_FILENO, ERRCLS, strlen(ERRCLS)); exit(-1);} //if error occurs.
    //split the information from configuration to working, input and output paths.
    char* workingDir = strtok(buffer, "\n");
    char* inputPath = strtok(NULL, "\n");
    char* outputPath = strtok(NULL, "\n");
    //open the working directory.
    DIR *dir;
    struct dirent *d;
    if ((dir = opendir(workingDir)) == NULL) {
        write(STDERR_FILENO, ERRDIR, strlen(ERRDIR));
        exit(-1);
    }
    //path to dir.
    char path[LENGTH];
    //name of c file.
    char cName[LENGTH];
    //path to inside dir that has 'c' file.
    char pathToC[LENGTH];
    while ((d = readdir(dir)) != NULL) { //loop that checks all inside directories in our working dir.
        // skip one of this directories or files.
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;
        //put zeros in the buffers in every iteration.
        bzero(path, LENGTH);
        bzero(cName, LENGTH);
        bzero(pathToC, LENGTH);
        //set new path to inside dir
        snprintf(path, LENGTH, "%s/%s", workingDir, d->d_name);
        // search if c file exist.
        CFileFinder(path, pathToC, cName);
        //handle the dir with the c file.
        DirectoryHandler(d->d_name, pathToC, outputPath, inputPath);
    }
    //success in doing the work, now closing working directory and remove error and output files.
    if(closedir(dir) < 0|| unlink(errorFile) < 0) {
        write(STDERR_FILENO, ERRCLS, strlen(ERRCLS)); //if error occurs.
        exit(-1);
    }
    return 0;
}
/*
 * Function get path to inside directory, and creates path to c file and find c file if exist.
 * if c file exist its name will be in cName and path to him will be in pathToC
 * if c file doesnt exist the buffers will stay with zeros.
 */
void CFileFinder(char *path, char *pathToC, char *cName) {
    DIR *dir;
    struct dirent *d;
    char temPath[LENGTH];
    if ((dir = opendir(path)) == NULL) { //try to open directory we are searching in.
        return;
    }
    while ((d = readdir(dir)) != NULL) { //recursive loop to find if there is a c file.
        if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0) continue;
        if (d->d_type == DT_DIR) { //d is directory-recursion.
            snprintf(temPath, LENGTH, "%s/%s", path, d->d_name); //temPath becomes path to recursion.
            CFileFinder(temPath, pathToC, cName);
        } else { //d is regular file.
            if (CFileChecker(d->d_name) > 0) { //file is ".c"
                snprintf(cName, LENGTH, "%s", d->d_name); //put file name in cName.
                snprintf(pathToC, LENGTH, "%s/%s", path, d->d_name); //put path to file in pathToC.
            }
        }
    }
    closedir(dir);
}
//function checks if file ends with '.c'.
int CFileChecker(char* file) {
    char* pt = strrchr(file, '.'); //find last ocurrance of . in file.
    if(!pt) {return 0;} //if '.' isnt in string.
    if(strcmp(pt, ".c") == 0) {
        return 1;
    } else {
        return 0;
    }
}
void DirectoryHandler(char *dirName, char *pathToC,
                      char *outputPath, char *inputPath) {
    if (strcmp(pathToC, "") == 0) { //if cName is full of zeros-means empty-means no c file.
        writeToResult(dirName , "0", "NO_C_FILE");
    } else {
        pid_t pid = fork();
        if (pid == 0) { //new proccess that compiles the c file.
            Compiler(pathToC);
        } else if (pid < 0) { //Opening proccess failed.
            write(STDERR_FILENO, ERRMSG, strlen(ERRMSG));
            exit(-1);
        } else {
            int status = 0;
            waitpid(pid, &status, 0);
            //son exit with compilation error.
            if (WEXITSTATUS(status) != 0) {
                writeToResult(dirName , "0", "COMPILATION_ERROR");
            } else { //compilation success.
                pid_t pid = fork();
                if (pid == 0) { //new proccess that executes the c compiled file.
                    Executer(inputPath, pathToC);
                } else if (pid < 0) { //execution fail.
                    write(STDERR_FILENO, ERRMSG, strlen(ERRMSG));
                    exit(-1);
                } else {
                    sleep(5); //sleep 5 seconds to check if execution ended.
                    int status = 0;
                    pid_t wait_pid = waitpid(pid, &status, WNOHANG);
                    if(unlink("./a.out") < 0) {
                        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG));
                        exit(-1);
                    }
                    if (wait_pid == 0) { //execution still running.
                        kill(pid, SIGKILL);
                        writeToResult(dirName , "0", "TIMEOUT");
                    } else { //check if files outputs are similar/different/identical with comp.out.
                        CompExec(dirName, outputPath);
                    }
                }
            }
        }
    }
}
/*
 * function writes to result file the directory name, the grade of c file, and exp(explanation.)
 * */
void writeToResult(char* dirName , char* stGrade, char* exp) {
    char msg[LENGTH];
    bzero(msg, LENGTH);
    //get structure of message to write in result.
    snprintf(msg, LENGTH, "%s,%s,%s\n", dirName, stGrade, exp);
    int fd;
    if ((fd = open(resultFile, O_RDWR | O_APPEND, S_IRUSR | S_IWUSR)) < 0) {
        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG)); //if failure occurs.
        exit(-1);
    }
    //write the output in result file at the end.
    if (write(fd, msg, strlen(msg)) < 0) {
        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG)); //if failure occurs.
        exit(-1);
    }
    close(fd);
}
//function compiles the cName file into executive a.out file.
void Compiler(char* pathToC) {
    // compiling args.
    char *compileOrders[] = { "gcc", pathToC, NULL};
    if(execvp("gcc", compileOrders) < 0) {
        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG));
        exit(-1);
    }
}
//function executes the c file inside inputPath.
void Executer(char *inputPath, char* pathToC) {
    int fd;
    //set the output indirection.
    char* args[] = {"./a.out", pathToC, NULL};
    if ((fd = open(TEMP_OUTPUT, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR)) < 0) {
        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG));
        exit(-1);
    }
    dup2(fd, 1);
    close(fd);
    //set the input indirection.
    if ((fd = open(inputPath, O_RDONLY)) < 0) {
        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG));
        exit(-1);
    }
    dup2(fd, 0);
    close(fd);
    //execute a.out.
    execvp("./a.out",args);
}
/*
 * function runs comp.out file and checks if output file of execution and for output are identical,similar,differ.
 * write the result to results file.
 * parameters-dirName-the directory of comp.out, outputPath-the path to output that needs to be.
 */
void CompExec(char* dirName, char* outputPath) {
    int status = 0;
    pid_t pid = fork();
    if (pid == 0) { //son proccess.
        char* args[] = {"./comp.out", TEMP_OUTPUT, outputPath, NULL};
        if (execvp("./comp.out", args) < 0) {
            write(STDERR_FILENO, ERRMSG, strlen(ERRMSG)); //if an error occurs,
            exit(-1);
        }
        //check if outputs are identical/similar/differ.
    } else if (pid < 0) {
        write(STDERR_FILENO, ERRMSG, strlen(ERRMSG)); //if an error occurs,
        exit(-1);
    } else {
        waitpid(pid, &status, 0); //wait for son proccess.
        if (unlink(TEMP_OUTPUT) < 0) {
            write(STDERR_FILENO, ERRMSG, strlen(ERRMSG)); //if an error occurs,
            exit(-1);
        }
        switch(WEXITSTATUS(status)) {
            case 1: {
                writeToResult(dirName , "60", "BAD_OUTPUT");
                break;
            }
            case 2: {
                writeToResult(dirName , "80", "SIMILAR_OUTPUT");
                break;
            }
            case 3: {
                writeToResult(dirName , "100", "GREAT_JOB");
                break;
            }
            default: {
                break;
            }
        }
    }
}
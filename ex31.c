// Name: Avi Simson, ID: 205789100, FILE:ex31.c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#define SIZE 15
#define EOFSIZE 10000
#define IDENTICAL 3
#define SIMILAR 2
#define DIFFERENT 1
#define FAILURE -1
#define OPENERROR "Error: program couldn't open files correctly."
#define READERROR "Error: program couldn't read from files correctly."
#define CLOSEERROR "Error: program couldn't close file "
/*
 * param a,b - chars.
 * return - 1 if a,b are similar in a-z A-Z or return 0 if not.
 */
int InsideAlphabet(char a, char b) {
    if((a <= 'z' && a >= 'a') || (a <= 'Z' && a >= 'A')) {
        if((b <= 'z' && b >= 'a') || (b <= 'Z' && b >= 'A')) {
            int check = a - b;
            if(check == 32 || check == -32) {
                return 1;
            }
        }
    }
    return 0;
}
/*
 * check buffers are identical, from what left to read after checkSimilarity method.
 * param buffer1, buffer2 - buffers of info from files.
 * param place1, place2 - places in buffers we need to continue reading from.
 * param result- current result status.
 * return result status.
 */
int EndedTogether(char buffer1[SIZE], char buffer2[SIZE], int place1, int place2, int result) {
    int i;
    if(place1 < SIZE || place2 < SIZE) { //files are not identical.
        result = SIMILAR;
        if(place1 < SIZE) {
            while(place1 < SIZE) {
                i = place1;
                if(((int)buffer1[i]) == ' ' || ((int)buffer1[i]) == '\n' || ((int)buffer1[i]) == '\0') {
                    result = DIFFERENT;
                    break;
                }
                place1++;
            }
        } else { //checkerArray[2] < SIZE
            while(place2 < SIZE) {
                i = place2;
                if(((int)buffer2[i]) == ' ' || ((int)buffer2[i]) == '\n' || ((int)buffer2[i]) == '\0') {
                    result = DIFFERENT;
                    break;
                }
                place2++;
            }
        }
    }
    return result;
}
/*
 * function checks if buffer are identical,similar or different.
 * param buffer1, buffer2 - buffers of informations from files 1-2.
 * param currentResult - current status of return type of program SIMILAR.
 * resultArray- { result status-similar,different, count1,count2}
 * returns - array in size of 3 [type of status, placement finish of buffer1, placement finish of buffer2].
 */
int* CheckSimilarity(char buffer1[SIZE], char buffer2[SIZE], int currentResult, int resultArray[3]) {
    int flag = currentResult;
    int count1 = 0, count2 = 0, checker = 0;
    while(count1 < SIZE && count2 < SIZE) {
        if(buffer1[count1] == buffer2[count2]) { //identical char
            count1++; count2++;
        } else if(InsideAlphabet(buffer1[count1], buffer2[count2])) { //similar in a-z,A-Z
            count1++; count2++;
        }
        else {
            if(((int)buffer1[count1]) == ' ' || ((int)buffer1[count1]) == '\n' || ((int)buffer1[count1]) == '\0') {
                count1++;
                checker++;
            }
            if(((int)buffer2[count2]) == ' ' || ((int)buffer2[count2]) == '\n' || ((int)buffer2[count2]) == '\0') {
                count2++;
                checker++;
            }
            if(checker == 0) { //if both places in buffer are different and not space or \n.
                resultArray[0] = DIFFERENT;
                return resultArray;
            }
            checker = 0;
        }
    }
    resultArray[0] = flag;
    resultArray[1] = count1;
    resultArray[2] = count2;
    return resultArray;
}
/*
 * function clears buffer and put all cells we didnt touch in the beggining.
 * param buffer - full buffer of chars.
 * param place - where we stopped reading from buffer.
 */
void ClearBuffer(char buffer[SIZE], int place) {
    int i;
    if(place == 0) {
        for(i = 0; i < SIZE; i++) {
            buffer[i] = '\0';
        }
        return;
    }
    for(i = place; i < SIZE; i++) {
        buffer[i - place] = buffer[i];
    }
    for(i = SIZE - place; i < SIZE; i++) {
        buffer[i] = '\0';
    }
}
/*
 * param file - symbol of file.
 * param numOfFile - number of file 1/2.
 * return - 1 if close succeed 0 if close failed.
 */
int CloseFile(int file, int numOfFile) {
    close(file);
    if(file < 0) { write(2, CLOSEERROR + numOfFile, strlen(CLOSEERROR) + 2); return 0;}
    return 1;
}
/*
 * param argc - number of arguments to program,when the 1st is name of program.
 * param argv - array of paths to 2 files
 * program gets 2 files and returns 3-identical,2-similar,1-different.
 */
int main(int argc, char** argv) {
    if(argc != 3) {printf("Error: wrong number of arguments. \n"); return 0;} //check if num of args is good.
    char endBuffer[EOFSIZE];
    char buffer1[SIZE], buffer2[SIZE]; //buffers to hold info from files.
    int result = IDENTICAL;
    //open 2 files for reading only.
    int fileOne = open(argv[1], O_RDONLY);
    int fileTwo = open(argv[2], O_RDONLY);
    if(fileOne < 0 || fileTwo < 0) { //program couldn't open files correctly.
        write(2, OPENERROR, strlen(OPENERROR) + 1); //write error message to stderr.
        return FAILURE;
    }
    int reader1, reader2;
    int* checkerArray;
    //check if files are identical or not.
    while(1) {
        reader1 = read(fileOne, buffer1, SIZE - 1); //read info from fileOne.
        reader2 = read(fileTwo, buffer2, SIZE - 1); //read info from fileTwo.
        if(reader1 < 0 || reader2 < 0) { //check if fail reading.
            write(2, READERROR, strlen(READERROR)); //write error message to stderr.
            return FAILURE;
        }
        if(strcmp(buffer1, buffer2) != 0) { //check if they are not identical.
            result = SIMILAR;
            break;
        }
        if(reader1 == 0 && reader2 == 0) {
            if(!CloseFile(fileOne, 1) || !CloseFile(fileTwo, 2)) {return FAILURE;} //close one of files failed.
            return result; //IDENTICAL.
        }
    }
    int initialArray[3] = {0, 0, 0};
    checkerArray = initialArray;
    reader1 = 1; reader2 = 1;
    int flag = 0;
    //check if similar/different.
    while(reader1 > 0 && reader2 > 0) {
        if(flag > 0) {
            reader1 = read(fileOne, buffer1 + SIZE - checkerArray[1], checkerArray[1]); //read info from fileOne.
            reader2 = read(fileTwo, buffer2 + SIZE - checkerArray[2], checkerArray[2]); //read info from fileTwo/
        }
        flag++;
        if(reader1 < 0 || reader2 < 0) { //check if fail reading.
            write(2, READERROR, strlen(READERROR)); //write error message to stderr.
            return FAILURE;
        }
        checkerArray = CheckSimilarity(buffer1, buffer2, result, checkerArray);
        result = checkerArray[0];
        if(result == DIFFERENT) { //files are different.
            if(!CloseFile(fileOne, 1) || !CloseFile(fileTwo, 2)) {return FAILURE;} //close one of files failed.
            return DIFFERENT;
        }
        ClearBuffer(buffer1, checkerArray[1]); //clear buffer for file one to next read.
        ClearBuffer(buffer2, checkerArray[2]); //clear buffer for file two to next read.
    }
    //if ended reading files together.
    if(reader1 == 0 && reader2 == 0) {
        result = EndedTogether(buffer1, buffer2, checkerArray[1], checkerArray[2], result);
    } else if(reader1 == 0) {//reader2 != 0.
        while(reader2 > 0) {
            reader2 = read(fileTwo, buffer2 + SIZE - checkerArray[2], checkerArray[2]); //read info from fileTwo.
            if(reader2 < 0) { //check if fail reading.
                write(2, READERROR, strlen(READERROR)); //write error message to stderr.
                return FAILURE;
            }
            checkerArray = CheckSimilarity(buffer1, buffer2, result, checkerArray);
            result = checkerArray[0];
            if(result == DIFFERENT) { //files are different.
                break;
            }
            ClearBuffer(buffer1, checkerArray[1]); //clear buffer for file one to next read.
            ClearBuffer(buffer2, checkerArray[2]); //clear buffer for file two to next read.
        }
    } else { //reader1 != 0 && reader2 == 0.
        while(reader2 > 0) {
            reader1 = read(fileTwo, buffer1 + SIZE - checkerArray[1], checkerArray[1]); //read info from fileTwo.
            if(reader1 < 0) { //check if fail reading.
                write(2, READERROR, strlen(READERROR)); //write error message to stderr.
                return FAILURE;
            }
            checkerArray = CheckSimilarity(buffer1, buffer2, result, checkerArray);
            result = checkerArray[0];
            if(result == DIFFERENT) { //files are different.
                break;
            }
            ClearBuffer(buffer1, checkerArray[1]); //clear buffer for file one to next read.
            ClearBuffer(buffer2, checkerArray[2]); //clear buffer for file two to next read.
        }
    }
    if(!CloseFile(fileOne, 1) || !CloseFile(fileTwo, 2)) {return FAILURE;}
    return result;
}

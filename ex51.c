//Name: Avi Simson id: 205789100
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

#define STDERR_FD 2
#define TETRIS_PROG "./draw.out"
#define FLIP 'w'
#define LEFT 'a'
#define RIGHT 'd'
#define DOWN 's'
#define QUIT 'q'

/*
 *listener to keyboard.
*/
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

/*
 * prints error in sys call to stderr.
 */
void printErrorInSysCallToSTDERR() {
    char error[] = "Error in system call\n";
    write(STDERR_FD, error, sizeof(error));
}
/*
 * check what the char user enter
 * param ch is the char that user enter
 * return 1 if get Q for exit and 0 else.
 */
int isGameKey(char ch) {
    switch(ch) {
        case RIGHT:
        case LEFT:
        case DOWN:
        case FLIP:
        case QUIT:
            return 1;
        default:
            return 0;
    }
}

int main() {
    int Pipe[2];
    pipe(Pipe);
    int pid;

    if ((pid = fork()) < 0) {
        printErrorInSysCallToSTDERR();
    }
    //child process.
    if (pid == 0) {
        printf("New process in town\n");
        // force our stdin to be the read size of the pipe we made
        dup2(Pipe[0], 0);
        //execute teris ex52.c
        execlp(TETRIS_PROG, TETRIS_PROG, NULL);
        // Gets here only if exelp failed.
        printErrorInSysCallToSTDERR();
    }

    //father process.
    char ch;
    while (1) {
        //check input char from user.
        ch = getch();
        // char from stdin is not a game key.
        if (!isGameKey(ch)) {
            continue;
        }
        // write the game key to the pipe
        if(write(Pipe[1], &ch, 1) < 0) {
            printErrorInSysCallToSTDERR();
        }
        //kill the process
        kill(pid, SIGUSR2);
        //check if ch is QUIT
        if (ch == QUIT) {
            break;
        }
    }

    return 0;
}
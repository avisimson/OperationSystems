//Name: Avi Simson id: 205789100
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define SIZE 20

//status of player
typedef enum {
    HORIZONTAL, VERTICAL
} status;
// enum of the motion command
typedef enum {
    LEFT, RIGHT, DOWN, FLIP
} movement;

// player
typedef struct Player {
    status stat;
    int x;
    int y;
} Player;

// global variables.
char gameBoard[SIZE][SIZE];
Player globalPlayer;
/*
 * clear gameBoard from player.
 */
void clearBoard() {
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (gameBoard[i][j] == '-') {
                gameBoard[i][j] = ' ';
            }
        }
    }
}
/**
 * initialize borders of board.
 */
void initializeBoard() {
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if ((j == 0) || (j == SIZE - 1)) {
                gameBoard[i][j] = '*';
            } else if (i == SIZE - 1) {
                gameBoard[i][j] = '*';
            } else {
                gameBoard[i][j] = ' ';
            }
        }
    }
}

/*
 * print board to output
 */
void printBoard() {
    system("clear");
    int i, j;
    for (i = 0; i < SIZE; i++) {
        for (j = 0; j < SIZE; j++) {
            if (globalPlayer.stat == HORIZONTAL) { //case of horizontal player
                if (i == globalPlayer.x) {
                    if (j == globalPlayer.y - 1 || j == globalPlayer.y) {
                        printf("-");
                    } else if (j == globalPlayer.y + 1) {
                        printf("-");
                        // if the board square doesn't contain a player
                    } else {
                        printf("%c", gameBoard[i][j]);
                    }
                } else {
                    printf("%c", gameBoard[i][j]);
                }
            } else if (globalPlayer.stat == VERTICAL) { //case of vertical player.
                if (j == globalPlayer.y) {
                    if (i == globalPlayer.x - 1 || i == globalPlayer.x) {
                        printf("-");
                    } else if (i == globalPlayer.x + 1) {
                        printf("-");
                    } else {
                        printf("%c", gameBoard[i][j]);
                    }
                    // if the board square doesn't contain a player
                } else {
                    printf("%c", gameBoard[i][j]);
                }
            }
            // if we reached the end of the line
            if (j == SIZE - 1) {
                printf("\n");
            }
        }
    }
}
/**
 *function updates the player's spot in board.
 * @param input is enum of movement for player.
 */
void updatePlayer(movement input) {
    switch (input) {
        case DOWN:
            if (globalPlayer.stat == HORIZONTAL) {
                if (globalPlayer.x == SIZE - 2) {
                    globalPlayer.x = 0;
                    globalPlayer.y = SIZE / 2;
                } else {
                    globalPlayer.x += 1;
                }
            } else {
                if (globalPlayer.x + 1 == SIZE - 2) {
                    globalPlayer.stat = HORIZONTAL;
                    globalPlayer.x = 0;
                    globalPlayer.y = SIZE / 2;
                } else {
                    globalPlayer.x += 1;
                }
            }
            break;
        case LEFT:
            if (globalPlayer.stat == HORIZONTAL) {
                if (globalPlayer.y - 1 != 1) {
                    globalPlayer.y -= 1;
                }
            } else {
                if (globalPlayer.y != 1) {
                    globalPlayer.y -= 1;
                }
            }
            break;
        case RIGHT:
            if (globalPlayer.stat == HORIZONTAL) {
                if (globalPlayer.y + 1 != SIZE - 2) {
                    globalPlayer.y += 1;
                }
            } else {
                if (globalPlayer.y != SIZE - 2) {
                    globalPlayer.y += 1;
                }
            }
            break;
        case FLIP:
            if (globalPlayer.stat == HORIZONTAL) {
                if (globalPlayer.x == 0 || globalPlayer.x >= SIZE - 2) { break; }
                globalPlayer.stat = VERTICAL;
            } else {
                if (globalPlayer.y <= 1 || globalPlayer.y >= SIZE - 2) { break; }
                globalPlayer.stat = HORIZONTAL;
            }
            break;
    }
    printBoard();
}
/*
 * function is activated everytime time SIGUSR2 has been sent to this process.
 */
void signalKey() {
    signal(SIGUSR2, signalKey);
    // read from pipe what sent
    char input = (char) getchar();
    // change the player's spot in cases a,d,s,w or exit program if q
    switch (input) {
        case 's':
            updatePlayer(DOWN);
            break;
        case 'w':
            updatePlayer(FLIP);
            break;
        case 'd':
            updatePlayer(RIGHT);
            break;
        case 'a':
            updatePlayer(LEFT);
            break;
        case 'q':
            exit(0);
        default:
            break;
    }
    printBoard(); //print gameBoard after the change.
}

/*
 * function is activated everytime sigalrm is being sent to this proccess.
 */
void alarmPlayer() {
    signal(SIGALRM, alarmPlayer);
    updatePlayer(DOWN);
    alarm(1);
}

/*
 * initialize board, player and signals and change the player's spot after every signal.
 */
int main() {
    signal(SIGUSR2, signalKey);
    signal(SIGALRM, alarmPlayer);
    globalPlayer.x = 0;
    globalPlayer.y = SIZE / 2;
    globalPlayer.stat = HORIZONTAL;
    initializeBoard();
    printBoard();
    clearBoard();
    alarm(1);
    while (1) {
        pause();
    }
}
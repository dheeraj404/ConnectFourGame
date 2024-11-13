#include <sys/types.h>
#include <errno.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 4444

using namespace std;
extern int errno;

struct infoPlayer {
    char playerSymbol;
    int socket;
};

const int width = 7, height = 6;
char board[height][width];

// Function to initialize the game board
void initializeBoard(char board[][width]) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            board[i][j] = ' ';
        }
    }
}

// Function to handle a player's turn
int playerTurn(char board[][width], infoPlayer activePlayer) {
    int selectedColumn;
    write(activePlayer.socket, "r", 1); // Indicate it's the player's turn
    while (true) {
        char received[100];
        bzero(received, sizeof(received));
        if (read(activePlayer.socket, received, sizeof(received)) <= 0) {
            perror("[server] Error reading from client.\n");
            close(activePlayer.socket);
            exit(1);
        }
        selectedColumn = atoi(received) - 1; // Adjust for 0-based indexing

        if (selectedColumn < 0 || selectedColumn >= width) {
            write(activePlayer.socket, "e", 1); // Invalid column
            continue;
        }

        if (board[0][selectedColumn] != ' ') {
            write(activePlayer.socket, "f", 1); // Column is full
            continue;
        }

        break; // Valid move received
    }

    return selectedColumn;
}

// Function to place a player's symbol on the board
void makeMove(char board[][width], infoPlayer activePlayer, int selectedColumn) {
    int row = height - 1; // Start from the bottom row
    while (row >= 0) {
        if (board[row][selectedColumn] == ' ') {
            board[row][selectedColumn] = activePlayer.playerSymbol; // Place the symbol
            break; // Exit the loop after placing the symbol
        }
        row--; // Move up one row
    }
}

// Function to send the updated board to both players
void displayBoard(char board[][width], infoPlayer player1, infoPlayer player2) {
    char buffer[2024];
    bzero(buffer, sizeof(buffer));

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (board[i][j] == ' ') {
                strcat(buffer, "0");
            } else if (board[i][j] == 'X') {
                strcat(buffer, "1");
            } else if (board[i][j] == 'O') {
                strcat(buffer, "2");
            }
        }
    }

    size_t bufferSize = strlen(buffer);

    // Send to player 1
    write(player1.socket, "t", 1); // Indicate board update
    write(player1.socket, &bufferSize, sizeof(bufferSize));
    if (write(player1.socket, buffer, bufferSize) <= 0) {
        perror("[server] Error writing to client 1.\n");
    }

    // Send to player 2
    write(player2.socket, "t", 1); // Indicate board update
    write(player2.socket, &bufferSize, sizeof(bufferSize));
    if (write(player2.socket, buffer, bufferSize) <= 0) {
        perror("[server] Error writing to client 2.\n");
    }
}

// Function to check if a player has won
int checkWin(char board[][width], infoPlayer activePlayer) {
    char symbol = activePlayer.playerSymbol;

    // Check horizontal, vertical, and diagonal connections
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (board[i][j] == symbol) {
                // Check horizontal
                if (j <= width - 4 &&
                    board[i][j + 1] == symbol &&
                    board[i][j + 2] == symbol &&
                    board[i][j + 3] == symbol)
                    return 1;

                // Check vertical
                if (i <= height - 4 &&
                    board[i + 1][j] == symbol &&
                    board[i + 2][j] == symbol &&
                    board[i + 3][j] == symbol)
                    return 1;

                // Check diagonal /
                if (i >= 3 && j <= width - 4 &&
                    board[i - 1][j + 1] == symbol &&
                    board[i - 2][j + 2] == symbol &&
                    board[i - 3][j + 3] == symbol)
                    return 1;

                // Check diagonal 
                if (i <= height - 4 && j <= width - 4 &&
                    board[i + 1][j + 1] == symbol &&
                    board[i + 2][j + 2] == symbol &&
                    board[i + 3][j + 3] == symbol)
                    return 1;
            }
        }
    }

    return 0;
}

// Function to check if the board is full
int isBoardFull(char board[][width]) {
    for (int i = 0; i < width; ++i) {
        if (board[0][i] == ' ')
            return 0; // Found an empty spot
    }
    return 1; // Board is full
}

// Function to declare the winner
void declareWinner(infoPlayer winner, infoPlayer loser) {
    write(winner.socket, "w", 1); // Winner message
    write(loser.socket, "l", 1);  // Loser message
}

// Function to declare a draw
void declareDraw(infoPlayer player1, infoPlayer player2) {
    write(player1.socket, "d", 1); // Draw message
    write(player2.socket, "d", 1); // Draw message
}

int main() {
    struct sockaddr_in server;
    struct sockaddr_in from;
    int sd;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[server] Error at socket().\n");
        return errno;
    }

    bzero(&server, sizeof(server));
    bzero(&from, sizeof(from));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1) {
        perror("[server] Error at bind().\n");
        return errno;
    }

    if (listen(sd, 5) == -1) {
        perror("[server] Error at listen().\n");
        return errno;
    }

    printf("[server] Waiting at port %d...\n", PORT);
    fflush(stdout);

    while (1) {
        int player1_socket, player2_socket;
        unsigned int length = sizeof(from);

        player1_socket = accept(sd, (struct sockaddr*)&from, &length);
        if (player1_socket < 0) {
            perror("[server] Error accepting client 1.\n");
            continue;
        }
        printf("[server] Player 1 connected.\n");
        fflush(stdout);

        player2_socket = accept(sd, (struct sockaddr*)&from, &length);
        if (player2_socket < 0) {
            perror("[server] Error accepting client 2.\n");
            close(player1_socket);
            continue;
        }
        printf("[server] Player 2 connected.\n");
        fflush(stdout);

        int pid;
        if ((pid = fork()) == -1) {
            close(player1_socket);
            close(player2_socket);
            continue;
        } else if (pid > 0) {
            close(player1_socket);
            close(player2_socket);
            while (waitpid(-1, NULL, WNOHANG) > 0);
            continue;
        } else if (pid == 0) {
            // Child process
            close(sd);

            // Assign symbols to players
            infoPlayer player1, player2;
            player1.playerSymbol = 'X';
            player1.socket = player1_socket;
            player2.playerSymbol = 'O';
            player2.socket = player2_socket;

            // Notify players of their symbols
            if (write(player1.socket, "1", 1) <= 0) {
                perror("[server] Error writing to player 1.\n");
                close(player1.socket);
                close(player2.socket);
                exit(1);
            }
            if (write(player2.socket, "2", 1) <= 0) {
                perror("[server] Error writing to player 2.\n");
                close(player1.socket);
                close(player2.socket);
                exit(1);
            }

            printf("[server] Starting game between player 1 and player 2.\n");
            fflush(stdout);

            initializeBoard(board);

            int gameOver = 0;
            while (!gameOver) {
                // Player 1's turn
                int selectedColumn = playerTurn(board, player1);
                makeMove(board, player1, selectedColumn);
                displayBoard(board, player1, player2);

                if (checkWin(board, player1)) {
                    declareWinner(player1, player2);
                    gameOver = 1;
                    break;
                }
                if (isBoardFull(board)) {
                    declareDraw(player1, player2);
                    gameOver = 1;
                    break;
                }

                // Player 2's turn
                selectedColumn = playerTurn(board, player2);
                makeMove(board, player2, selectedColumn);
                displayBoard(board, player1, player2);

                if (checkWin(board, player2)) {
                    declareWinner(player2, player1);
                    gameOver = 1;
                    break;
                }
                if (isBoardFull(board)) {
                    declareDraw(player1, player2);
                    gameOver = 1;
                    break;
                }
            }

            printf("[server] Game over.\n");
            fflush(stdout);

            close(player1.socket);
            close(player2.socket);
            exit(0);
        }
    }
}

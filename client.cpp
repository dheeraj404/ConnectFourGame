#include <sys/types.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
using namespace std;
extern int errno;

int port;

// Function to clear the console screen
void clearScreen() {
    // Check for Windows or Unix-based systems
#ifdef _WIN32
    system("CLS");
#else
    system("clear");
#endif
}

// Function to display the game board
void displayBoard(char* buffer) {
    int index = 0;
    const int height = 6;
    const int width = 7;
    clearScreen();

    // Display column numbers at the top
    cout << "   ";
    for (int j = 1; j <= width; j++) {
        cout << j << "   ";
    }
    cout << endl;

    // Display the board
    for (int i = 0; i < height; i++) {
        cout << " |";
        for (int j = 0; j < width; j++) {
            if (buffer[index] == '0') {
                cout << "   |";
            } else if (buffer[index] == '1') {
                cout << " \033[1;32mX\033[0m |";
            } else if (buffer[index] == '2') {
                cout << " \033[1;31mO\033[0m |";
            }
            index++;
        }
        cout << endl;
        // Print separator line
        cout << " +";
        for (int k = 0; k < width; k++) {
            cout << "---+";
        }
        cout << endl;
    }
}

int main(int argc, char* argv[]) {
    int sd;
    struct sockaddr_in server;
    char received[2048];
    char playerSymbol;

    if (argc != 3) {
        printf("[-] Syntax: %s <server_address> <port>\n", argv[0]);
        return -1;
    }

    port = atoi(argv[2]);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[-] Error at socket().\n");
        return errno;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(port);

    if (connect(sd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1) {
        perror("[-] Error at connect().\n");
        return errno;
    }

    cout << "\033[1;34mWelcome to Connect Four!\033[0m\n";
    cout << "Waiting for another player to join...\n";
    fflush(stdout);

    if (read(sd, received, 1) <= 0) {
        perror("[-] Error reading from server.\n");
        return errno;
    }

    if (received[0] == '1') {
        printf("[+] You are player number 1!\n");
        cout << "Your symbol is \033[1;32mX\033[0m (Green). Good luck!\n";
        playerSymbol = 'X';
    } else if (received[0] == '2') {
        printf("[+] You are player number 2!\n");
        cout << "Your symbol is \033[1;31mO\033[0m (Red). Good luck!\n";
        playerSymbol = 'O';
    } else {
        printf("[-] Unknown player number received: %c\n", received[0]);
        return -1;
    }
    fflush(stdout);

    bool gameOver = false;
    while (!gameOver) {
        bzero(received, sizeof(received));

        if (read(sd, received, 1) <= 0) {
            perror("[-] Error reading from server.\n");
            return errno;
        }

        if (received[0] == 'w') {
            cout << "\033[1;32m[+] You win!\033[0m\n";
            gameOver = true;
        } else if (received[0] == 'l') {
            cout << "\033[1;31m[-] You lost!\033[0m\n";
            gameOver = true;
        } else if (received[0] == 'd') {
            cout << "\033[1;33m[=] It's a draw!\033[0m\n";
            gameOver = true;
        } else if (received[0] == 'r') {
            // Your turn to play
            cout << "\033[1;36m[*] It's your turn. Enter column number (1-7): \033[0m";
            fflush(stdout);
            char input[100];
            bzero(input, sizeof(input));
            read(0, input, sizeof(input));
            if (write(sd, input, strlen(input)) <= 0) {
                perror("[-] Error writing to server.\n");
                return errno;
            }
        } else if (received[0] == 'e') {
            cout << "\033[1;31m[-] Invalid column number. Please try again.\033[0m\n";
            cout << "\033[1;36m[*] Enter column number (1-7): \033[0m";
            fflush(stdout);
            char input[100];
            bzero(input, sizeof(input));
            read(0, input, sizeof(input));
            if (write(sd, input, strlen(input)) <= 0) {
                perror("[-] Error writing to server.\n");
                return errno;
            }
        } else if (received[0] == 'f') {
            cout << "\033[1;31m[-] Column is full. Please choose a different column.\033[0m\n";
            cout << "\033[1;36m[*] Enter column number (1-7): \033[0m";
            fflush(stdout);
            char input[100];
            bzero(input, sizeof(input));
            read(0, input, sizeof(input));
            if (write(sd, input, strlen(input)) <= 0) {
                perror("[-] Error writing to server.\n");
                return errno;
            }
        } else if (received[0] == 't') {
            // Receive board update
            size_t bufferSize;
            if (read(sd, &bufferSize, sizeof(bufferSize)) <= 0) {
                perror("[-] Error reading buffer size from server.\n");
                return errno;
            }

            char* boardData = new char[bufferSize + 1];
            size_t totalRead = 0;
            while (totalRead < bufferSize) {
                ssize_t bytesRead = read(sd, boardData + totalRead, bufferSize - totalRead);
                if (bytesRead <= 0) {
                    perror("[-] Error reading board data from server.\n");
                    delete[] boardData;
                    return errno;
                }
                totalRead += bytesRead;
            }
            boardData[bufferSize] = '\0'; // Null-terminate the string

            displayBoard(boardData);

            delete[] boardData; // Free allocated memory
        }
    }

    close(sd);
    return 0;
}

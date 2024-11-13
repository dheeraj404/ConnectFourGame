# Connect Four Game using TCP/IP Networking

A networked Connect Four game implemented in C++, allowing multiple pairs of players to play simultaneously over TCP/IP connections.

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Gameplay Instructions](#gameplay-instructions)
- [Running the Game on Different Devices](#running-the-game-on-different-devices)
- [Understanding TCP/IP Usage](#understanding-tcpip-usage)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

## Introduction

This project is a console-based Connect Four game that supports multiple pairs of players over TCP/IP networking. Players can be on the same machine or on different devices connected over a network. The server can handle any number of players (in even numbers), pairing them up for separate game sessions.

## Features

- **Multiple Game Sessions:** Supports multiple pairs of players playing simultaneously.
- **TCP/IP Networking:** Utilizes TCP/IP protocols for reliable network communication.
- **Cross-Platform Support:** Compatible with Linux, macOS, and Windows.
- **Colorful Game Board:** Visually appealing game board with colored game pieces.
- **User-Friendly Interface:** Clear instructions and prompts make the game easy to play.

## Requirements

- **C++ Compiler:** `g++` or any C++11 compatible compiler.
- **Terminal with ANSI Color Support:** To display colored game pieces.
- **Network Connectivity:** Required for client-server communication over TCP/IP.
- **Git:** For cloning the repository (optional).

## Installation

### Clone the Repository

```bash
git clone https://github.com/yourusername/ConnectFourGame.git
cd ConnectFourGame
```

### Compile the Code

**For Linux/macOS:**

- **Server:**

  ```bash
  g++ -o server server.cpp
  ```

- **Client:**

  ```bash
  g++ -o client client.cpp
  ```

**For Windows:**

- Use MinGW or Visual Studio to compile the code.
- Example with MinGW:

  ```bash
  g++ -o server.exe server.cpp
  g++ -o client.exe client.cpp
  ```

## Usage

### Running the Server

On the server machine, run:

```bash
./server
```

or on Windows:

```bash
server.exe
```

The server will start listening on port `4444` for incoming TCP/IP connections.

### Running the Clients

On client machines, run:

```bash
./client <server_ip_address> 4444
```

or on Windows:

```bash
client.exe <server_ip_address> 4444
```

Replace `<server_ip_address>` with the actual IP address of the server machine.

**Example:**

```bash
./client 192.168.1.100 4444
```

## Gameplay Instructions

- **Objective:** Connect four of your symbols horizontally, vertically, or diagonally before your opponent.
- **Symbols:**
  - Player 1: `X` (Green)
  - Player 2: `O` (Red)
- **How to Play:**
  - Players take turns entering a column number (1-7) to drop their piece into the game board.
  - The piece will occupy the lowest available space within the selected column.
- **Multiple Game Sessions:**
  - The server pairs up players as they connect.
  - Multiple games can be played simultaneously by different pairs of players.
- **Winning the Game:**
  - The first player to connect four of their symbols wins.
- **Draw:**
  - If the board is full and no player has connected four symbols, the game ends in a draw.

## Running the Game on Different Devices

To play the game with clients on different devices:

1. **Ensure Network Connectivity:**
   - Both the server and client devices must be on the same network or accessible over the internet.
2. **Find the Server's IP Address:**
   - On the server machine, determine the IP address (e.g., `192.168.1.100`).
3. **Adjust Firewall Settings:**
   - Allow incoming connections on port `4444` on the server machine.
4. **Run the Server:**
   - Start the server program on the server machine.
5. **Run the Clients:**
   - On client machines, run the client program using the server's IP address.
6. **Multiple Clients:**
   - You can run multiple clients (in even numbers). The server will pair them up for separate game sessions.

## Understanding TCP/IP Usage

This project makes extensive use of TCP/IP networking for client-server communication.

- **TCP Sockets:**
  - The server and clients use TCP sockets (`SOCK_STREAM`) for reliable communication.
  - TCP ensures that all messages are delivered in order and without errors.

- **Server Networking:**
  - The server listens for incoming TCP connections on a specified port (`4444`).
  - Uses `bind()`, `listen()`, and `accept()` functions to handle connections.

- **Client Networking:**
  - Clients establish a TCP connection to the server using `connect()`.
  - Communicate with the server by reading from and writing to the TCP socket.

- **Handling Multiple Connections:**
  - The server uses `fork()` to create child processes for handling multiple pairs of clients.
  - Each child process manages a separate game session between two clients.

- **Data Transmission:**
  - Game data (e.g., moves, board state) is transmitted over TCP sockets.
  - The code ensures synchronization between clients through the server.

- **Network Byte Order:**
  - Uses `htonl()`, `htons()`, `ntohl()`, `ntohs()` functions to ensure proper byte order in network communication.

## Troubleshooting

- **Client Cannot Connect to Server:**
  - Verify the server's IP address and ensure the server is running.
  - Check firewall settings on the server machine to allow TCP connections on port `4444`.
  - Ensure both devices are on the same network or that port forwarding is correctly set up.

- **Game Does Not Start After Connecting Clients:**
  - Clients are paired in the order they connect. Ensure an even number of clients are connected.
  - Check for error messages in the server or client terminals.

- **Colors Not Displaying Correctly:**
  - Ensure your terminal supports ANSI color codes.
  - On Windows, use a terminal like Git Bash or Windows Terminal.

- **Connection Issues Over the Internet:**
  - If connecting over the internet, ensure the server's public IP address is used.
  - Configure port forwarding on the server's router for port `4444`.
  - Be mindful of security considerations when exposing ports.

## Contributing

Contributions are welcome! If you'd like to improve this project, please fork the repository and submit a pull request.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

Distributed under the MIT License. See `LICENSE` for more information.

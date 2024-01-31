# CyberEcho Chat Application

## Overview
CyberEcho is a real-time chat application that bridges terminal-based and web-based communication. It combines a C server for backend processing with a Node.js server for web interface interactions.

## Getting Started

### Prerequisites
- GCC Compiler for C
- Node.js and npm
- Terminal and Web Browser

### Node.js Installation
1. **Download Node.js**: Visit [Node.js Official Website](https://nodejs.org/) and download the installer for your OS.

2. **Install Node.js**: Run the downloaded installer and follow the instructions to install Node.js and npm.

3. **Verify Installation**: Open a terminal and execute:
   ```bash
   node -v
   npm -v


### Server Installation
1. **Compile the C Server**
   ```bash
   gcc -pthread -o server server.c

2. **Run the Server**
   ```bash
   sudo ./server
   
- In case of port conflicts:
   ```bash
   sudo lsof -i :9000
   sudo kill -9 [PID]


### Client Setup
1. **Compile and Run Client Instances**
   ```bash
   gcc -pthread -o client client.c
   sudo ./client

   gcc -pthread -o client2 client.c
   sudo ./client2


### Web Interface Setup
1. **Initialize Node.js Project**:
   Navigate to the project directory where `server.js` is located and run:
   ```bash
   npm init
   npm install express socket.io net
   
2. **Start the Node.js Server**
   ```bash
   node server.js

3. **Access the Web Interface**
   ```bash
   Open http://localhost:3000 in your browser.


### Troubleshooting
**Process Management**
   ```bash
   ps aux | grep 'server.js'



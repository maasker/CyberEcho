# CyberEcho Chat Application

## Overview
CyberEcho is a real-time chat application that bridges terminal-based and web-based communication. It combines a C server for backend processing with a Node.js server for web interface interactions.

## Getting Started

### Prerequisites
- GCC Compiler for C
- Node.js and npm
- Terminal and Web Browser

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
1. **Start the Node.js Server**
   ```bash
   node server.js

2. **Access the Web Interface**
   ```bash
   Open http://localhost:3000 in your browser.
  


### User Guide
- The C server manages chat connections and messaging.
- Terminal clients provide direct chat interfaces.
- The web application offers a graphical UI for chatting.

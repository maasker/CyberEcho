const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const net = require('net');
const fs = require('fs');
const path = require('path');

// Initialize Express app and HTTP server
const app = express();
const server = http.createServer(app);

// Initialize Socket.io for real-time communication
const io = socketIo(server);

// Establish a connection to the C server
const cServer = new net.Socket();
cServer.connect({ port: 9000, host: 'localhost' });

// Serve static files (HTML, CSS, JS) from the 'public' directory
app.use(express.static('public'));

// Error handling for C server connection
cServer.on('error', (err) => {
    console.error('Failed to connect to C server:', err.message);
});

// Socket.io connection handler
io.on('connection', (webSocket) => {
    console.log('Web client connected');

    // Handle disconnection
    webSocket.on('disconnect', () => {
        console.log('Web client disconnected');
    });

    // Handle login requests from web clients
    webSocket.on('login', (userData) => {
        // Validate user credentials
        fs.readFile(path.join(__dirname, 'user_credentials.txt'), 'utf8', (err, data) => {
            if (err) {
                console.error('Error reading user credentials:', err);
                return;
            }
            const users = data.split('\n').map(line => line.trim().split(/\s+/));
            const isValidUser = users.some(([username, password]) => username === userData.username && password === userData.password);

            // If valid, format and send login data to C server
            if (isValidUser) {
                let loginData = `login|${userData.username}|${userData.password}`;
                cServer.write(loginData);
                webSocket.emit('login status', { success: true });
            } else {
                webSocket.emit('login status', { success: false });
            }
        });
    });

    // Handle chat messages from web clients
    webSocket.on('chat message', (msgData) => {
        // Format message for C server (5 is a placeholder user ID)
        let formattedMessage = `send|5|${msgData.message}`;
        cServer.write(formattedMessage);
    });
});

// Broadcast messages received from C server to all connected web clients
cServer.on('data', (data) => {
    io.emit('chat message', data.toString());
});

// Start the HTTP server
server.listen(3000, () => {
    console.log('Listening on *:3000');
});

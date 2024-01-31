$(function() {
    var socket = io();
    var currentUser;

    // Handle login form submission
    $('#loginForm').submit(function(e) {
        e.preventDefault();
        var username = $('#username').val();
        var password = $('#password').val();
        currentUser = username;
        socket.emit('login', { username: username, password: password });
    });

    // Listen for login status from the server
    socket.on('login status', function(data) {
        if (data.success) {
            $('#loginContainer').hide();
            $('#chatContainer').show();
        } else {
            alert('Login failed. Please check your credentials.');
        }
    });

    // Handle chat message form submission
    $('#chatForm').submit(function(e) {
        e.preventDefault();
        var message = $('#input').val();
        if (message.trim() !== '') {
            socket.emit('chat message', { username: currentUser, message: message });
            $('#input').val('');
        }
    });    

    // Display incoming messages
    socket.on('chat message', function(msg) {
        $('#messages').append($('<li>').text(msg));
        var chatBox = $('#chatContainer');
        chatBox.scrollTop(chatBox.prop('scrollHeight'));
    });
});

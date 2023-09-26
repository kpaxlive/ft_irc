<h1 align="center"> 📮 IRC SERVER </h1>

<h2 align="center">Internet Relay Chat</h2>

<!-- TABLE OF CONTENTS -->
<summary>Table of Contents</summary>
<ol>
<li>
	<a href="#-about">About The Project</a>
</li>
<li>
	<a href="#-our-irc-commands">Our commands</a>
</li>
<li><a href="#%EF%B8%8F-start-irc-server">Usage</a></li>
<li><a href="#-resources">Resources</a></li>
</ol>
</details>

## 🤔 About
This project is about creating our own IRC server. We used an actual IRC client to connect to our server and test it.
**IRC** (Internet Relay Chat) is a protocol for real-time text messaging between internet-connected computers created in **1988**. It is mainly used for group discussion in chat rooms called “**channels**” although it supports private messages between two users, data transfer, and various client-side commands.

#### Skills

- Network & system administration
- Rigor
- Object-oriented programming

### Mandatory part
  - We have to develop an IRC server in C++ 98.
  - The server must be capable of handling multiple clients at the same time and never hang.
  - Only 1 poll() (or equivalent) can be used for handling all these operations (read, write, but also listen, and so forth).
  - An IRC client must be able to connect to your server without encountering any error.
  - Communication between client and server has to be done via TCP/IP (v4).
  - Using the IRC client with our server must be similar to using it with any official IRC server. However, we only have to implement the following features:
    -  We must be able to authenticate, set a nickname, a username, join a channel, send and receive private messages using the IRC client.
    -  All the messages sent from one client to a channel have to be forwarded to every other client that joined the channel.
    -  We must have operators and regular users.
    -  Then, we have to implement the commands that are specific to operators.

## 🔑 Our IRC Commands
These are the available commands in our IRC Server:
  - **PASS**: The **PASS** command is used to set a 'connection password'.
  - **NICK**: NICK command is used to give user a nickname or change the existing one.
  - **USER**: he USER command is used at the beginning of connection to specify the username, hostname and realname of a new user.
  - **QUIT**: A client session is terminated with a quit message.
  - **JOIN**: The JOIN command is used by a user to request to start listening to the specific channel.
  - **MEMBER**: The MEMBER command shows the channels that user belongs to.
  - **PART**: The PART command causes the user sending the message to be removed from the list of active members for all given channels listed in the parameter string.
  - **KICK**: The KICK command can be used to request the forced removal of a user from a channel.
  - **PRIVMSG**: PRIVMSG is used to send private messages between users, as well as to send messages to channels.
  - **NOTICE**: The NOTICE command is for admins to make announcements to channels.
  - **WHO**: The WHO command needs a channel parameter. Command shows user list of that channel and shows who is the admin.
  - **LIST**: The LIST command shows the channels that user belongs to.

## ⚙️ Start IRC Server
To compile the program, use:
  - `make`
To start the Server, use:
  - `./ircserv <port> <password>`
    - **port**: The port number on which your IRC server will be listening to for incoming IRC connections.
    - **password**: The connection password. It will be needed by any IRC client that tries to connect to your server.
To connect to the server, you can use:
  - `nc <IP ADDRESS> <PORT>`
    - **IP ADDRESS**: Host IP address.
    - **PORT**: The PORT that the server listening on.
  - You can also use an IRC Client e.g LimeChat, WeeChat...

## 👨‍💻 Resources
* [What is a Socket?](https://www.tutorialspoint.com/unix_sockets/what_is_socket.htm)
* [Unix Socket - Network Addresses](https://www.tutorialspoint.com/unix_sockets/network_addresses.htm)
* [Unix Socket - Core Functions](https://www.tutorialspoint.com/unix_sockets/socket_core_functions.htm)
* [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/html/)
* [The UChicago χ-Projects](http://chi.cs.uchicago.edu/chirc/index.html)
* [Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
* [Internet Relay Chat: Architecture](https://datatracker.ietf.org/doc/html/rfc2810)
* [Internet Relay Chat: Channel Management](https://datatracker.ietf.org/doc/html/rfc2811)
* [Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
* [Internet Relay Chat: Server Protocol](https://datatracker.ietf.org/doc/html/rfc2813)

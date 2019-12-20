# Sloth Chat
### by William Cao–Period 10

# Proposal

### Description
A terminal chat program in which multiple users on the same network can communicate with each other. The user who starts the chat will also host the server and store all chat messages and log. 

### Usage
To start the program:
1. The user will run the executable (there will only be one out file).
2. The user will choose to open an existing chat log, start a new chat, or connect to one. Use number keys to determine options.
	- If the user chooses a new chat, the user will enter password (blank for none) and name of chat (must be unique).
	- If user connects to existing one, the user will enter IP, port, and password
	- If user chooses an existing chat, user will have to launch the program with name of chat as an argument.
4. User can then start typing away.

Information displayed at all times:
- IP address and port number (right aligned)
- Error messages (left aligned and covers IP address). Must press escape to close error message.

The controls when using the program is inspired by vim:
- To start typing, user must first press ESCAPE then "i". This will show lines on the bottom of the terminal to type in (like command mode in vim).
- To view previous messages, user will press ESCAPE and use arrow keys or "h" and "l" (lowercase L) to move up and down respectively.
- To view information about the chat (who is connected, when chat created, etc.), the user will press ESCAPE then "m".
- To view password, user will press ESCAPE then type in "password" and finally press enter.
- To view last error message, user will press ESCAPE then press "e".
- To quit the chat, user will press ESCAPE then "q" twice (to confirm). Chat will display "\<username\> has left".
![Example chat](/documentation/example_chat.png)

### Technical Design (project parts)
#### Storage of log / chat messages / chat details (all user who connected, password, last created, etc.)
- A directory will be created in "~/.slothchat".
- A separate directory inside "~/.slothchat" for each chat instance.
- Each chat instance directory has the following:
	- messages.txt: All messages sent in the chat
	- details.txt: Server information (password, users who connected, creation date, last accessed).
	- log.txt: Debugging purposes. 
- Uses: "working with files"

#### Networking aspect
The user who created the chat / hosts the chat is acting both as the client and server. The process will fork with one acting as server and the other as server. All communication will be done to the server process.
![Network Image](/documentation/network.png)
The server and owner client will communicate using shared memory and semaphores. Server can write to the shared memory. Client can only read from.
Both the owner client and connecting clients talk to the server through the network.
Every message sent to server by client will include:
- Message
- Type (leaving chat, sending message)
- Username
- Time sent (UNIX timestamp and millisecond)

#### Handling displaying both what the user is typing and chat being updated
The terminal is partitioned into three parts:
1. Error messages / IP, port
2. Chat log
3. What the user is typing / command being typed (like normal mode vim where you can enter commands)

There will be 3 threads:
1.. Updating stdout: Every .125 seconds, the terminal will update with a string generated from three other strings containing each of the above bullet points.
2. Listening to server for updates: updates string presenting terminal Part 2.
3. Main thread: Listens for user input and send information to server and builds string for Part 1 and Part 3.

Threads sources: 
- [Library](https://en.wikipedia.org/wiki/POSIX_Threads)
- [Locks](https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/)
- Additional note: I have used multithreading in Java before during the Google mentorship program, in which I made a collaborative photo editor that allows for multiple computers in the same network communicate with each other.

#### Getting user input
Terminal will be switched to "raw" mode so ESCAPE, arrow keys, etc will be parsed.

#### Data structure and algorithms
1. Priority queue (sorted list if I struggle too much to make one): To keep chat in time order. If one computer stalls in the network, the order of messages will be maintained.
2. Rope ([source](https://en.wikipedia.org/wiki/Rope_(data_structure))): Building strings and editing midline. This might not be used since I will cap message length to 250 characters (excluding end of line character) and an array will suffice.

### Timeline
12/23 -- Get raw mode working and building user input. Print terminal in an organized manner (divided into 3 parts).  
12/25 -- Get different mode working (typing, viewing chat, viewing details, etc.)  
12/29 -- Learn and become familiar with threading with locks. For testing, I can make a program continuously enter into a pipe and the chat program read it.  
12/31 -- Store what was typed into disk. Implement rope?  
Working since last decade to write this program?!?!  
1/2 -- Open program from what was written to disk  
1/5 -- Implement priority queue.  
1/11 -- Have networking finished implemented.  
1/15 -- Debug.  
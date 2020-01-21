# Sloth Chat

### The team:
William Cao Period 10 

# Broad descriptor of project
A chat program in which up to 20 people can connect to one computer (host) and talk to one another. The host saves the chat log for continutation in the future.

# Required libraries
None!

# How to use
### To install the program:
1. Clone this repository using https:
    ```sh
    $ git clone https://github.com/WilliamC07/c-chatter.git
    ```
2. Run make:
    ```sh
    $ make
    ```
### Run the program
1. The host computer should run one of the following:
    To open an existing chat, run:
    ```sh
    $ ./output -o <name of chat> 
    # To see chat that you have run
    $ ls ~/.slothchat
    ```
    To create a new chat, run:
    ```sh
    $ ./output -c <name of chat> <your username>
    ```
    Note: Your username cannot be more than 25 characters long. The program will cut off additional characters. Username cannot be "system" or "System"
2. Everyone who wants to run the program should run:
    ```sh
    $ ./output -j <host IPv4 address> <client username>
    ```
    Note: there can be a maximum of 20 active people connected including the host.
    Note: Your username cannot be more than 25 characters long. The program will cut off additional characters. Username cannot be "system" or "System"

### Instructions when running
- You can start typing right away and send messages! Just press enter at the end of your message. You are capped at 255 characters. The program will ignore all input past 255 characters.
- Your message cannot start with a white space nor be blank (no characters/only white space).
- Use the up and down arrow keys to scroll the chat. Don't have up and down arrow key? Get a **real** keyboard.
- How do i read the display? Here:
![How to read](https://github.com/WilliamC07/c-chatter/blob/master/documentation/demo.png "How to read")

### Need a reminder on the terminal? Run:
```sh
$ ./output -h
```

### To uninstall
1. Delete the repository
2. Delete "~/.slothchat"
    ```sh
    $ rm -rf ~/.slothchat
    ```

# Bugs
- This isn't really a bug, there there are some extended waiting periods (hardcoded wait() periods) for the socket to be ready or packet to be sent out before shutting down the server.

# Works Cited
- [Entering raw mode](https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html)
- Terminal escape values (ex.setting text color): [this](https://stackoverflow.com/questions/1508490/erase-the-current-printed-console-line) and [this](http://www.climagic.org/mirrors/VT100_Escape_Codes.html)
- [Threading](https://www.geeksforgeeks.org/mutex-lock-for-linux-thread-synchronization/)
- Networking: [dw](https://github.com/mks65/dwsource/tree/master/networking) and [this](https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/)
- [Checking if port is open (this isn't code, it is to help me debug)](https://unix.stackexchange.com/questions/92608/how-can-i-tell-if-a-tcp-port-is-open-or-not)

# Dev log
[link](https://github.com/WilliamC07/c-chatter/blob/master/devlog.txt)

# Comments
- I hope there aren't any race conditions :)
- The program uses port 7777

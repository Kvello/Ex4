# Network-Sim
Developed for the course Computer Networks sem I 2023.
## Description
Implements a server and client using UDP sockets. The client reads a file and transmitts it to the server in fixed sized messages.
The message size is tunable. The code also implements timeouts, CRC for error detection and simulates error such as message loss
and bit flip error. The probabilities for errors are also adjustable. An breif analysis of results can be found in this repo aswell.
 ## Basic usage
 Run:
 ```bash
make server && make client
build/server <path/to/write/file>
build/client <ipv4_addr> <path/to/read/file>
```

The interesting parameters like error probabilities, message size, timeout length are defined as
macros and can be adjusted in udp.h, msg.h
## Known bugs
The server is unable to initialize new connections, this means it need to be restarted after a 
transmission is over. The current implementation timeouts the server after 5s without new messages

## Further work
See known bugs.
Also ipv6 support should be fairly straightforward to implement.

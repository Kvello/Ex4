# Network-Sim

Developed for the course Computer Networks sem I 2023.
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

## Furter work
See known bugs.
Also ipv6 support should be fairly straigth forward to implement.

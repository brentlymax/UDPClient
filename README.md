# UDP_Client
 UDP Client written in C for Windows

This client works in conjunction with my UDP_Server program.
Run the server program and keep it listening prior to running this.
Upon execution, the client sends data packets to the server.
Upon receiving a packet, the server checks for validity then sends a response back to the client.

Requires the following 3 libraries:
Ws2_32.lib
Mswsock.lib
AdvApi32.lib

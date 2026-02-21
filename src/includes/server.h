/*
 * Multi-threaded TCP server — public API.
 * C99, POSIX. Target: Unix-like systems only.
 */

#ifndef SERVER_H
#define SERVER_H

/* Default listen port. */
#define SERVER_DEFAULT_PORT 8080

/* Max length of a single read line from client. */
#define SERVER_LINE_MAX 1024

/*
 * Run the server: bind, listen, accept in a loop; each connection
 * is handled in a separate thread. Returns 0 on normal shutdown,
 * non-zero on error. Port is in host byte order.
 */
int server_run(unsigned short port);

#endif /* SERVER_H */

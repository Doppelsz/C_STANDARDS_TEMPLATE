/*
 * server.h — Single-threaded static HTML5 server. C99, POSIX.
 * Target: Unix-like systems only. Inline citations per INLINE_TRACEABILITY_GUIDE.
 */

#ifndef SERVER_H
#define SERVER_H

/* C99 6.2.1: one definition per identifier; header guard avoids multiple definitions. */
#define SERVER_PORT 8080

/* Run the server loop: accept connections and respond with static HTML5. Returns 0 on normal stop. */
int server_run(void);

#endif /* SERVER_H */

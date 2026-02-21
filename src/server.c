#define _POSIX_C_SOURCE 200809L
/*
 * Single-threaded server — serves static HTML5 on port 8080.
 * C99, POSIX. Target: Unix-like systems only.
 * Inline citations per INLINE_TRACEABILITY_GUIDE.
 */

#include "server.h"
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

/* C99 7.14.1.1: volatile sig_atomic_t for async-signal-safe use in handler. */
static volatile sig_atomic_t g_running = 1;

static void on_signal(int signum)
{
    (void)signum;
    g_running = 0;
}

/* C99 6.4.4.2: string literal for static HTML5 document. */
static const char html[] =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head><meta charset=\"UTF-8\"><title>Server</title></head>\n"
    "<body><h1>Static HTML5</h1><p>Single-threaded server on port 8080.</p></body>\n"
    "</html>\n";

/* C99 5.2.4.1: minimum limits; snprintf and buffer sizes within range. */
static const size_t buf_size = 4096u;

/* Build HTTP response (Status + headers + body). C99 6.2.1: block-scope declarations before statements. */
static int build_response(char *buf, size_t size, size_t *out_len)
{
    int n;
    /* C99 7.19.6.5: snprintf returns number of chars that would have been written. */
    n = snprintf(buf, size,
                 "HTTP/1.1 200 OK\r\n"
                 "Content-Type: text/html; charset=UTF-8\r\n"
                 "Content-Length: %zu\r\n"
                 "Connection: close\r\n"
                 "\r\n"
                 "%s",
                 (sizeof(html) - 1u), html);
    if (n < 0 || (size_t)n >= size)
        return -1;
    *out_len = (size_t)n;
    return 0;
}

/* C99 6.2.1: declarations at block scope; POSIX socket APIs. */
int server_run(void)
{
    int server_fd;
    struct sockaddr_in addr;
    char *buf;
    size_t resp_len;
    int ret;

    ret = 0;
    server_fd = -1;
    buf = NULL;

    /* Allocate response buffer once. C99 7.20.3: malloc. */
    buf = (char *)malloc(buf_size);
    if (buf == NULL)
    {
        (void)fprintf(stderr, "server: malloc failed\n");
        return -1;
    }

    if (build_response(buf, buf_size, &resp_len) != 0)
    {
        (void)fprintf(stderr, "server: build_response failed\n");
        ret = -1;
        goto cleanup;
    }

    /* POSIX: socket creation. */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        (void)fprintf(stderr, "server: socket: %s\n", strerror(errno));
        ret = -1;
        goto cleanup;
    }

    /* Allow reuse of address to avoid "Address already in use" after restart. */
    {
        int one = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0)
        {
            (void)fprintf(stderr, "server: setsockopt: %s\n", strerror(errno));
            ret = -1;
            goto cleanup;
        }
    }

    (void)memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((unsigned short)SERVER_PORT);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        (void)fprintf(stderr, "server: bind: %s\n", strerror(errno));
        ret = -1;
        goto cleanup;
    }

    if (listen(server_fd, 5) < 0)
    {
        (void)fprintf(stderr, "server: listen: %s\n", strerror(errno));
        ret = -1;
        goto cleanup;
    }

    (void)printf("Listening on port %d (single-threaded, static HTML5)\n", SERVER_PORT);

    /* POSIX: signal handler so shutdown frees buffer and closes socket (valgrind-clean). */
    {
        struct sigaction sa;
        (void)memset(&sa, 0, sizeof(sa));
        sa.sa_handler = on_signal;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0; /* no SA_RESTART so accept() returns EINTR on SIGINT/SIGTERM */
        (void)sigaction(SIGINT, &sa, NULL);
        (void)sigaction(SIGTERM, &sa, NULL);
    }

    /* Single-threaded accept loop. */
    while (g_running)
    {
        int client_fd;
        struct sockaddr_in client_addr;
        socklen_t len;
        ssize_t sent;
        ssize_t n;

        len = (socklen_t)sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
        if (client_fd < 0)
        {
            if (errno == EINTR && !g_running)
                break;
            (void)fprintf(stderr, "server: accept: %s\n", strerror(errno));
            continue;
        }

        /* Send full response. */
        sent = 0;
        while (sent < (ssize_t)resp_len)
        {
            n = write(client_fd, buf + (size_t)sent, resp_len - (size_t)sent);
            if (n <= 0)
                break;
            sent += n;
        }
        (void)close(client_fd);
    }

cleanup:
    free(buf);
    if (server_fd >= 0)
        (void)close(server_fd);
    return ret;
}

int main(void)
{
    /* C99 5.1.2.2.1: main returns int. */
    return server_run();
}

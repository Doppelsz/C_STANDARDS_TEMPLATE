/*
 * Multi-threaded TCP server.
 * C99, POSIX. Target: Unix-like systems only.
 * Build: -std=c99 -I src/includes -lpthread
 */

#include "server.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

/* Set by signal handler to request shutdown. */
static volatile sig_atomic_t g_running = 1;

static void on_signal(int signum)
{
    (void)signum;
    g_running = 0;
}

/*
 * Thread entry: handle one client. arg is a pointer to an int (client fd);
 * caller passes heap-allocated storage; this function takes ownership and
 * frees it after reading the fd.
 */
static void *handle_connection(void *arg)
{
    int client_fd;
    int *pfd;
    char buf[SERVER_LINE_MAX];
    ssize_t n;
    const char *reply = "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\nOK\n";

    pfd = (int *)arg;
    if (pfd == NULL)
    {
        return NULL;
    }
    client_fd = *pfd;
    free(pfd);
    pfd = NULL;

    /* Read one line (simple) and send a minimal response. */
    n = 0;
    while (n < (ssize_t)(sizeof(buf) - 1))
    {
        ssize_t r = read(client_fd, buf + (size_t)n, 1);
        if (r <= 0)
        {
            break;
        }
        if (buf[n] == '\n')
        {
            n++;
            break;
        }
        n += r;
    }
    buf[n >= 0 ? (size_t)n : 0] = '\0';

    (void)send(client_fd, reply, (size_t)strlen(reply), 0);
    (void)close(client_fd);
    return NULL;
}

int server_run(unsigned short port)
{
    int listen_fd;
    struct sockaddr_in addr;
    int opt;
    int ret;
    fd_set read_fds;
    struct timeval tv;
    struct sigaction sa;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        perror("socket");
        return -1;
    }

    opt = 1;
    ret = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret < 0)
    {
        perror("setsockopt");
        (void)close(listen_fd);
        return -1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    ret = bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        perror("bind");
        (void)close(listen_fd);
        return -1;
    }

    ret = listen(listen_fd, 16);
    if (ret < 0)
    {
        perror("listen");
        (void)close(listen_fd);
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_signal;
    sigemptyset(&sa.sa_mask);
    ret = sigaction(SIGINT, &sa, NULL);
    if (ret < 0)
    {
        perror("sigaction");
        (void)close(listen_fd);
        return -1;
    }

    (void)printf("Server listening on port %u (Ctrl+C to stop).\n",
                 (unsigned int)port);

    while (g_running)
    {
        int client_fd;
        struct sockaddr_in peer;
        socklen_t peer_len;
        int *pfd;
        pthread_t thr;

        FD_ZERO(&read_fds);
        FD_SET(listen_fd, &read_fds);
        tv.tv_sec = 1;
        tv.tv_usec = 0;

        ret = select(listen_fd + 1, &read_fds, NULL, NULL, &tv);
        if (ret < 0)
        {
            if (errno == EINTR)
            {
                continue;
            }
            perror("select");
            break;
        }
        if (ret == 0 || !FD_ISSET(listen_fd, &read_fds))
        {
            continue;
        }

        peer_len = sizeof(peer);
        client_fd = accept(listen_fd, (struct sockaddr *)&peer, &peer_len);
        if (client_fd < 0)
        {
            if (errno == EINTR || !g_running)
            {
                continue;
            }
            perror("accept");
            continue;
        }

        pfd = (int *)malloc(sizeof(int));
        if (pfd == NULL)
        {
            (void)close(client_fd);
            continue;
        }
        *pfd = client_fd;

        ret = pthread_create(&thr, NULL, handle_connection, pfd);
        if (ret != 0)
        {
            free(pfd);
            (void)close(client_fd);
            continue;
        }
        (void)pthread_detach(thr);
    }

    (void)close(listen_fd);
    (void)printf("Server stopped.\n");
    return 0;
}

int main(void)
{
    unsigned short port;
    const char *env_port;

    port = SERVER_DEFAULT_PORT;
    env_port = getenv("PORT");
    if (env_port != NULL)
    {
        long val;
        val = strtol(env_port, NULL, 10);
        if (val > 0 && val <= 65535)
        {
            port = (unsigned short)val;
        }
    }

    return server_run(port) == 0 ? 0 : 1;
}

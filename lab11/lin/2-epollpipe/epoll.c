/**
 * SO, 2016
 * Lab #11, Advanced IO Linux
 *
 * Task #2, Linux
 *
 * Epoll usage
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <wait.h>

#define CLIENT_COUNT 10

#define MSG_SIZE 32

#define ANY -1
#define PIPE_READ 0
#define PIPE_WRITE 1

#define MAGIC_EXIT (1ULL << 63) /* half of (1<<64) */
#define MAGIC_MASK 0xFF

#include "utils.h"

static int pipes[CLIENT_COUNT][2];
int chld_pid[CLIENT_COUNT];

#define USE_EVENTFD
int event_fd;

static void set_event(int index, uint64_t *event) {
    *event = MAGIC_EXIT | index;
}

static int get_index(uint64_t event) { return (int)(MAGIC_MASK & event); }

static int is_magic(uint64_t n, int k) {
    if (n & (1 << (k - 1)))
        return 1;
    else
        return 0;
}

static int server(void) {
    /* epoll handle */
    int efd;

    /* epoll event structure */
    struct epoll_event ev;

    struct epoll_event ev_list[5];

    int i;
    int recv_msgs;
    int recv_count;
    char msg[MSG_SIZE];
    int status, rc, index;
    uint64_t event;

    printf("server: started\n");

    /* init epoll and close write pipe heads */
    efd = epoll_create(CLIENT_COUNT + 1);
    for (i = 0; i < CLIENT_COUNT; i++) {
        rc = close(pipes[i][PIPE_WRITE]);
        DIE(rc < 0, "close failed");
        ev.events = EPOLLIN;
        ev.data.fd = pipes[i][PIPE_READ];
        epoll_ctl(efd, EPOLL_CTL_ADD, pipes[i][PIPE_READ], &ev);
    }

    /* add eventfd in epoll*/
    ev.events = EPOLLIN;
    ev.data.fd = event_fd;
    epoll_ctl(efd, EPOLL_CTL_ADD, event_fd, &ev);

    /* number of received messages */
    recv_msgs = 0;

    while (recv_msgs < CLIENT_COUNT) {
        printf("SERVER: waiting for messages (recv_msgs = %i)\n", recv_msgs);
        /* use epoll to wait to read from pipes */
        rc = epoll_wait(efd, &ev_list, CLIENT_COUNT * 4, -1);
        DIE(rc < 0, "epoll_wait failed");

        for (i = 0; i < rc; i++) {
            if (ev_list[i].data.fd == event_fd) {
                recv_count = read(ev_list[i].data.fd, msg, 8);
                uint64_t e = *(uint64_t *)msg;

                printf("SERVER: received MAGIC exit 0x%lx\n", e);

                rc = epoll_ctl(efd, EPOLL_CTL_DEL,
                               pipes[get_index(e)][PIPE_READ], &ev_list[i]);
                DIE(rc < 0, "epoll_ctl failed");

                close(get_index(e));
                recv_msgs++;
            } else {
                recv_count = read(ev_list[i].data.fd, msg, MSG_SIZE);
                DIE(recv_count < 0, "read");

                msg[recv_count] = '\0';
                printf("SERVER: received:%s\n", msg);
            }
        }
    }
    printf("SERVER: going to wait for clients to end\n");

    for (i = 0; i < CLIENT_COUNT; ++i) {
        rc = waitpid(ANY, &status, 0);
        DIE(rc < 0, "waitpid");
    }

    printf("server: exiting\n");
    return 0;
}

static int client(unsigned int index) {
    char msg[MSG_SIZE];
    int rand_no, rc;
    uint64_t event = 0;

    /* Close read pipe head, wait random time and send a message */
    // printf("client %i: started\n", index);

    rc = close(pipes[index][PIPE_READ]);
    DIE(rc < 0, "close failed");

    srandom(index);

    sleep(random() % 10);

    // printf("client %i: writing message\n", index);

    memset(msg, 0, MSG_SIZE);
    rand_no = (char)(random() % 30);
    sprintf(msg, "<%i>:<%c>", getpid(), 'a' + rand_no);
    rc = write(pipes[index][PIPE_WRITE], msg, MSG_SIZE);
    DIE(rc < 0, "write");

#ifdef USE_EVENTFD
    /* Init event (see set_event()) and use it to signal the
     * server of our termination
     */
    set_event(index, &event);
    rc = write(event_fd, &event, sizeof(event));

    printf("CLIENT%d:sending MAGIC exit = 0x%lx\n", index,
           (unsigned long)event);

#endif

    // printf("client %i: exiting\n", index);
    rc = close(pipes[index][PIPE_WRITE]);
    DIE(rc < 0, "close");

    return 0;
}

int main(void) {
    unsigned int i;
    int rc;
    pid_t pid;

    /* Init pipes */
    for (i = 0; i < CLIENT_COUNT; i++) {
        rc = pipe(pipes[i]);
        DIE(rc < 0, "pipe");
    }

#ifdef USE_EVENTFD
    /* Create eventfd */
    event_fd = eventfd(0, 0);
#endif

    /* Create clients as child processes */
    for (i = 0; i < CLIENT_COUNT; i++) {
        pid = fork();
        if (pid < 0) {
            perror("fork");
            return EXIT_FAILURE;
        } else if (!pid) {
            client(i);
            return EXIT_SUCCESS;
        }
        chld_pid[i] = pid;
    }

    server();

    return EXIT_SUCCESS;
}

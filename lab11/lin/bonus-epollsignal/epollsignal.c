/**
 * SO, 2016
 * Lab #11, Advanced IO Linux
 *
 * Task #2, Linux
 *
 * Epoll usage
 */

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <wait.h>

#define CLIENT_COUNT 10

#define MSG_SIZE 32

#define ANY -1
#define PIPE_READ 0
#define PIPE_WRITE 1

#include "utils.h"

static int pipes[CLIENT_COUNT][2];
int chld_pid[CLIENT_COUNT];

int sfd;

static int get_index(pid_t pid) {
    int i;

    for (i = 0; i < CLIENT_COUNT; i++)
        if (chld_pid[i] == pid) return i;

    DIE(1, "there is no such PID.");
}

static int server(void) {
    /* epoll handle */
    int efd;

    /* epoll event structure */
    struct epoll_event ev;

    int i;
    int recv_msgs;
    int recv_count;
    char msg[MSG_SIZE];
    int status, rc, index;
    uint64_t event;
    struct signalfd_siginfo fdsi;

    printf("server: started\n");

    ev.events = EPOLLIN;

    /* init epoll*/
    efd = epoll_create(CLIENT_COUNT + 1);
    DIE(efd < 0, "server: epoll_create failed");

    for (i = 0; i < CLIENT_COUNT; i++) {
        /* close write pipe heads */
        rc = close(pipes[i][PIPE_WRITE]);
        DIE(rc < 0, "server: close pipe failed");

        ev.data.fd = pipes[i][PIPE_READ];
        epoll_ctl(efd, EPOLL_CTL_ADD, pipes[i][PIPE_READ], &ev);
        DIE(rc < 0, "server: epoll_ctl failed");
    }

    /* add signalfd in epoll */
    ev.data.fd = sfd;
    epoll_ctl(efd, EPOLL_CTL_ADD, sfd, &ev);
    DIE(rc < 0, "server: epoll_ctl failed");

    /* number of received messages */
    recv_msgs = 0;

    while (recv_msgs < CLIENT_COUNT) {
        printf("server: waiting for messages (recv_msgs = %i)\n", recv_msgs);
        /* use epoll to wait to read from pipes */
        rc = epoll_wait(efd, &ev, 1, -1);
        DIE(rc < 0, "epoll_wait failed");

        if (ev.data.fd == sfd && (ev.events & EPOLLIN)) {
            recv_count = read(ev.data.fd, msg, MSG_SIZE);
            DIE(recv_count < 0, "read");
            
            msg[recv_count] = '\0';
            printf("server: received:%s\n", msg);

        } else if (ev.events & EPOLLIN) {
            rc = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
            DIE(rc < 0, "read");

            if (fdsi.ssi_signo == SIGCHLD) {
                printf("Got SIGINT from %d\n", fdsi.ssi_pid);

                rc = epoll_ctl(efd, EPOLL_CTL_DEL,
                               pipes[get_index(fdsi.ssi_pid)][PIPE_READ], &ev);
                DIE(rc < 0, "epoll_ctl failed");

                rc = close(pipes[get_index(fdsi.ssi_pid)][PIPE_READ]);
                DIE(rc < 0, "close failed");

                recv_msgs++;
            } else {
                printf("Read unexpected signal\n");
            }
        }
    }
    printf("server: going to wait for clients to end\n");

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

    sleep(1);
    /* Close read pipe head, wait random time and send a message */
    printf("client%i: started\n", index);

    rc = close(pipes[index][PIPE_READ]);
    DIE(rc < 0, "close failed");

    srandom(index);
    sleep(random() % 10);

    printf("client%i: writing message\n", index);

    memset(msg, 0, MSG_SIZE);
    rand_no = (char)(random() % 30);
    sprintf(msg, "<%i>:<%c>", getpid(), 'a' + rand_no);
    rc = write(pipes[index][PIPE_WRITE], msg, MSG_SIZE);
    DIE(rc < 0, "write");

    printf("client %i: exiting\n", index);
    rc = close(pipes[index][PIPE_WRITE]);
    DIE(rc < 0, "close");
    return 0;
}

int main(void) {
    unsigned int i;
    int rc;
    pid_t pid;
    sigset_t mask;

    rc = sigemptyset(&mask);
    DIE(rc < 0, "main: sigemptyset");

    rc = sigaddset(&mask, SIGCHLD);
    DIE(rc < 0, "main: sigaddset");

    rc = sigprocmask(SIG_BLOCK, &mask, NULL);
    DIE(rc < 0, "main: sigprocmask");

    sfd = signalfd(-1, &mask, 0);
    DIE(sfd < 0, "main: signalfd");

    /* Init pipes */
    for (i = 0; i < CLIENT_COUNT; i++) {
        rc = pipe(pipes[i]);
        DIE(rc < 0, "pipe");
    }

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

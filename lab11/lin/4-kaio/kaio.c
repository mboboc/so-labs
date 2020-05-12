/**
 * SO, 2016
 * Lab #11, Advanced IO Linux
 *
 * Task #4, Linux
 *
 * KAIO
 */

#include <errno.h>
#include <fcntl.h>
#include <libaio.h>
#include <linux/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/eventfd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"

#ifndef BUFSIZ
#define BUFSIZ 4096
#endif

/* file names */
static char *files[] = {"/tmp/slo.txt", "/tmp/oer.txt", "/tmp/rse.txt",
                        "/tmp/ufver.txt"};

#define USE_EVENTFD 1
#define IOCB_FLAG_RESFD (1 << 0)

/* eventfd file descriptor */
int efd;

/* file descriptors */
static int *fds;
static char g_buffer[BUFSIZ];

static void open_files(void) {
    size_t n_files = sizeof(files) / sizeof(files[0]);
    size_t i;

    fds = malloc(n_files * sizeof(int));
    DIE(fds == NULL, "malloc");

    for (i = 0; i < n_files; i++) {
        fds[i] = open(files[i], O_CREAT | O_WRONLY | O_TRUNC, 0666);
        DIE(fds[i] < 0, "open");
    }
}

static void close_files(void) {
    size_t n_files = sizeof(files) / sizeof(files[0]);
    size_t i;
    int rc;

    for (i = 0; i < n_files; i++) {
        rc = close(fds[i]);
        DIE(rc < 0, "close");
    }

    free(fds);
}

/*
 * init buffer with random data
 */
static void init_buffer(void) {
    size_t i;

    srand(time(NULL));

    for (i = 0; i < BUFSIZ; i++) g_buffer[i] = (char)rand();
}

/*
 * wait for asynchronous I/O operations
 * (eventfd or io_getevents)
 */
static void wait_aio(io_context_t ctx, int nops) {
    struct io_event *events;
    u_int64_t efd_ops = 0;
    int rc, i;

    /* alloc structure */
    events = malloc(sizeof(*events));
    DIE(!events, "malloc failed");

#ifndef USE_EVENTFD
    /* wait for async operations to finish */
    rc = io_getevents(ctx, nops, nops, events, NULL);
    DIE(rc < 0, "io_getevents failed");

#else
    /* wait for async operations to finish use eventfd for completion notify */
    for (i = 0; i != nops; i += efd_ops) {
        rc = read(efd, &efd_ops, sizeof(efd_ops));
        DIE(rc < 0, "read(efd_ops)");
    }

#endif
}

/*
 * write data asynchronously (using io_setup(2), io_sumbit(2),
 *	io_getevents(2), io_destroy(2))
 */

static void do_io_async(void) {
    size_t n_files = sizeof(files) / sizeof(files[0]);
    size_t i;
    io_context_t ctx = 0;
    struct iocb *iocb;
    struct iocb **piocb;
    int n_aio_ops, rc;

    /* allocate iocb and piocb */
    iocb = malloc(sizeof(*iocb) * n_files);
    piocb = malloc(sizeof(*piocb) * n_files);

    for (i = 0; i < n_files; i++) {
        /* initialiaze iocb and piocb for reading */
        piocb[i] = iocb + i;
        io_prep_pwrite(iocb + i, fds[i], g_buffer, BUFSIZ, 0);

#ifdef USE_EVENTFD
        /* set up eventfd notification */
        io_set_eventfd(iocb + i, efd);
#endif
    }

    /* setup aio context */
    rc = io_setup(n_files, &ctx);
    DIE(rc < 0, "io_setup failed");

    /* submit aio */
    rc = io_submit(ctx, n_files, piocb);
    DIE(rc < 0, "io_submit failed");

    /* wait for completion*/
    wait_aio(ctx, n_files);

    /* destroy aio context */
    rc = io_destroy(ctx);
    DIE(rc < 0, "io_destroy failed");
}

int main(void) {
    int rc;

    open_files();
    init_buffer();

#ifdef USE_EVENTFD
    /* init eventfd */
    efd = eventfd(0, 0);
    DIE(efd < 0, "eventfd");

#endif

    do_io_async();

#ifdef USE_EVENTFD
    /* close eventfd */
    rc = close(efd);
    DIE(rc < 0, "close(efd)");

#endif
    close_files();

    return 0;
}

//
// Author: Alexis Alulema
// Compilation Command: cc -o sampled main.c
//

#include <stdio.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>
#include <memory.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define DAEMON_NAME     "sampled"
#define ERROR_FORMAT    "My error: %s"

#define OK              1
#define ERR_FORK        28
#define ERR_SETSID      58
#define ERR_CHDIR       37

static void _signal_handler(const int signal) {
    switch (signal) {
        case SIGHUP:
            break;
        case SIGTERM:
            syslog(LOG_INFO, "received SIGTERM, exiting.");
            closelog();
            exit(OK);
        default:
            syslog(LOG_INFO, "received unhandled signal");
    }
}

static void _do_work(void) {
    for (int i = 0; true; ++i) {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);

        syslog(LOG_INFO, "Date-Time Now: %d-%d-%d %d:%d:%d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        sleep(1);
    }
}

int main() {
    openlog(DAEMON_NAME, LOG_PID | LOG_NDELAY | LOG_NOWAIT, LOG_DAEMON);
    syslog(LOG_INFO, "staring sampled");

    // We really don't want to take over syslogd or initd, so fork.
    pid_t pid = fork();

    // Well something went wrong. fork() uses standar unix errno
    // functionality, so let's log the problem and exit.
    if (pid < 0) {
        syslog(LOG_ERR, ERROR_FORMAT, strerror(errno));
        return ERR_FORK;
    }

    // We receive a PID if we're the parent process. If we're then
    // parent, let's just exit. We only care about the forked child.
    if (pid > 0) {
        return OK;
    }

    // I'd like to be the leader of the session. Id I can't, I'm out.
    if (setsid() < -1) {
        syslog(LOG_ERR, ERROR_FORMAT, strerror(errno));
        return ERR_SETSID;
    }

    // We have console to write, so there file pointers are useless.
    // Close them.
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // This gives us read/write, read for everybody else.
    umask(S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    // Root is our working dir now.
    if (chdir("/") < 0) {
        syslog(LOG_ERR, ERROR_FORMAT, strerror(errno));
        return ERR_CHDIR;
    }

    // Now we can only be controlled by signals as we have no interactive
    // user session. So let's handle those signals
    signal(SIGTERM, _signal_handler);
    signal(SIGHUP, _signal_handler);

    _do_work();

    return OK;
}
/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/*
 * libsystem
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "libsystem.h"

static int wait_child(pid_t pid, int64_t timeout_msec, int sig) {
        struct timeval start, timeout;
        int status;

        if (timeout_msec < 0)
                return 0;

        if (timeout_msec > 0) {
                msec_to_timeval((uint64_t) timeout_msec, &timeout);

                if (gettimeofday(&start, NULL) < 0)
                        return -errno;
        }

        for (;;) {
                struct timeval current, delta;
                pid_t p;

                p = waitpid(pid, &status, timeout_msec ? WNOHANG : 0);
                if (p == pid)
                        break;

                if (gettimeofday(&current, NULL) < 0)
                        return -errno;

                timersub(&current, &start, &delta);

                if (timercmp(&timeout, &delta, <)) {
                        (void) kill(pid, sig);
                        return -ETIME;
                }

                usleep(100000);
        }

        return WEXITSTATUS(status);
}

int do_fork_exec_kill_redirect(char *const argv[], char * const envp[], int64_t timeout_msec, int sig, int fd, int flags) {
        pid_t pid;

        assert(argv);

        pid = fork();
        if (pid < 0)
                return -errno;
        else if (pid == 0) {

                if (fd >= 0) {
                        if (flags & EXEC_REDIRECT_OUTPUT)
                                dup2(fd, STDOUT_FILENO);

                        if (flags & EXEC_REDIRECT_ERROR)
                                dup2(fd, STDERR_FILENO);
                }

                if (!envp)
                        execv(*argv, argv);
                else
                        execvpe(*argv, argv, envp);

                _exit(EXIT_FAILURE);
        }

        return wait_child(pid, timeout_msec, sig);
}

#define IOPRIO_CLASS_SHIFT      (13)

static inline int ioprio_set(int which, int who, int ioprio) {
        return syscall(__NR_ioprio_set, which, who, ioprio);
}

enum {
        IOPRIO_WHO_PROCESS = 1,
        IOPRIO_WHO_PGRP,
        IOPRIO_WHO_USER,
};

int fork_exec(struct exec_info *exec) {
        pid_t pid;

        assert(exec);

        pid = fork();
        if (pid < 0)
                return -errno;
        else if (pid == 0) {
                int r;

                if (exec->out_fd >= 0)
                        dup2(exec->out_fd, STDOUT_FILENO);

                if (exec->err_fd >= 0)
                        dup2(exec->err_fd, STDERR_FILENO);

                if (exec->prio != getpriority(PRIO_PROCESS, 0)) {
                        r = setpriority(PRIO_PROCESS, 0, exec->prio);
                        if (r < 0)
                                _exit(errno);
                }

                if (exec->ioprio) {
                        r = ioprio_set(IOPRIO_WHO_PROCESS, 0, exec->ioprio << IOPRIO_CLASS_SHIFT);
                        if (r < 0)
                                _exit(errno);
                }

                if (!exec->envp)
                        execv(exec->argv[0], exec->argv);
                else
                        execvpe(exec->argv[0], exec->argv, exec->envp);

                _exit(EXIT_FAILURE);
        }

        if (exec->timeout_msec < 0)
                return pid;

        return wait_child(pid, exec->timeout_msec, exec->kill_signal);
}

int do_fork_exec_redirect(char *const argv[], char * const envp[], int64_t timeout_msec, int fd, int flags) {

        assert(argv);

        return do_fork_exec_kill_redirect(argv, envp, timeout_msec, SIGTERM, fd, flags);
}

int do_fork_exec_kill(char *const argv[], char * const envp[], int64_t timeout_msec, int sig) {

        assert(argv);

        return do_fork_exec_kill_redirect(argv, envp, timeout_msec, sig, -1, EXEC_REDIRECT_NONE);
}

int do_fork_exec(char *const argv[], char * const envp[], int64_t timeout_msec) {

        assert(argv);

        return do_fork_exec_kill(argv, envp, timeout_msec, SIGTERM);
}

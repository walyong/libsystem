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
#include <sys/time.h>
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

                p = waitpid(pid, &status, WNOHANG);
                if (p == pid)
                        break;

                if (timeout_msec == 0)
                        continue;

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

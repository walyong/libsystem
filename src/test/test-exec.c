/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/*
 * libsystem
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include "libsystem/libsystem.h"

#define TEST_EXEC_FILE "/tmp/test-exec"

static void test_do_sleep(int argc, char *argv[]) {

        assert(argc == 2);

        sleep(atoi(argv[1]));

        exit(EXIT_SUCCESS);
}

static void test_do_fork_exec(int argc, char *argv[]) {
        char *test_argv[3] = { NULL, NULL, NULL };

        test_argv[0] = argv[0];
        test_argv[1] = "1";

        assert(do_fork_exec(test_argv, NULL, 500) == -ETIME);
        assert(do_fork_exec(test_argv, NULL, 1500) == 0);
}

static void test_do_write_file(int argc, char *argv[]) {
        char *test1, *test2;

        assert(argc == 3);

        test1 = getenv("TEST1");
        test2 = getenv("TEST2");

        fprintf(stdout, "%s", argv[1]);
        fprintf(stdout, "%s", argv[2]);
        fprintf(stdout, "%s", test1);
        fprintf(stdout, "%s", test2);

        exit(EXIT_SUCCESS);
}

static void test_do_fork_exec_redirect(int argc, char *argv[]) {
        char *test_argv[4] = { NULL, "foo", "bar", NULL };
        char *test_envp[] = { "TEST1=7", "TEST2=hello", NULL };
        _cleanup_close_ int fd = -1;
        _cleanup_free_ char *buf = NULL;

        fd = creat(TEST_EXEC_FILE, 0644);
        /* Skip if file is not able to be opened. */
        if (fd < 0) {
                fprintf(stderr, "Failed to open '" TEST_EXEC_FILE "': %m, skipping\n");
                return;
        }

        test_argv[0] = argv[0];

        assert(do_fork_exec_redirect(test_argv, test_envp, 0, fd, EXEC_REDIRECT_ALL) == 0);
        assert(read_one_line_from_path(TEST_EXEC_FILE, &buf) == 0);
        assert(strneq(buf, "foobar7hello", 12));
}

int main(int argc, char *argv[]) {
        if (argc == 2)
                test_do_sleep(argc, argv);
        else if (argc == 3)
                test_do_write_file(argc, argv);

        test_do_fork_exec(argc, argv);
        test_do_fork_exec_redirect(argc, argv);

        return 0;
}

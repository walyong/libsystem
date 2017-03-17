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
#include <limits.h>
#include <time.h>

#include "libsystem/libsystem.h"

#define TEST_SRC_FILE "/tmp/test-cp-src"
#define TEST_DST_FILE "/tmp/test-cp-dst"

static int random_char(char **buf, size_t len) {
        static int rand_init = 0;
        char *b;
        int rnd;
        size_t s;

        if (!rand_init) {
                srand(time(NULL));
                rand_init = 1;
        }

        b = new0(char, len);
        if (!b)
                return -ENOMEM;

        for (s = 0; s < len; s++) {

                /* 98 = number_of_chars((sp) ~ (DEL) + \t + \n + \r) */
                rnd = rand() % 98;

                assert(rnd < 98);

                switch (rnd) {
                case 95:
                        b[s] = '\t';
                        break;
                case 96:
                        b[s] = '\n';
                        break;
                case 97:
                        b[s] = '\r';
                        break;
                default:
                        b[s] = ' ' + rnd;
                        break;
                }
        }

        *buf = b;

        return 0;
}

static int write_src_file(unsigned int n_byte) {
        _cleanup_free_ char *buf = NULL;
        _cleanup_close_ int fd = -1;

        assert(random_char(&buf, n_byte) == 0);

        fd = open(TEST_SRC_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        assert(fd >= 0);
        assert(write(fd, buf, n_byte) == n_byte);

        return 0;
}

static void compare_file(void) {
        _cleanup_close_ int src_fd = -1, dst_fd = -1;
        char src_buf[1024], dst_buf[1024];
        ssize_t src_red, dst_red;

        src_fd = open(TEST_SRC_FILE, O_RDONLY);
        assert(src_fd >= 0);

        dst_fd = open(TEST_DST_FILE, O_RDONLY);
        assert(dst_fd >= 0);

        while(src_red = read(src_fd, src_buf, 1024) > 0,
              dst_red = read(dst_fd, dst_buf, 1024) > 0) {
                assert(src_red == dst_red);
                assert(memcmp(src_buf, dst_buf, src_red) == 0);
        }
}

static void test_overwite(void) {
        assert(unlink(TEST_SRC_FILE) == 0 || errno == ENOENT);
        assert(unlink(TEST_DST_FILE) == 0 || errno == ENOENT);

        assert(touch(TEST_SRC_FILE) == 0);
        assert(touch(TEST_DST_FILE) == 0);

        assert(do_copy(TEST_SRC_FILE, TEST_DST_FILE) == -EALREADY);
        assert(do_copy_force(TEST_SRC_FILE, TEST_DST_FILE) == 0);
        compare_file();
}

static void test_n_byte_cp_force(unsigned int n) {
        assert(write_src_file(n) == 0);
        assert(do_copy_force(TEST_SRC_FILE, TEST_DST_FILE) == 0);
        compare_file();
}

int main(int argc, char *argv[]) {
        unsigned int b;

        test_overwite();

        for (b = 8; b < (1 << 30); b = b << 1)
                test_n_byte_cp_force(b);

        unlink(TEST_SRC_FILE);
        unlink(TEST_DST_FILE);

        return 0;
}

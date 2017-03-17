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
#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include "libsystem/libsystem.h"

#define TEST_READ_WRITE_FILE    "/tmp/test-read-write"
#define TEST_STRING             "L!i@b#s$y%s^t&e*m(T)e-s=tS`t_r+i|n~g"
#define TEST_INT32              0x7fabcdef
#define TEST_UINT32             0xffabcdef
#define TEST_INT64              0x7fabcdef00abcdef
#define TEST_UINT64             0xffabcdef00abcdef

static void test_string_read_write(void) {
        _cleanup_free_ char *str = NULL;

        assert(unlink(TEST_READ_WRITE_FILE) == 0 || errno == ENOENT);

        assert(write_str_to_path(TEST_READ_WRITE_FILE, TEST_STRING, 0) >= 0);
        assert(read_one_line_from_path(TEST_READ_WRITE_FILE, &str) >= 0);

        assert(streq(str, TEST_STRING));

        assert(unlink(TEST_READ_WRITE_FILE) == 0 || errno == ENOENT);
}

static void test_int32_read_write(void) {
        int32_t i;

        assert(unlink(TEST_READ_WRITE_FILE) == 0 || errno == ENOENT);

        assert(write_int32_to_path(TEST_READ_WRITE_FILE, TEST_INT32, 0) >= 0);
        assert(read_int32_from_path(TEST_READ_WRITE_FILE, &i) >= 0);

        assert(i == TEST_INT32);

        assert(unlink(TEST_READ_WRITE_FILE) == 0 || errno == ENOENT);
}

static void test_uint32_read_write(void) {
        uint32_t u;

        assert(unlink(TEST_READ_WRITE_FILE) == 0 || errno == ENOENT);

        assert(write_uint32_to_path(TEST_READ_WRITE_FILE, TEST_UINT32, 0) >= 0);
        assert(read_uint32_from_path(TEST_READ_WRITE_FILE, &u) >= 0);

        assert(u == TEST_UINT32);

        assert(unlink(TEST_READ_WRITE_FILE) == 0 || errno == ENOENT);
}

static void test_int64_read_write(void) {
        int64_t i;

        assert(unlink(TEST_READ_WRITE_FILE) == 0 || errno == ENOENT);

        assert(write_int64_to_path(TEST_READ_WRITE_FILE, TEST_INT64, 0) >= 0);
        assert(read_int64_from_path(TEST_READ_WRITE_FILE, &i) >= 0);

        assert(i == TEST_INT64);

        assert(unlink(TEST_READ_WRITE_FILE) == 0 || errno == ENOENT);
}

static void test_uint64_read_write(void) {
        uint64_t u;

        assert(unlink(TEST_READ_WRITE_FILE) == 0 || errno == ENOENT);

        assert(write_uint64_to_path(TEST_READ_WRITE_FILE, TEST_UINT64, 0) >= 0);
        assert(read_uint64_from_path(TEST_READ_WRITE_FILE, &u) >= 0);

        assert(u == TEST_UINT64);

        assert(unlink(TEST_READ_WRITE_FILE) == 0 || errno == ENOENT);
}

int main(int argc, char *argv[]) {
        test_string_read_write();
        test_int32_read_write();
        test_uint32_read_write();
        test_int64_read_write();
        test_uint64_read_write();

        return 0;
}

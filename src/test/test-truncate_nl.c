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
#include <assert.h>

#include "libsystem/libsystem.h"

#define TEST_STRING	"hello libsystem"
#define TEST_STRING_NL	TEST_STRING "\n\r"

static void test_truncate_nl(void) {
        char s[] = TEST_STRING_NL;

        assert(!streq(s, TEST_STRING));
        truncate_nl(s);
        assert(streq(s, TEST_STRING));
}

int main(int argc, char *argv[]) {
        test_truncate_nl();

        return 0;
}

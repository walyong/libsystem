/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/*
 * libsystem
 *
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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
#include <assert.h>
#include <errno.h>

#include "libsystem/libsystem.h"
#include "libsystem/proc.h"
#include "test.h"

static void test_get_pid_smaps(pid_t pid) {
        _cleanup_smaps_free_ struct smaps *maps = NULL;
        int r;

        r = proc_pid_get_smaps(pid, &maps, SMAPS_MASK_ALL);
        assert(r == 0);
}

int main(int argc, char *argv[]) {

        if (argc > 1) {
                pid_t pid = getpid();
                int i;

                for (i = 1; i < argc; i++) {
                        pid = (pid_t)atoi(argv[1]);
                        test_get_pid_smaps(pid);
                }

                return 0;
        }

        test_get_pid_smaps(getpid());

        return 0;
}

/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/*
 * time-util.c
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

#include "libsystem.h"

#define TIME_BUF_LEN    30

int sec_to_timestr(time_t sec, const char *format, char **time) {
        struct tm tm;
        char *buf;
        size_t l;

        assert(format);
        assert(time);

        localtime_r(&sec, &tm);

        buf = new0(char, TIME_BUF_LEN);
        if (!buf)
                return -ENOMEM;

        l = strftime(buf, TIME_BUF_LEN, format, &tm);
        if (l <= 0) {
                free(buf);
                return -EINVAL;
        }

        *time = buf;

        return 0;
}

int sec_to_timestr_full(time_t sec, char **time) {
        assert(time);

        return sec_to_timestr(sec, DOW_YYYY_MM_DD_HH_MM_SS_Z, time);
}

int timestr_to_sec(const char *format, const char *time, time_t *sec) {
        struct tm tm;
        char *ret;

        assert(format);
        assert(time);
        assert(sec);

        memset(&tm, 0, sizeof(struct tm));
        ret = strptime(time, format, &tm);
        if ((ret - time) < strlen(time))
                return -EINVAL;

        *sec = mktime(&tm);

        return 0;
}

void msec_to_timeval(uint64_t msec, struct timeval *tv) {
        assert(tv);

        tv->tv_sec = msec / MSEC_PER_SEC;
        tv->tv_usec = (msec % MSEC_PER_SEC) * USEC_PER_MSEC;
}

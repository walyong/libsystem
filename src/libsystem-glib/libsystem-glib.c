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

#include <errno.h>
#include <assert.h>
#include <glib.h>

guint g_new_msec_timer(GMainContext *context,
                       guint msec,
                       GSourceFunc func,
                       gpointer data,
                       GDestroyNotify notify) {
        g_autoptr(GSource) src = NULL;

        g_assert(func);

        src = g_timeout_source_new(msec);
        g_source_set_callback(src, func, data, notify);

        return g_source_attach(src, context);
}

guint g_new_sec_timer(GMainContext *context,
                      guint sec,
                      GSourceFunc func,
                      gpointer data,
                      GDestroyNotify notify) {
        g_autoptr(GSource) src = NULL;

        g_assert(func);

        src = g_timeout_source_new_seconds(sec);
        g_source_set_callback(src, func, data, notify);

        return g_source_attach(src, context);
}

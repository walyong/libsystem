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
#include <assert.h>
#include <limits.h>
#include <glib.h>

#include "libsystem/libsystem.h"
#include "libsystem/glib-util.h"

static void gen_test_list(GList **list) {
        GList *l = NULL;
        int i;

        assert(list);
        assert(!*list);

        for (i = 1; i <= 10; i++)
                l = g_list_append(l, GINT_TO_POINTER(i));

        *list = l;
}

static void test_foreach_g_list(void) {
        GList *list = NULL, *node;
        char buf[LINE_MAX];
        int n = 0;

        gen_test_list(&list);

        FOREACH_G_LIST(node, list) {
                int i = GPOINTER_TO_INT(node->data);

                n += snprintf(buf + n, LINE_MAX - n - 1, "%d ", i);
        }

        assert(streq(buf, "1 2 3 4 5 6 7 8 9 10 "));

        g_list_free(list);
}

static void test_foreach_g_list_reverse(void) {
        GList *list = NULL, *node;
        char buf[LINE_MAX];
        int n = 0;

        gen_test_list(&list);

        FOREACH_G_LIST_REVERSE(node, list) {
                int i = GPOINTER_TO_INT(node->data);

                n += snprintf(buf + n, LINE_MAX - n - 1, "%d ", i);
        }

        assert(streq(buf, "10 9 8 7 6 5 4 3 2 1 "));

        g_list_free(list);
}

static void test_foreach_g_list_safe(void) {
        GList *list = NULL, *node, *next;
        char buf[LINE_MAX];
        int n = 0;

        gen_test_list(&list);

        FOREACH_G_LIST_SAFE(node, next, list) {
                int i = GPOINTER_TO_INT(node->data);

                if (i % 2)
                        list = g_list_remove_link(list, node);
        }

        FOREACH_G_LIST(node, list) {
                int i = GPOINTER_TO_INT(node->data);

                n += snprintf(buf + n, LINE_MAX - n - 1, "%d ", i);
        }

        assert(streq(buf, "2 4 6 8 10 "));

        g_list_free(list);
}

static void test_foreach_g_list_safe_reverse(void) {
        GList *list = NULL, *node, *prev;
        char buf[LINE_MAX];
        int n = 0;

        gen_test_list(&list);

        FOREACH_G_LIST_SAFE_REVERSE(node, prev, list) {
                int i = GPOINTER_TO_INT(node->data);

                if (i % 2)
                        list = g_list_remove_link(list, node);
        }

        FOREACH_G_LIST_REVERSE(node, list) {
                int i = GPOINTER_TO_INT(node->data);

                n += snprintf(buf + n, LINE_MAX - n - 1, "%d ", i);
        }

        assert(streq(buf, "10 8 6 4 2 "));

        g_list_free(list);
}

int main(int argc, char *argv[]) {

        test_foreach_g_list();
        test_foreach_g_list_reverse();

        test_foreach_g_list_safe();
        test_foreach_g_list_safe_reverse();

        return 0;
}

/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/*
 * libsystem
 *
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
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
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>

#include "libsystem.h"
#include "config-parser.h"

#define MAX_SECTION     64

static int config_table_lookup(
                void *table,
                const char *section,
                const char *lvalue,
                ConfigParserCallback *func,
                int *ltype,
                void **data) {

        ConfigTableItem *t;

        assert(table);
        assert(lvalue);
        assert(func);
        assert(ltype);
        assert(data);

        for (t = table; t->lvalue; t++) {

                if (!streq(lvalue, t->lvalue))
                        continue;

                if (!streq_ptr(section, t->section))
                        continue;

                *func = t->cb;
                *ltype = t->ltype;
                *data = t->data;
                return 1;
        }

        return 0;
}

/* Run the user supplied parser for an assignment */
static int config_parse_table(
                const char *filename,
                unsigned line,
                void *table,
                const char *section,
                const char *lvalue,
                const char *rvalue) {

        ConfigParserCallback cb = NULL;
        int ltype = 0;
        void *data = NULL;
        int r;

        assert(filename);
        assert(section);
        assert(lvalue);
        assert(rvalue);

        r = config_table_lookup(table,
                                section,
                                lvalue,
                                &cb,
                                &ltype,
                                &data);
        if (r <= 0)
                return r;

        if (cb)
                return cb(filename,
                          line,
                          section,
                          lvalue,
                          ltype,
                          rvalue,
                          data);

        return 0;
}

int config_parse(const char *filename, void *table) {

        _cleanup_fclose_ FILE *f = NULL;
        char *sections[MAX_SECTION] = { 0 };
        char *section = NULL, *n, *e, l[LINE_MAX];
        size_t len;
        int i, r, num_section = 0;
        bool already;
        unsigned line = 0;

        assert(filename);

        f = fopen(filename, "r");
        if (!f)
                return -errno;

        while (!feof(f)) {
                _cleanup_free_ char *lvalue = NULL, *rvalue = NULL;

                if (fgets(l, LINE_MAX, f) == NULL) {
                        if (feof(f))
                                break;

                        r = -errno;
                        goto finish;
                }

                line++;
                truncate_nl(l);

                if (strchr(COMMENTS NEWLINE, *l))
                        continue;

                if (*l == '[') {
                        len = strlen(l);
                        if (l[len - 1] != ']') {
                                r = -EBADMSG;
                                goto finish;
                        }

                        n = strndup(l + 1, len - 2);
                        if (!n) {
                                r = -ENOMEM;
                                goto finish;
                        }

                        already = false;
                        for (i = 0; i < num_section; i++) {
                                if (streq(n, sections[i])) {
                                        section = sections[i];
                                        already = true;
                                        free(n);
                                        break;
                                }
                        }

                        if (already)
                                continue;

                        section = n;
                        sections[num_section] = n;
                        num_section++;
                        if (num_section > MAX_SECTION) {
                                r = -EOVERFLOW;
                                goto finish;
                        }

                        continue;
                }

                if (!section)
                        continue;

                e = strchr(l, '=');
                if (e == NULL)
                        continue;

                r = strndup_strip(l, e - l, &lvalue);
                if (r < 0)
                        goto finish;

                r = strdup_strip(e + 1, &rvalue);
                if (r < 0)
                        goto finish;

                r = config_parse_table(filename,
                                       line,
                                       table,
                                       section,
                                       lvalue,
                                       rvalue);
                if (r < 0)
                        goto finish;
        }

        r = 0;

finish:
        for (i = 0; i < num_section; i++)
                if (sections[i])
                        free(sections[i]);

        return r;
}

int config_parse_dir(const char *dir, ConfigParseFunc fp, void *data) {
        _cleanup_closedir_ DIR *d = NULL;
        struct dirent *de;

        d = opendir(dir);
        if (!d)
                return errno;

        FOREACH_DIRENT(de, d, return -errno) {
                _cleanup_free_ char *path = NULL;
                int r;

                if (de->d_type != DT_REG)
                        continue;

                r = asprintf(&path, "%s/%s", dir, de->d_name);
                if (r < 0)
                        return -ENOMEM;

                /* Do not just break loop until parse all file of
                 * dir. ignore return */
                (void) fp(path, data);
        }

        return 0;
}

int config_parse_int(
                const char *filename,
                unsigned line,
                const char *section,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data) {

        int *i = data;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        if (!is_number(rvalue, strlen(rvalue)))
                return -EINVAL;

        *i = atoi(rvalue);

        return 0;
}

int config_parse_bool(
                const char *filename,
                unsigned line,
                const char *section,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data) {

        int k;
        bool *b = data;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        k = parse_boolean(rvalue);
        if (k < 0)
                return 0;

        *b = !!k;
        return 0;
}

int config_parse_string(
                const char *filename,
                unsigned line,
                const char *section,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data) {

        char **s = data, *n;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        if (isempty(rvalue))
                n = NULL;
        else {
                n = strdup(rvalue);
                if (!n)
                        return -ENOMEM;
        }

        free(*s);
        *s = n;

        return 0;
}

int config_parse_bytes(
                const char *filename,
                unsigned line,
                const char *section,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data) {

        size_t *ss = data;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        *ss = 0;

        if (isempty(rvalue))
                return 0;

        return parse_bytes(rvalue, ss);
}

int config_parse_percent(
                const char *filename,
                unsigned line,
                const char *section,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data) {

        size_t *percent = data, p = 0;
        int r;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        if (!isempty(rvalue)) {
                r = parse_percent(rvalue, &p);
                if (r < 0)
                        return r;
        }

        *percent = p;

        return 0;
}

int config_parse_strv(const char *filename,
                unsigned line,
                const char *section,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data)
{
        char ***strv = data;
        char **o = NULL, **v = NULL, **vv = NULL;
        int r;

        assert(filename);
        assert(lvalue);
        assert(rvalue);
        assert(data);

        if (isempty(rvalue))
                return 0;

        r = str_to_strv(rvalue, &v, WHITESPACE);
        if (r < 0)
                return r;

        o = *strv;

        r = strv_attach(o, v, &vv, true);
        if (r < 0)
                return r;

        *strv = vv;

        return 0;
}

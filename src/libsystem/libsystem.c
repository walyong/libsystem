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

#include <stddef.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mntent.h>
#include <stdint.h>
#include <inttypes.h>

#include "libsystem.h"

static int _errno_old;

#define STORE_RESET_ERRNO do {          \
        _errno_old = errno;             \
        errno = 0;                      \
} while (0)

#define RESTORE_ERRNO do {              \
        errno = _errno_old;             \
        _errno_old = 0;                 \
} while (0)

bool streq_ptr(const char *a, const char *b) {

        /* Like streq(), but tries to make sense of NULL pointers */

        if (a && b)
                return streq(a, b);

        if (!a && !b)
                return true;

        return false;
}

char *truncate_nl(char *s) {
        assert(s);

        s[strcspn(s, NEWLINE)] = 0;

        return s;
}

char *strnappend(const char *s, const char *suffix, size_t b) {
        size_t a;
        char *r;

        if (!s && !suffix)
                return strdup("");

        if (!s)
                return strndup(suffix, b);

        if (!suffix)
                return strdup(s);

        assert(s);
        assert(suffix);

        a = strlen(s);
        if (b > ((size_t) - 1) - a)
                return NULL;

        r = new(char, a + b + 1);
        if (!r)
                return NULL;

        memcpy(r, s, a);
        memcpy(r + a, suffix, b);
        r[a + b] = 0;

        return r;
}

char *strappend(const char *s, const char *suffix) {
        return strnappend(s, suffix, suffix ? strlen(suffix) : 0);
}

char *strstrip(char *s) {
        char *e;

        /* Drops trailing whitespace. Modifies the string in
         * place. Returns pointer to first non-space character */

        s += strspn(s, WHITESPACE);

        for (e = strchr(s, 0); e > s; e--)
                if (!strchr(WHITESPACE, e[-1]))
                        break;

        *e = 0;

        return s;
}

int strdup_strip(const char *str, char **ret) {
        char *r = NULL;
        size_t s, l;

        assert(str);
        assert(ret);

        s = strspn(str, WHITESPACE);

        for (l = strlen(str + s); l > 0; l--)
                if (!strchr(WHITESPACE, str[s + l - 1]))
                        break;

        r = strndup(str + s, l);
        if (!r)
                return -ENOMEM;

        *ret = r;

        return 0;
}

int strndup_strip(const char *str, size_t len, char **ret) {
        char *r = NULL;
        size_t s, l;

        assert(str);
        assert(ret);

        s = strspn(str, WHITESPACE);

        l = strlen(str + s);
        if (len > s)
                l = l < len - s ? l : len - s;
        else
                return -EFAULT;

        for (; l > 0; l--)
                if (!strchr(WHITESPACE, str[s + l - 1]))
                        break;

        r = strndup(str + s, l);
        if (!r)
                return -ENOMEM;

        *ret = r;

        return 0;
}

bool nulstr_contains(const char *nulstr, const char *needle) {
        const char *i;

        if (!nulstr)
                return false;

        NULSTR_FOREACH(i, nulstr)
                if (streq(i, needle))
                        return true;

        return false;
}

bool path_is_absolute(const char *p) {

        assert(p);

        return p[0] == '/';
}

char *path_kill_slashes(char *path) {
        char *f, *t;
        bool slash = false;

        /* Removes redundant inner and trailing slashes. Modifies the
         * passed string in-place.
         *
         * ///foo///bar/ becomes /foo/bar
         */

        for (f = path, t = path; *f; f++) {

                if (*f == '/') {
                        slash = true;
                        continue;
                }

                if (slash) {
                        slash = false;
                        *(t++) = '/';
                }

                *(t++) = *f;
        }

        /* Special rule, if we are talking of the root directory, a
           trailing slash is good */

        if (t == path && slash)
                *(t++) = '/';

        *t = 0;
        return path;
}

char *endswith(const char *s, const char *postfix) {
        size_t sl, pl;

        assert(s);
        assert(postfix);

        sl = strlen(s);
        pl = strlen(postfix);

        if (pl == 0)
                return (char *) s + sl;

        if (sl < pl)
                return NULL;

        if (memcmp(s + sl - pl, postfix, pl) != 0)
                return NULL;

        return (char *) s + sl - pl;
}

int parse_boolean(const char *v) {
        assert(v);

        if (streq(v, "1") || v[0] == 'y' || v[0] == 'Y' || v[0] == 't' || v[0] == 'T' || strcaseeq(v, "on"))
                return 1;
        else if (streq(v, "0") || v[0] == 'n' || v[0] == 'N' || v[0] == 'f' || v[0] == 'F' || strcaseeq(v, "off"))
                return 0;

        return -EINVAL;
}

int parse_bytes(const char *b, size_t *s) {
        _cleanup_free_ char *num = NULL;
        size_t len, num_l, unit_l;

        assert(b);

        len = strlen(b);

        if (!len)
                return 0;

        num_l = strspn(b, "0123456789");
        if (num_l < len - 1)
                return -EINVAL;

        unit_l = strcspn(b, "BKMG");
        if (num_l != unit_l)
                return -EINVAL;

        num = strndup(b, num_l);
        if (!num)
                return -ENOMEM;

        switch (b[len - 1]) {
        case 'G':
                *s = atoi(num) << 30;
                break;
        case 'M':
                *s = atoi(num) << 20;
                break;
        case 'K':
                *s = atoi(num) << 10;
                break;
        case 'B':
        default:
                *s = atoi(num);
                break;
        }

        return 0;
}

int parse_percent(const char *string, size_t *percent) {
        _cleanup_free_ char *num = NULL;
        size_t len, num_len, per;

        assert(string);
        assert(percent);

        len = strlen(string);
        if (!len)
                return 0;

        if (string[len - 1] != '%')
                return -EINVAL;

        num_len = strspn(string, "0123456789");
        if (num_len < len - 1)
                return -EINVAL;

        num = strndup(string, num_len);
        if (!num)
                return -ENOMEM;

        per = atoi(num);
        if (per > 100)
                return -EINVAL;

        *percent = per;

        return 0;
}

static bool __quote_complete(char *str, size_t l, char q) {
        char *s, *s2;

        assert(str);

        if (!l)
                return true;

        s = strchr(str, q);
        if (!s || (s - str) > l)
                return true;

        s = strchr(s + 1, q);
        if (!s || (s - str) > l)
                return false;

        s2 = strchr(s + 1, q);
        if (!s2 || (s2 - str) > l)
                return true;

        return __quote_complete(s + 1, l - (s + 1 - str), q);
}

static bool quote_complete(char *str, size_t l) {
        char quotes[] = QUOTES;
        int i;

        assert(str);

        if (!l)
                return true;

        for (i = 0; quotes[i]; i++) {
                if (!__quote_complete(str, l, quotes[i]))
                        return false;
        }

        return true;
}

char *split(const char *c, size_t *l, const char *separator, char **state) {
        bool separator_include_quotes;
        char *current;
        size_t s;

        assert(c);
        assert(l);
        assert(separator);
        assert(state);

        current = *state ? *state : (char *) c;
        if (!*current || *c == 0)
                return NULL;

        *l = 0;
        separator_include_quotes = !!strspn(separator, QUOTES);
        current += strspn(current, separator);

        while ((s = strcspn(current + *l, separator))) {
                *l += s;
                if (separator_include_quotes ||
                    quote_complete(current, *l))
                        break;
                (*l)++;
        }

        *state = current + *l;

        return (char *) current;
}

bool is_number(const char *s, int l) {
        int i;

        for (i = 0; i < l; i++)
                if (!isdigit(s[i]))
                        return false;

        return true;
}

static int do_copy_internal(const char *src, const char *dst, mode_t mode, bool force) {
        _cleanup_close_ int rfd = -1, wfd = -1;
        char buf[1024];
        ssize_t red;
        int r;

        assert(src);
        assert(dst);

        if (!force) {
                r = access(dst, F_OK);
                if (r == 0)
                        return -EALREADY;
                else if (errno != ENOENT)
                        return -errno;
        }

        wfd = open(dst, O_CREAT | O_WRONLY | O_TRUNC, mode);
        if (wfd < 0)
                return -errno;

        rfd = open(src, O_RDONLY);
        if (rfd < 0)
                return -errno;

        while ((red = read(rfd, buf, 1024)) > 0)
                if (write(wfd, buf, red) != red)
                        return -errno;

        if (red < 0)
                return -errno;

        return 0;
}

int do_copy_mode(const char *src, const char *dst, mode_t mode) {

        assert(src);
        assert(dst);

        return do_copy_internal(src, dst, mode, false);
}

int do_copy_mode_force(const char *src, const char *dst, mode_t mode) {

        assert(src);
        assert(dst);

        return do_copy_internal(src, dst, mode, true);
}

int do_copy(const char *src, const char *dst) {

        assert(src);
        assert(dst);

        return do_copy_internal(src, dst, 0644, false);
}

int do_copy_force(const char *src, const char *dst) {

        assert(src);
        assert(dst);

        return do_copy_internal(src, dst, 0644, true);
}

int do_mkdir(const char *path, mode_t mode) {
        char d[PATH_MAX];
        size_t s, l;
        int r, p;

        assert(path);

        l = strlen(path);

        for (p = 0, s = 0; p < l; p += s + 1) {
                s = strcspn(path + p, "/");
                if (!s)
                        continue;

                assert(PATH_MAX > p + s + 1);

                r = snprintf(d, p + s + 1, "%s", path);
                if (r < 0)
                        return r;

                r = mkdir(d, mode);
                if (r < 0 && errno != EEXIST)
                        return -errno;
        }

        return 0;
}

int rmdir_recursive(const char *path) {
        _cleanup_closedir_ DIR *d = NULL;
        struct dirent *de;
        int r;

        assert(path);

        d = opendir(path);
        if (!d)
                return -errno;

        FOREACH_DIRENT(de, d, return -errno) {
                _cleanup_free_ char *p = NULL;

                r = asprintf(&p, "%s/%s", path, de->d_name);
                if (r < 0)
                        return -ENOMEM;

                if (de->d_type == DT_DIR) {
                        r = rmdir_recursive(p);
                        if (r < 0)
                                return r;
                } else {
                        r = unlink(p);
                        if (r < 0)
                                return r;
                }
        }

        return rmdir(path);
}

char *strdup_unquote(const char *str, const char *quotes) {
        size_t l;

        assert(str);

        l = strlen(str);
        if (l < 2)
                return strdup(str);

        if (strchr(quotes, str[0]) && str[0] == str[l - 1])
                return strndup(str + 1, l - 2);

        return strdup(str);
}

int write_str_to_file(FILE *f, const char *str, enum file_write_flags flags) {
        int r = 0;

        assert(f);
        assert(str);

        STORE_RESET_ERRNO;

        (void) fputs(str, f);
        if ((flags & FILE_WRITE_NEWLINE_IF_NOT) && !endswith(str, "\n"))
                (void) fputc('\n', f);

        if (flags & FILE_WRITE_WITH_FFLUSH)
                (void) fflush(f);

        if (ferror(f))
                r = errno ? -errno : -EIO;

        RESTORE_ERRNO;

        return r;
}

int write_str_to_path(const char *path, const char *str, enum file_write_flags flags) {
        _cleanup_fclose_ FILE *f = NULL;

        assert(path);
        assert(str);

        if (flags & FILE_WRITE_APPEND)
                f = fopen(path, "ae");
        else
                f = fopen(path, "we");
        if (!f)
                return -errno;

        return write_str_to_file(f, str, flags);
}

int read_one_line_from_file(FILE *f, char **line) {
        char t[LINE_MAX], *c;

        assert(f);
        assert(line);

        STORE_RESET_ERRNO;

        if (!fgets(t, sizeof(t), f)) {

                if (ferror(f)) {
                        int r;

                        r = errno ? -errno : -EIO;
                        RESTORE_ERRNO;
                        return r;
                }

                t[0] = 0;
        }

        RESTORE_ERRNO;

        c = strdup(t);
        if (!c)
                return -ENOMEM;

        *line = truncate_nl(c);

        return 0;
}

int read_one_line_from_path(const char *path, char **line) {
        _cleanup_fclose_ FILE *f = NULL;

        assert(path);
        assert(line);

        f = fopen(path, "re");
        if (!f)
                return -errno;

        return read_one_line_from_file(f, line);
}

#define DEFINE_WRITE_NUM_TO_FILE(type, format)                          \
        int write_##type##_to_file(FILE *f,                             \
                                   type##_t u,                          \
                                   enum file_write_flags flags) {       \
                int r = 0;                                              \
                                                                        \
                assert(f);                                              \
                                                                        \
                STORE_RESET_ERRNO;                                      \
                                                                        \
                (void) fprintf(f, format, u);                           \
                if (flags & FILE_WRITE_NEWLINE_IF_NOT)                  \
                        (void) fputc('\n', f);                          \
                                                                        \
                if (flags & FILE_WRITE_WITH_FFLUSH)                     \
                        (void) fflush(f);                               \
                                                                        \
                if (ferror(f))                                          \
                        r = errno ? -errno : -EIO;                      \
                                                                        \
                RESTORE_ERRNO;                                          \
                                                                        \
                return r;                                               \
        }

#define DEFINE_WRITE_NUM_TO_PATH(type)                                  \
        int write_##type##_to_path(const char *path,                    \
                                   type##_t u,                          \
                                   enum file_write_flags flags) {       \
                _cleanup_fclose_ FILE *f = NULL;                        \
                                                                        \
                assert(path);                                           \
                                                                        \
                if (flags & FILE_WRITE_APPEND)                          \
                        f = fopen(path, "ae");                          \
                else                                                    \
                        f = fopen(path, "we");                          \
                if (!f)                                                 \
                        return -errno;                                  \
                                                                        \
                return write_##type##_to_file(f, u, flags);             \
        }

#define DEFINE_WRITE_NUM_DUAL(type, format)     \
        DEFINE_WRITE_NUM_TO_FILE(type, format); \
        DEFINE_WRITE_NUM_TO_PATH(type)

#define DEFINE_READ_NUM_FROM_FILE(type, format)                 \
        int read_##type##_from_file(FILE *f, type##_t *num) {   \
                int r = 0;                                      \
                                                                \
                assert(f);                                      \
                assert(num);                                    \
                                                                \
                STORE_RESET_ERRNO;                              \
                                                                \
                r = fscanf(f, format, num);                     \
                if (r == EOF && ferror(f))                      \
                        r = errno ? -errno : -EOF;              \
                                                                \
                RESTORE_ERRNO;                                  \
                                                                \
                return r;                                       \
        }

#define DEFINE_READ_NUM_FROM_PATH(type)                                 \
        int read_##type##_from_path(const char *path, type##_t *num) {  \
                _cleanup_fclose_ FILE *f = NULL;                        \
                                                                        \
                assert(path);                                           \
                assert(num);                                            \
                                                                        \
                f = fopen(path, "re");                                  \
                if (!f)                                                 \
                        return -errno;                                  \
                                                                        \
                return read_##type##_from_file(f, num);                 \
        }

#define DEFINE_READ_NUM_DUAL(type, format)              \
        DEFINE_READ_NUM_FROM_FILE(type, format);        \
        DEFINE_READ_NUM_FROM_PATH(type)

#define DEFINE_READ_WRITE_NUM_DUAL(type, r_format, w_format)    \
        DEFINE_READ_NUM_DUAL(type, r_format);                   \
        DEFINE_WRITE_NUM_DUAL(type, w_format)

DEFINE_READ_WRITE_NUM_DUAL(int32, "%d", "%d");
DEFINE_READ_WRITE_NUM_DUAL(uint32, "%u", "%u");
DEFINE_READ_WRITE_NUM_DUAL(int64, "%" SCNd64, "%" PRId64);
DEFINE_READ_WRITE_NUM_DUAL(uint64, "%" SCNu64, "%" PRIu64);

int write_int_to_file(FILE *f, int num, enum file_write_flags flags) {

        assert(f);

        return write_int32_to_file(f, (int32_t) num, flags);
}

int write_int_to_path(const char *path, int num, enum file_write_flags flags) {

        assert(path);

        return write_int32_to_path(path, (int32_t) num, flags);
}

int write_unsigned_int_to_file(FILE *f, unsigned int num, enum file_write_flags flags) {

        assert(f);

        return write_uint32_to_file(f, (uint32_t) num, flags);
}

int write_unsigned_int_to_path(const char *path, unsigned int num, enum file_write_flags flags) {

        assert(path);

        return write_uint32_to_path(path, (uint32_t) num, flags);
}

int write_long_int_to_file(FILE *f, long int num, enum file_write_flags flags) {

        assert(f);

#if __WORDSIZE == 64
        return write_int64_to_file(f, (int64_t) num, flags);
#else
        return write_int32_to_file(f, (int32_t) num, flags);
#endif
}

int write_long_int_to_path(const char *path, long int num, enum file_write_flags flags) {

        assert(path);

#if __WORDSIZE == 64
        return write_int64_to_path(path, (int64_t) num, flags);
#else
        return write_int32_to_path(path, (int32_t) num, flags);
#endif
}

int write_unsigned_long_int_to_file(FILE *f, unsigned long int num, enum file_write_flags flags) {

        assert(f);

#if __WORDSIZE == 64
        return write_uint64_to_file(f, (uint64_t) num, flags);
#else
        return write_uint32_to_file(f, (uint32_t) num, flags);
#endif
}

int write_unsigned_long_int_to_path(const char *path, unsigned long int num, enum file_write_flags flags) {

        assert(path);

#if __WORDSIZE == 64
        return write_uint64_to_path(path, (uint64_t) num, flags);
#else
        return write_uint32_to_path(path, (uint32_t) num, flags);
#endif
}

int write_long_long_int_to_file(FILE *f, long long int num, enum file_write_flags flags) {

        assert(f);

        return write_int64_to_file(f, (int64_t) num, flags);
}

int write_long_long_int_to_path(const char *path, long long int num, enum file_write_flags flags) {

        assert(path);

        return write_int64_to_path(path, (int64_t) num, flags);
}

int write_unsigned_long_long_int_to_file(FILE *f, unsigned long long int num, enum file_write_flags flags) {

        assert(f);

        return write_uint64_to_file(f, (uint64_t) num, flags);
}

int write_unsigned_long_long_int_to_path(const char *path, unsigned long long int num, enum file_write_flags flags) {

        assert(path);

        return write_uint64_to_path(path, (uint64_t) num, flags);
}

int read_int_from_file(FILE *f, int *num) {

        assert(f);
        assert(num);

        return read_int32_from_file(f, (int32_t *) num);
}

int read_int_from_path(const char *path, int *num) {

        assert(path);
        assert(num);

        return read_int32_from_path(path, (int32_t *) num);
}

int read_unsigned_int_from_file(FILE *f, unsigned int *num) {

        assert(f);
        assert(num);

        return read_uint32_from_file(f, (uint32_t *) num);
}

int read_unsigned_int_from_path(const char *path, unsigned int *num) {

        assert(path);
        assert(num);

        return read_uint32_from_path(path, (uint32_t *) num);
}

int read_long_int_from_file(FILE *f, long int *num) {

        assert(f);
        assert(num);

#if __WORDSIZE == 64
        return read_int64_from_file(f, (int64_t *) num);
#else
        return read_int32_from_file(f, (int32_t *) num);
#endif
}

int read_long_int_from_path(const char *path, long int *num) {

        assert(path);
        assert(num);

#if __WORDSIZE == 64
        return read_int64_from_path(path, (int64_t *) num);
#else
        return read_int32_from_path(path, (int32_t *) num);
#endif
}

int read_unsigned_long_int_from_file(FILE *f, unsigned long int *num) {

        assert(f);
        assert(num);

#if __WORDSIZE == 64
        return read_uint64_from_file(f, (uint64_t *) num);
#else
        return read_uint32_from_file(f, (uint32_t *) num);
#endif
}

int read_unsigned_long_int_from_path(const char *path, unsigned long int *num) {

        assert(path);
        assert(num);

#if __WORDSIZE == 64
        return read_uint64_from_path(path, (uint64_t *) num);
#else
        return read_uint32_from_path(path, (uint32_t *) num);
#endif
}

int read_long_long_int_from_file(FILE *f, long long *num) {

        assert(f);
        assert(num);

        return read_int64_from_file(f, (int64_t *) num);
}

int read_long_long_int_from_path(const char *path, long long *num) {

        assert(path);
        assert(num);

        return read_int64_from_path(path, (int64_t *) num);
}

int read_unsigned_long_long_int_from_file(FILE *f, unsigned long long *num) {

        assert(f);
        assert(num);

        return read_uint64_from_file(f, (uint64_t *) num);
}

int read_unsigned_long_long_int_from_path(const char *path, unsigned long long *num) {

        assert(path);
        assert(num);

        return read_uint64_from_path(path, (uint64_t *) num);
}

int str_to_strv(const char *str, char ***strv, const char *separator) {
        char *w, *state, *p;
        char **v = NULL, **new = NULL;
        size_t l;
        size_t i = 0;

        FOREACH_WORD_SEPARATOR(w, l, str, separator, state) {
                p = strndup(w, l);
                if (!p) {
                        free(v);
                        return -ENOMEM;
                }

                new = (char **) realloc(v, sizeof(char *) * (i + 2));
                if (!new) {
                        free(p);
                        free(v);
                        p = NULL;
                        return -ENOMEM;
                }

                v = new;

                v[i] = p;
                v[i + 1] = NULL;
                i++;
        }

        *strv = v;

        return 0;
}

size_t sizeof_strv(char **strv) {
        size_t u = 0;

        if (!strv)
                return 0;

        while(strv[u++])
                ;

        return u - 1;
}

int strv_attach(char **first, char **second, char ***strv, bool free_second) {
        char **new = NULL;
        size_t n1 = 0, n2 = 0;

        assert(strv);

        if (first)
                n1 = sizeof_strv(first);

        if (second) {
                n2 = sizeof_strv(second);

                new = (char **) realloc(first, sizeof(char *) * (n1 + n2 + 1));
                if (!new)
                        return -ENOMEM;

                first = new;

                memcpy(first + n1, second, sizeof(char *) * (n2 + 1));

                if (free_second)
                        free(second);
        }

        *strv = first;

        return 0;
}

void strv_free_full(char **strv) {
        char **s;

        if (!strv)
                return;

        FOREACH_STRV(s, strv) {
                if (s && *s) {
                        free(*s);
                        *s = NULL;
                }
        }

        free(strv);
        strv = NULL;
}

bool isdir(const char *path) {
        struct stat st;

        assert(path);

        if (lstat(path, &st) < 0)
                return false;

        return S_ISDIR(st.st_mode);
}

int touch(const char *path) {
        _cleanup_fclose_ FILE *f = NULL;

        assert(path);

        f = fopen(path, "w");
        if (!f)
                return -errno;

        return 0;
}

bool mnt_is_mounted(const char *fsname, const char *dir, const char *type, const char *opts) {
        struct mntent *ent;
        FILE *f = NULL;
        bool matched = false;

        f = setmntent("/etc/mtab", "r");
        if (!f)
                return false;

        while ((ent = getmntent(f))) {
                if (fsname) {
                        if (streq(fsname, ent->mnt_fsname))
                                matched = true;
                        else
                                goto no_match;
                }

                if (dir) {
                        if (streq(dir, ent->mnt_dir))
                                matched = true;
                        else
                                goto no_match;
                }

                if (type) {
                        if (streq(type, ent->mnt_type))
                                matched = true;
                        else
                                goto no_match;
                }

                if (opts) {
                        if (streq(opts, ent->mnt_opts))
                                matched = true;
                        else
                                goto no_match;
                }

                if (matched)
                        break;

        no_match:
                matched = false;
        }

        endmntent(f);

        return matched;
}

bool is_float(const char *s) {
        char *endptr;

        strtof(s, &endptr);
        if ((*endptr == '\0') || (isspace(*endptr) != 0))
                return true;

        return false;
}

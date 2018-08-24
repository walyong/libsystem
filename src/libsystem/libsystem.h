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

/**
 * @file libsystem.h
 *
 * system utility library
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#include <time.h>
#include <sys/resource.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Many functions have no effects except the return value and their
 * return value depends only on the parameters and/or global
 * variables. Such a function can be subject to common subexpression
 * elimination and loop optimization just as an arithmetic operator
 * would be. These functions should be declared with the attribute
 * pure.
 */
#define _pure_ __attribute__ ((pure))

/**
 * The cleanup attribute runs a function when the variable goes out of
 * scope. This attribute can only be applied to auto function scope
 * variables; it may not be applied to parameters or variables with
 * static storage duration. The function must take one parameter, a
 * pointer to a type compatible with the variable. The return value of
 * the function (if any) is ignored.
 */
#define _cleanup_(x) __attribute__((cleanup(x)))

/**
 * whitespaces such like space, tab or newlines
 */
#define WHITESPACE " \t\n\r"

/**
 * newlines
 */
#define NEWLINE "\n\r"

/**
 * single or double quotes
 */
#define QUOTES "\"\'"

/**
 * comment start specifiers such like sharp(#) or semicolon(;)
 */
#define COMMENTS "#;"

/**
 * @defgroup GCC_CLEANUP_ATT_GROUP gcc cleanup attribute
 *
 * @{
 */

static inline void __cleanup_free_func(void *p) {
        free(*(void**) p);
}

static inline void __cleanup_close_func(int *fd) {
        if (*fd >= 0)
                close(*fd);
}

static inline void __cleanup_fclose_func(FILE **f) {
        if (*f)
                fclose(*f);
}

static inline void __cleanup_pclose_func(FILE **f) {
        if (*f)
                pclose(*f);
}

static inline void __cleanup_closedir_func(DIR **d) {
        if (*d)
                closedir(*d);
}

static inline const char *startswith(const char *s, const char *prefix) {
        if (strncmp(s, prefix, strlen(prefix)) == 0)
                return s + strlen(prefix);
        return NULL;
}

static inline bool isempty(const char *p) {
        return !p || !p[0];
}

/**
 * Declare value with cleanup attribute. free() is called when is
 * going out the scope.
 */
#define _cleanup_free_ _cleanup_(__cleanup_free_func)

/**
 * Declare value with cleanup attribute. close() is called when is
 * going out the scope.
 */
#define _cleanup_close_ _cleanup_(__cleanup_close_func)

/**
 * Declare value with cleanup attribute. fclose() is called when is
 * going out the scope.
 */
#define _cleanup_fclose_ _cleanup_(__cleanup_fclose_func)

/**
 * Declare value with cleanup attribute. pclose() is called when is
 * going out the scope.
 */
#define _cleanup_pclose_ _cleanup_(__cleanup_pclose_func)

/**
 * Declare value with cleanup attribute. closedir() is called when is
 * going out the scope.
 */
#define _cleanup_closedir_ _cleanup_(__cleanup_closedir_func)
/**
 * @}
 */

/**
 * Allocate n number of size t memory.
 */
#define new(t, n) ((t*) malloc(sizeof(t) * (n)))

/**
 * Allocate n number of size t memory. And initialize to 0 all.
 */
#define new0(t, n) ((t*) calloc((n), sizeof(t)))

/**
 * Allocate n number memory.
 */
#define malloc0(n) (calloc((n), 1))

/**
 * @brief Parse boolean type string.
 *
 * @param v String to parse.
 *
 * @return TRUE on "1", 'y', 'Y', 't', 'T' and "on". FALSE on "0",
 * 'n', 'N', 'f', 'F', "off".
 */
int parse_boolean(const char *v) _pure_;

/**
 * @brief Parse byte type string.
 *
 * @param b Byte string. This can be only digit number with byte unit
 * "BKMG". B is byte, K is kilo byte, M is mega byte and G is gira
 * byte. Byte is default.
 * @param s Parsed byte size is filled.
 *
 * @return 0 on success, -errno on failure.
 */
int parse_bytes(const char *b, size_t *s) _pure_;

/**
 * @brief Parse percentage type string.
 *
 * @param string Percentage string to parse. Such like "70%".
 * @param percent Parsed percentage size is filled.
 *
 * @return 0 on success, -errno on failure.
 */
int parse_percent(const char *string, size_t *percent) _pure_;

/**
 * @brief Parse "YYYY-MM-DD hh:mm:ss" formatted time string to struct
 * tm.
 *
 * @param time_string "YYYY-MM-DD hh:mm:ss" formatted string
 * @param time parsed struct tm.
 *
 * @return 0 on success, -errno on failure.
 */
int parse_time(const char *time_string, struct tm *time) _pure_;

/**
 * @brief check the path string is started with '/'
 *
 * @param p a path to check
 *
 * @return true if p started with '/', otherwise false.
 */
bool path_is_absolute(const char *p);

/**
 * @brief Removes redundant inner and trailing slashes. Modifies the
 * passed string in-place. For example, if "///foo//bar/" is given
 * then the path will be changed as "/foo/bar"
 *
 * @param path a path to modify.
 *
 * @return modified path pointer. It maybe identical with given path.
 */
char *path_kill_slashes(char *path);

/**
 * Get element number of array.
 */
#define ELEMENTSOF(x) (sizeof(x)/sizeof((x)[0]))

/**
 * Iterate for each struct reference.
 */
#define FOREACH_STRUCT_REF(s, f, i)                  \
        for ((i) = 0; s[(i)].f != NULL; (i)++)

/**
 * @brief Iterate for each directory entries exclude "." and "..".
 */
#define FOREACH_DIRENT(de, d, on_error)                                 \
        for (errno = 0, de = readdir(d);; errno = 0, de = readdir(d))   \
                if (!de) {                                              \
                        if (errno > 0) {                                \
                                on_error;                               \
                        }                                               \
                        break;                                          \
                } else if (streq(de->d_name, ".") ||                    \
                           streq(de->d_name, ".."))                     \
                        continue;                                       \
                else

/**
 * @brief Check string is digit.
 *
 * @param s String to check.
 * @param l Length to check.
 *
 * @return TRUE on all the characters are digit. FALSE on the others.
 */
bool is_number(const char *s, int l);

/**
 * @brief copy a file with mode, if destination file exists, return
 * error.
 *
 * @param src source file path
 * @param dst destination file path
 * @param mode destination file mode
 *
 * @return 0 on success, -errno on failure. -EALREADY if destination
 * file exist.
 */
int do_copy_mode(const char *src, const char *dst, mode_t mode);

/**
 * @brief copy a file with mode, if destination file exists, the file
 * is overwritten.
 *
 * @param src source file path
 * @param dst destination file path
 * @param mode destination file mode
 *
 * @return 0 on success, -errno on failure.
 */
int do_copy_mode_force(const char *src, const char *dst, mode_t mode);

/**
 * @brief copy a file, destination file mode is 0644, if destination
 * file exist, return error.
 *
 * @param src source file path
 * @param dst destination file path
 *
 * @return 0 on success, -errno on failure. -EALREADY if destination
 * file exist.
 */
int do_copy(const char *src, const char *dst);

/**
 * @brief copy a file, destination file mode is 0644, if destination
 * file exist, the file is overwritten.
 *
 * @param src source file path
 * @param dst destination file path
 *
 * @return 0 on success, -errno on failure.
 */
int do_copy_force(const char *src, const char *dst);

/**
 * @brief Make a directory. If parent directories are also absent,
 * make them also. Corresponding with "mkdir -p".
 *
 * @param path Path to make directory.
 * @param mode The directory mode.
 *
 * @return 0 on success, -errno on failure.
 */
int do_mkdir(const char *path, mode_t mode);

/**
 * @brief Remove all elements in path recursivly.
 *
 * @param path Path to make directory.
 *
 * @return 0 on success, -errno on failure.
 */
int rmdir_recursive(const char *path);

/**
 * @defgroup FILE_READ_WRITE_GROUP File Read/Write utility
 *
 * @{
 */

/**
 * file write flags
 */
enum file_write_flags {
        /** Append line-end(\\n) at the end of file. In case of string
         * write, if given string has already line-end characters
         * then this flag has no effect. */
        FILE_WRITE_NEWLINE_IF_NOT       =  1 << 0,
        /** Run fflush(3) after file write. */
        FILE_WRITE_WITH_FFLUSH          =  1 << 1,
        /** Open file as append mode. */
        FILE_WRITE_APPEND               =  1 << 2,
};

/**
 * @brief Write strings to FILE
 *
 * @param f File pointer.
 * @param str Strings to write.
 * @param flags Optional flags to write file. For
 * ::FILE_WRITE_NEWLINE_IF_NOT, if str has already line-end,
 * ::FILE_WRITE_NEWLINE_IF_NOT will has no effect. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_str_to_file(FILE *f, const char *str, enum file_write_flags flags);

/**
 * @brief Write strings to path.
 *
 * @param path File path.
 * @param str Strings to write.
 * @param flags Optional flags to write file. For
 * ::FILE_WRITE_NEWLINE_IF_NOT, if str has already line-end,
 * ::FILE_WRITE_NEWLINE_IF_NOT will has no effect. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_str_to_path(const char *path, const char *str, enum file_write_flags flags);

/**
 * @brief Write signed decimal integer to FILE.
 *
 * @param f File pointer.
 * @param i Signed integer to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_int32_to_file(FILE *f, int32_t i, enum file_write_flags flags);

/**
 * @brief Write signed decimal integer to path.
 *
 * @param path File path.
 * @param i Signed integer to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_int32_to_path(const char *path, int32_t i, enum file_write_flags flags);

/**
 * @brief Write unsigned decimal integer to FILE.
 *
 * @param f File pointer
 * @param u Unsigned integer to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_uint32_to_file(FILE *f, uint32_t u, enum file_write_flags flags);

/**
 * @brief Write unsigned decimal integer to path.
 *
 * @param path File path.
 * @param u Unsigned integer to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_uint32_to_path(const char *path, uint32_t u, enum file_write_flags flags);

/**
 * @brief Write 64 bit signed decimal integer to FILE.
 *
 * @param f File pointer.
 * @param i 64 bit signed integer to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_int64_to_file(FILE *f, int64_t i, enum file_write_flags flags);

/**
 * @brief Write 64 bit signed decimal integer to path.
 *
 * @param path File path.
 * @param i 64 bit signed integer to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_int64_to_path(const char *path, int64_t i, enum file_write_flags flags);

/**
 * @brief Write 64 bit unsigned decimal integer to FILE.
 *
 * @param f File pointer
 * @param u 64 bit Unsigned integer to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_uint64_to_file(FILE *f, uint64_t u, enum file_write_flags flags);

/**
 * @brief Write 64 bit unsigned decimal integer to path.
 *
 * @param path File path.
 * @param u 64 bit Unsigned integer to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_uint64_to_path(const char *path, uint64_t u, enum file_write_flags flags);

/**
 * @brief Write int to FILE.
 *
 * @param f File pointer
 * @param num int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_int_to_file(FILE *f, int num, enum file_write_flags flags);

/**
 * @brief Write int to path.
 *
 * @param path File path.
 * @param num int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_int_to_path(const char *path, int num, enum file_write_flags flags);

/**
 * @brief Write type unsigned int to FILE.
 *
 * @param f File pointer
 * @param num unsigned int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_unsigned_int_to_file(FILE *f, unsigned int num, enum file_write_flags flags);

/**
 * @brief Write int to path.
 *
 * @param path File path.
 * @param num int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_unsigned_int_to_path(const char *path, unsigned int num, enum file_write_flags flags);

/**
 * @brief Write long int to FILE.
 *
 * @param f File pointer
 * @param num long int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_long_int_to_file(FILE *f, long int num, enum file_write_flags flags);

/**
 * @brief Write int to path.
 *
 * @param path File path.
 * @param num int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_long_int_to_path(const char *path, long int num, enum file_write_flags flags);

/**
 * @brief Write unsigned long int to FILE.
 *
 * @param f File pointer
 * @param num unsigned long int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_unsigned_long_int_to_file(FILE *f, unsigned long int num, enum file_write_flags flags);

/**
 * @brief Write int to path.
 *
 * @param path File path.
 * @param num int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_unsigned_long_int_to_path(const char *path, unsigned long int num, enum file_write_flags flags);

/**
 * @brief Write long long int to FILE.
 *
 * @param f File pointer
 * @param num long long int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_long_long_int_to_file(FILE *f, long long int num, enum file_write_flags flags);

/**
 * @brief Write int to path.
 *
 * @param path File path.
 * @param num int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_long_long_int_to_path(const char *path, long long int num, enum file_write_flags flags);

/**
 * @brief Write unsigned long long int to FILE.
 *
 * @param f File pointer
 * @param num unsigned long long int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_unsigned_long_long_int_to_file(FILE *f, unsigned long long int num, enum file_write_flags flags);

/**
 * @brief Write int to path.
 *
 * @param path File path.
 * @param num int to write.
 * @param flags Optional flags to write file. if
 * ::FILE_WRITE_NEWLINE_IF_NOT is set, line-end added. For detail, see
 * ::file_write_flags.
 *
 * @return 0 on success, -errno on failure.
 */
int write_unsigned_long_long_int_to_path(const char *path, unsigned long long int num, enum file_write_flags flags);

/**
 * @brief Read the first line from FILE
 *
 * @param f File pointer.
 * @param line Duplicated string line is filled. This value has to
 * be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int read_one_line_from_file(FILE *f, char **line);

/**
 * @brief Read the first line from path
 *
 * @param path File path.
 * @param line Duplicated string line is filled. This value has to
 * be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int read_one_line_from_path(const char *path, char **line);

/**
 * @brief Read signed decimal integer from FILE.
 *
 * @param f File pointer.
 * @param i signed int value pointer.
 *
 * @return 0 on success, -errno on failure.
 */
int read_int32_from_file(FILE *f, int32_t *i);

/**
 * @brief Read signed decimalinteger from path.
 *
 * @param path File path.
 * @param i signed int value pointer.
 *
 * @return 0 on success, -errno on failure.
 */
int read_int32_from_path(const char *path, int32_t *i);

/**
 * @brief Read unsigned decimalinteger from FILE.
 *
 * @param f File pointer.
 * @param u unsigned int value pointer.
 *
 * @return 0 on success, -errno on failure.
 */
int read_uint32_from_file(FILE *f, uint32_t *u);

/**
 * @brief Read unsigned decimal integer from path
 *
 * @param path File path.
 * @param u unsigned int value pointer.
 *
 * @return 0 on success, -errno on failure.
 */
int read_uint32_from_path(const char *path, uint32_t *u);

/**
 * @brief Read 64 bit signed decimal integer from FILE.
 *
 * @param f File pointer.
 * @param i 64 bit signed int value pointer.
 *
 * @return 0 on success, -errno on failure.
 */
int read_int64_from_file(FILE *f, int64_t *i);

/**
 * @brief Read 64 bit signed decimal integer from path.
 *
 * @param path File path.
 * @param i 64 bit signed int value pointer.
 *
 * @return 0 on success, -errno on failure.
 */
int read_int64_from_path(const char *path, int64_t *i);

/**
 * @brief Read 64 bit unsigned decimal integer from FILE.
 *
 * @param f File pointer.
 * @param u 64 bit unsigned int value pointer.
 *
 * @return 0 on success, -errno on failure.
 */
int read_uint64_from_file(FILE *f, uint64_t *u);

/**
 * @brief Read 64 bit unsigned decimal integer from path
 *
 * @param path File path.
 * @param u 64 bit unsigned int value pointer.
 *
 * @return 0 on success, -errno on failure.
 */
int read_uint64_from_path(const char *path, uint64_t *u);

/**
 * @brief Read type int from FILE
 *
 * @param f File pointer.
 * @param num type int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_int_from_file(FILE *f, int *num);

/**
 * @brief Read type int from path
 *
 * @param path File path.
 * @param num type int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_int_from_path(const char *path, int *num);

/**
 * @brief Read type unsigned int from FILE
 *
 * @param f File pointer.
 * @param num type unsigned int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_unsigned_int_from_file(FILE *f, unsigned int *num);

/**
 * @brief Read type unsigned int from path
 *
 * @param path File path.
 * @param num type unsigned int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_unsigned_int_from_path(const char *path, unsigned int *num);

/**
 * @brief Read type long int from FILE
 *
 * @param f File pointer.
 * @param num type long int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_long_int_from_file(FILE *f, long int *num);

/**
 * @brief Read type long int from path
 *
 * @param path File path.
 * @param num type long int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_long_int_from_path(const char *path, long int *num);

/**
 * @brief Read type unsigned long int from FILE
 *
 * @param f File pointer.
 * @param num type unsigned long int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_unsigned_long_int_from_file(FILE *f, unsigned long int *num);

/**
 * @brief Read type unsigned long int from path
 *
 * @param path File path.
 * @param num type unsigned long int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_unsigned_long_int_from_path(const char *path, unsigned long int *num);

/**
 * @brief Read type long long int from FILE
 *
 * @param f File pointer.
 * @param num type long long int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_long_long_int_from_file(FILE *f, long long *num);

/**
 * @brief Read type long long int from path
 *
 * @param path File path.
 * @param num type long long int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_long_long_int_from_path(const char *path, long long *num);

/**
 * @brief Read type unsigned long long int from FILE
 *
 * @param f File pointer.
 * @param num type unsigned long long int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_unsigned_long_long_int_from_file(FILE *f, unsigned long long *num);

/**
 * @brief Read type unsigned long long int from path
 *
 * @param path File path.
 * @param num type unsigned long long int pointer
 *
 * @return 0 on success, -errno on failure.
 */
int read_unsigned_long_long_int_from_path(const char *path, unsigned long long *num);
/**
 * @}
 */

/**
 * @defgroup STRING_GROUP String helper
 *
 * @{
 */

/**
 * Compare two strings. TRUE on same, FALSE on others.
 * Same with (strcmp((a),(b)) == 0)
 */
#define streq(a,b) (strcmp((a),(b)) == 0)
/**
 * Compare two strings for n length. TRUE on same, FALSE on others.
 * Same with (strncmp((a), (b), (n)) == 0)
 */
#define strneq(a, b, n) (strncmp((a), (b), (n)) == 0)

/**
 * Compare two strings. Similar to streq() but ignore case. TRUE on
 * same, FALSE on others.
 * Same with (strcasecmp((a),(b)) == 0)
 */
#define strcaseeq(a,b) (strcasecmp((a),(b)) == 0)

/**
 * Compare two strings for n length. Similar to strneq() but ignore
 * case. TRUE on same, FALSE on others.
 * Same with (strcasecmp((a),(b)) == 0)
 */
#define strncaseeq(a, b, n) (strncasecmp((a), (b), (n)) == 0)

/**
 * Iterate string in strings which include null characters.
 * For example,
 *\code{.c}
const char str[] = {
        "foo\0"
        "bar\0";
};

const char *s;

NULSTR_FOREACH(s, str) {
        // do something here
}
 *\endcode
 */
#define NULSTR_FOREACH(i, l)                                    \
        for ((i) = (l); (i) && *(i); (i) = strchr((i), 0)+1)

/**
 * @brief Like streq(), but tries to make sense of NULL pointers.
 *
 * @param a String.
 * @param b String.
 *
 * @return TRUE on same, FALSE on the others.
 */
bool streq_ptr(const char *a, const char *b) _pure_;

/**
 * @brief Truncate line end characters.
 *
 * @param s String to truncate.
 *
 * @return Result string.
 */
char *truncate_nl(char *s);

/**
 * @brief Append suffix string to sting s with size b.
 *
 * @param s Ahead string.
 * @param suffix The second string.
 * @param b suffix size to append.
 *
 * @return Result string. This string has to be free-ed by caller.
 */
char *strnappend(const char *s, const char *suffix, size_t b);

/**
 * @brief Append suffix string to sting s.
 *
 * @param s Ahead string.
 * @param suffix The second string.
 *
 * @return Result string. This string has to be free-ed by caller.
 */
char *strappend(const char *s, const char *suffix);

/**
 * @brief Drops trailing whitespaces.
 *
 * @param s String.
 *
 * @return The pointer to the first non-space character.
 */
char *strstrip(char *s);

/**
 * @brief duplicate string without leading and trailing whitespaces
 *
 * @param str a target string to duplicate
 * @param ret newly allocated string is filled
 *
 * @return 0 on success, -errno on failure.
 */
int strdup_strip(const char *str, char **ret);

/**
 * @brief duplicate string without leading and trailing whitespaces,
 * duplicated string is not over given length len
 *
 * @param str a target string to duplicate
 * @param len maxium length of duplicate
 * @param ret newly allocated string is filled
 *
 * @return 0 on success, -errno on failure.
 */
int strndup_strip(const char *str, size_t len, char **ret);

/**
 * @brief nulstr is similar to string list but each strings ends with
 * null and the strings are put at one memory buffer. For example,
 * "foo" and "bar" string can be represented "foo\0bar". This function
 * check nulstr is containing the needle string.
 *
 * @param nulstr a nulstr
 * @param needle a needle string to find
 *
 * @return true if the needle found, otherwise false.
 */
bool nulstr_contains(const char*nulstr, const char *needle);

/**
 * @brief check a string ends with postfix pattern
 *
 * @param s a string to check
 * @param postfix postfix string
 *
 * @return if s is ended with postfix string the pointer of the
 * string, matched pointer of s is returned. Otherwise NULL.
 */
char* endswith(const char *s, const char *postfix);

/**
 * @brief split a string into words. This api generally is not called
 * directly, #FOREACH_WORD_SEPARATOR or #FOREACH_WORD are using
 * this. If separator does not include quotes then quoted words are
 * assumed as single word.
 *
 * @param c string to split
 * @param l splitted word length
 * @param separator separator strings such like #WHITESPACE
 * @param state a state internally used
 *
 * @return a splitted current word pointer
 */
char *split(const char *c, size_t *l, const char *separator, char **state);

/**
 * @brief Iterate for each words. If separator does not include quotes
 * then quoted words are assumed as single word.
 *
 * @param word Each word
 * @param length Length of word
 * @param s Target string
 * @param separator Seperator string
 * @param state Used only internal split().
 */
#define FOREACH_WORD_SEPARATOR(word, length, s, separator, state)       \
        for ((state) = NULL, (word) = split((s), &(length), (separator), &(state)); (word); (word) = split((s), &(length), (separator), &(state)))

/**
 * @brief Iterate for each words. (Seperators are WHITESPACES.) Quoted
 * words are assumed as single word.
 *
 * @param word Each word
 * @param length Length of word
 * @param s Target string
 * @param state Used only internal split().
 */
#define FOREACH_WORD(word, length, s, state)                            \
        FOREACH_WORD_SEPARATOR(word, length, s, WHITESPACE, state)

/**
 * @brief Duplicate string and strip quotes from the string.
 *
 * @param str String to duplicate.
 * @param quotes Quote characters to strip. Predefined #QUOTES can be
 * used to specify quote and double quote.
 *
 * @return Result string. This value has to be free-ed by caller.
 */
char *strdup_unquote(const char *str, const char *quotes);
/**
 * @}
 */

/**
 * @defgroup STRV_GROUP String List
 *
 * @{
 */

/**
 * iterate for each elements of string list.
 */
#define FOREACH_STRV(s, l)                      \
        for ((s) = (l); (s) && *(s); (s)++)

/**
 * @brief Split given string to string list with separator.
 *
 * @param str string to split as string list.
 * @param strv Splitted string list is filled. This string list has to
 *   be free-ed.
 * @param separator sperators to split the string.
 *
 * @return 0 on success, -errno on failure.
 */
int str_to_strv(const char *str, char ***strv, const char *separator);

/**
 * @brief Get elements of string list. #sizeof_strv() does not count
 * end of list NULL. For example, for {"foo", "bar", NULL} string
 * list, #sizeof_strv() returns 2.
 *
 * @param strv string list.
 *
 * @return number of string list.
 */
size_t sizeof_strv(char **strv);

/**
 * @brief Merge two string lists. If {"foo", "bar"} and {"baz", "qux"}
 * are given, the result is {"foo", "bar", "baz", "quz"}.
 *
 * @param first The first string list.
 * @param second The second string list.
 * @param strv Merged string list.
 * @param free_second If TRUE is given, the second string list will be
 * free-ed. If FALSE, no action.
 *
 * @return number of string list.
 */
int strv_attach(char **first, char **second, char ***strv, bool free_second);

/**
 * @brief Free all given string list
 *
 * @param strv string list to free.
 */
void strv_free_full(char **strv);
/**
 * @}
 */

/**
 * @brief Check given path is directory or not
 *
 * @param path path to check
 *
 * @return TRUE if path is directory, FALSE on others.
 */
bool isdir(const char *path);

/**
 * @brief Simple file create api similar to touch(1)
 *
 * @param path file path
 *
 * @return 0 on success, -errno on failure.
 */
int touch(const char *path);

/**
 * @brief Check mount entry. Multiple matches of conditoin are able to
 * be set with mnt_fsname, mnt_dir, mnt_type or mnt_opts. If multiple
 * matches are given, return true if a entry satisfied all matches.
 *
 * \code{.c}
// check cgroup is mounted
if (is_mounted("cgroup", NULL, NULL, NULL))
        printf("cgroup is mounted\n");

// check /tmp is mounted
if (is_mounted("tmpfs", "/tmp", NULL, NULL))
        printf("/tmp is mounted\n");

// check cgroup is mounted as cgroup2
if (is_mounted("cgroup", "/sys/fs/cgroup", "cgroup2", NULL))
        printf("cgroup is mounted as cgroup2\n");
 * \endcode
 *
 * @param fsname find matched mount filesystem name
 * @param dir find matched mount dir(path) name
 * @param type find matched mount type name
 * @param opts find matched mount option name
 *
 * @return true if matched mount entry found, otherwise false.
 */
bool mnt_is_mounted(const char *fsname, const char *dir, const char *type, const char *opts);

/**
 * @defgroup EXEC_GROUP exec group
 *
 * @brief fork() and exec() utility
 * @{
 */

/**
 * standard output/error redirect flags
 */
enum {
        /**
         * Do not redirect standard output/error
         */
        EXEC_REDIRECT_NONE      = 0x01 << 0,
        /**
         * Redirect standard output only
         */
        EXEC_REDIRECT_OUTPUT    = 0x01 << 1,
        /**
         * Redirect standard error only
         */
        EXEC_REDIRECT_ERROR     = 0x01 << 2,
        /**
         * Redirect standard output and error all
         */
        EXEC_REDIRECT_ALL       = (EXEC_REDIRECT_OUTPUT | EXEC_REDIRECT_ERROR),
};

/**
 * @brief Traditional fork() and exec() helper. If child is not
 * deactivated within given \p timeout_msec then kill it with given
 * signal. And additionally redirect child process standard output or
 * standard error to given fd.
 *
 * @param argv array of pointers to null-terminated strings that
 * represent the argument list available to the new program. The first
 * argument should point to the filename associated with the file
 * being executed. The array of pointers must be terminated by a NULL
 * pointer.
 * @param envp specify the environment of the executed program via the
 * argument envp. The envp argument is an array of pointers to
 * null-terminated strings and must be terminated by a NULL pointer.
 * @param timeout_msec timeout millisecond to prevent infinite
 * waiting. If negative is given, the parent will not wait the
 * child. In other word, the parent will return immediately. If 0 is
 * given, parent will wait the child infinitly. And if positive value
 * is given parent will wait given milliseconds and expired return
 * -1. If the child is exit within the tiemout millisecond return with
 * child exit code.
 * @param sig signal to kill the child on timeout.
 * @param fd file descriptor to redirect child standard output or
 * error.
 * @param flags redirect flag. This flags is able to include
 * EXEC_REDIRECT_OUTPUT or EXEC_REDIRECT_ERROR.
 *
 * @return exit code of child. It is fully depend on the child
 * process. If the child exit with 1 then this function also return 1.
 * Negative errno on error. -ETIME on timer expired.
 */
int do_fork_exec_kill_redirect(char *const argv[], char * const envp[], int64_t timeout_msec, int sig, int fd, int flags);

/**
 * @brief Traditional fork() and exec() helper. And additionally
 * redirect child process standard output or standard error to given fd.
 *
 * @param argv array of pointers to null-terminated strings that
 * represent the argument list available to the new program. The first
 * argument should point to the filename associated with the file
 * being executed. The array of pointers must be terminated by a NULL pointer.
 * @param envp specify the environment of the executed program via the
 * argument envp. The envp argument is an array of pointers to
 * null-terminated strings and must be terminated by a NULL pointer.
 * @param timeout_msec timeout millisecond to prevent infinite
 * waiting. If negative is given, the parent will not wait the
 * child. In other word, the parent will return immediately. If 0 is
 * given, parent will wait the child infinitly. And if positive value
 * is given parent will wait given milliseconds and expired return
 * -1. If the child is exit within the tiemout millisecond return with
 * child exit code.
 * @param fd file descriptor to redirect child standard output or error.
 * @param flags redirect flag. This flags is able to include
 * EXEC_REDIRECT_OUTPUT or EXEC_REDIRECT_ERROR.
 *
 * @return exit code of child. It is fully depend on the child
 * process. If the child exit with 1 then this function also return 1.
 * Negative errno on error. -ETIME on timer expired.
 */
int do_fork_exec_redirect(char *const argv[], char * const envp[], int64_t timeout_msec, int fd, int flags);

/**
 * @brief Traditional fork() and exec() helper. If child is not
 * deactivated within given \p timeout_msec then kill it with given
 * signal.
 *
 * @param argv array of pointers to null-terminated strings that
 * represent the argument list available to the new program. The first
 * argument should point to the filename associated with the file
 * being executed. The array of pointers must be terminated by a NULL pointer.
 * @param envp specify the environment of the executed program via the
 * argument envp. The envp argument is an array of pointers to
 * null-terminated strings and must be terminated by a NULL pointer.
 * @param timeout_msec timeout millisecond to prevent infinite
 * waiting. If negative is given, the parent will not wait the
 * child. In other word, the parent will return immediately. If 0 is
 * given, parent will wait the child infinitly. And if positive value
 * is given parent will wait given milliseconds and expired return
 * -1. If the child is exit within the tiemout millisecond return with
 * child exit code.
 * @param sig signal to kill the child on timeout.
 *
 * @return exit code of child. It is fully depend on the child
 * process. If the child exit with 1 then this function also return 1.
 * Negative errno on error. -ETIME on timer expired.
 */
int do_fork_exec_kill(char *const argv[], char * const envp[], int64_t timeout_msec, int sig);

/**
 * @brief Traditional fork() and exec() helper.
 *
 * @param argv array of pointers to null-terminated strings that
 * represent the argument list available to the new program. The first
 * argument should point to the filename associated with the file
 * being executed. The array of pointers must be terminated by a NULL pointer.
 * @param envp specify the environment of the executed program via the
 * argument envp. The envp argument is an array of pointers to
 * null-terminated strings and must be terminated by a NULL pointer.
 * @param timeout_msec timeout millisecond to prevent infinite
 * waiting. If negative is given, the parent will not wait the
 * child. In other word, the parent will return immediately. If 0 is
 * given, parent will wait the child infinitly. And if positive value
 * is given parent will wait given milliseconds and expired return
 * -1. If the child is exit within the tiemout millisecond return with
 * child exit code.
 *
 * @return exit code of child. It is fully depend on the child
 * process. If the child exit with 1 then this function also return 1.
 * Negative errno on error. -ETIME on timer expired.
 */
int do_fork_exec(char *const argv[], char * const envp[], int64_t timeout_msec);

enum {
        /**
         * Does not set any I/O priority class.
         */
        IOPRIO_CLASS_NONE,

        /**
         * This is the real-time I/O class. This scheduling class is
         * given higher priority than any other class: processes from
         * this class are given first access to the disk every
         * time. Thus this I/O class needs to be used with some care:
         * one I/O real-time process can starve the entire
         * system. Within the real-time class, there are 8 levels of
         * class data (priority) that determine exactly how much time
         * this process needs the disk for on each service. The
         * highest real-time priority level is 0; the lowest is 7. In
         * the future this might change to be more directly mappable
         * to performance, by passing in a desired data rate instead.
         */
        IOPRIO_CLASS_RT,

        /**
         * This is the best-effort scheduling class, which is the
         * default for any process that hasn't set a specific I/O
         * priority. The class data (priority) determines how much I/O
         * bandwidth the process will get. Best-effort priority levels
         * are analogous to CPU nice values (see getpriority(2)). The
         * priority level determines a priority relative to other
         * processes in the best-effort scheduling class. Priority
         * levels range from 0 (highest) to 7 (lowest).
         */
        IOPRIO_CLASS_BE,

        /**
         * This is the idle scheduling class. Processes running at
         * this level only get I/O time when no-one else needs the
         * disk. The idle class has no class data. Attention is
         * required when assigning this priority class to a process,
         * since it may become starved if higher priority processes
         * are constantly accessing the disk.
         */
        IOPRIO_CLASS_IDLE,
};

struct exec_info {
        /**
         * array of pointers to null-terminated strings that represent
         * the argument list available to the new program. The first
         * argument should point to the filename associated with the
         * file being executed. The array of pointers must be
         * terminated by a NULL pointer.
         */
        char **argv;

        /**
         * specify the environment of the executed program via the
         * argument envp. The envp argument is an array of pointers to
         * null-terminated strings and must be terminated by a NULL
         * pointer.
         */
        char **envp;

        /**
         * timeout milliseconds. On negative, parent will not wait the
         * child. On 0, parent will wait the child until exit. On
         * positive, parent will wait the child until exit during
         * given milliseconds. If the child is not exited within this
         * milliseconds, the child will be killed with given
         * kill_signal.
         */
        int64_t timeout_msec;

        /**
         * signal to kill the child on timeout. Default is SIGTERM.
         */
        int kill_signal;

        /**
         * file descriptor to redirect child standard output.
         */
        int out_fd;

        /**
         * file descriptor to redirect child standard error.
         */
        int err_fd;

        /**
         * process priority of child.
         */
        int prio;

        /**
         * I/O process priority of child.
         */
        int ioprio;
};

/**
 * Initialize struct exec_info.
 */
#define EXEC_INFO_INIT { NULL, NULL, 0, SIGTERM, -1, -1, getpriority(PRIO_PROCESS, 0), IOPRIO_CLASS_NONE }

/**
 * @brief Traditional fork() and exec() helper.
 *
 * @param exec struct exec_info.
 *
 * @return If timeout_msec has negative value, pid of child. Others exit code of child.
 */
int fork_exec(struct exec_info *exec);
/**
 * @}
 */

/**
 * @defgroup TIME_UTIL_GROUP time util group
 *
 * @brief time utility libraries
 * @{
 */

/** millisecond per second */
#define MSEC_PER_SEC            1000ULL
/** microsecond per second */
#define USEC_PER_SEC            ((uint64_t) 1000000ULL)
/** microsecond per millisecond */
#define USEC_PER_MSEC           ((uint64_t) 1000ULL)
/** nanosecond per second */
#define NSEC_PER_SEC            ((uint64_t) 1000000000ULL)
/** nanosecond per microsecond */
#define NSEC_PER_MSEC           ((uint64_t) 1000000ULL)
/** nanosecond per microsecond */
#define NSEC_PER_USEC           ((uint64_t) 1000ULL)

/** microsecond per minute */
#define USEC_PER_MINUTE         ((uint64_t) (60ULL*USEC_PER_SEC))
/** nanosecond per minute */
#define NSEC_PER_MINUTE         ((uint64_t) (60ULL*NSEC_PER_SEC))
/** microsecond per hour */
#define USEC_PER_HOUR           ((uint64_t) (60ULL*USEC_PER_MINUTE))
/** nanosecond per hour */
#define NSEC_PER_HOUR           ((uint64_t) (60ULL*NSEC_PER_MINUTE))
/** microsecond per day */
#define USEC_PER_DAY            ((uint64_t) (24ULL*USEC_PER_HOUR))
/** nanosecond per day */
#define NSEC_PER_DAY            ((uint64_t) (24ULL*NSEC_PER_HOUR))
/** microsecond per week */
#define USEC_PER_WEEK           ((uint64_t) (7ULL*USEC_PER_DAY))
/** nanosecond per week */
#define NSEC_PER_WEEK           ((uint64_t) (7ULL*NSEC_PER_DAY))
/** microsecond per month */
#define USEC_PER_MONTH          ((uint64_t) (2629800ULL*USEC_PER_SEC))
/** nanosecond per month */
#define NSEC_PER_MONTH          ((uint64_t) (2629800ULL*NSEC_PER_SEC))
/** microsecond per year */
#define USEC_PER_YEAR           ((uint64_t) (31557600ULL*USEC_PER_SEC))
/** nanosecond per year */
#define NSEC_PER_YEAR           ((uint64_t) (31557600ULL*NSEC_PER_SEC))

/** frequently used time format string: 12:34 */
#define HH_MM                           "%H:%M"
/** frequently used time format string: 12:34:56 */
#define HH_MM_SS                        "%H:%M:%S"

/** frequently used time format string: 2015-01-23 */
#define YYYY_MM_DD                      "%Y-%m-%d"
/** frequently used time format string: 2015-01-23 12:34 */
#define YYYY_MM_DD_HH_MM                "%Y-%m-%d %H:%M"
/** frequently used time format string: 2015-01-23 12:34:56 */
#define YYYY_MM_DD_HH_MM_SS             "%Y-%m-%d %H:%M:%S"
/** frequently used time format string: 2015-01-23 12:34:56 KST */
#define YYYY_MM_DD_HH_MM_SS_Z           "%Y-%m-%d %H:%M:%S %Z"

/** frequently used time format string: Fri 2015-01-23 */
#define DOW_YYYY_MM_DD                  "%a %Y-%m-%d"
/** frequently used time format string: Fri 2015-01-23 12:34 */
#define DOW_YYYY_MM_DD_HH_MM            "%a %Y-%m-%d %H:%M"
/** frequently used time format string: Fri 2015-01-23 12:34:56 */
#define DOW_YYYY_MM_DD_HH_MM_SS         "%a %Y-%m-%d %H:%M:%S"
/** frequently used time format string: Fri 2015-01-23 12:34:56 KST */
#define DOW_YYYY_MM_DD_HH_MM_SS_Z       "%a %Y-%m-%d %H:%M:%S %Z"

/**
 * @brief Convert time_t to given format time string.
 *
 * @param sec time second to convert
 * @param format format string
 * @param time string pointer to converted time is filled. On
 * successful return, this value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int sec_to_timestr(time_t sec, const char *format, char **time);

/**
 * @brief Convert time_t to \%a \%Y-\%m-\%d \%H:\%M:\%S \%Z format time string.
 *
 * @param sec time second to convert
 * @param time string pointer to converted time is filled. On
 * successful return, this value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int sec_to_timestr_full(time_t sec, char **time);

/**
 * @brief Convert given format time string to time_t.
 *
 * @param format format string
 * @param time time string to convert to time_t
 * @param sec converted time_t
 *
 * @return 0 on success, -errno on failure.
 */
int timestr_to_sec(const char *format, const char *time, time_t *sec);

/**
 * @brief Make struct timeval from millisecond
 *
 * @param msec millisecond to Convert
 * @param tv struct timeval to be filled
 */
void msec_to_timeval(uint64_t msec, struct timeval *tv);

/**
 * @brief Check string is float.
 *
 * @param s String to check.
 *
 * @return TRUE on all the characters are digit. FALSE on the others.
 */
bool is_float(const char *s);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

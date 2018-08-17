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
 * @file config-parser.h
 *
 * ini type config file parser
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 */

#pragma once

#include <stdio.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Prototype for a parser for a specific configuration setting
 */
typedef int (*ConfigParserCallback)(
                const char *filename,
                unsigned line,
                const char *section,
                const char *lvalue,
                int ltype,
                const char *rvalue,
                void *data);

/**
 * @brief A callback function of #config_parse_dir.
 *
 * @param path a parsing config file name
 * @param data user data to be passed by #config_parse_dir.
 */
typedef int (*ConfigParseFunc)(const char *path, void *data);

/**
 * Wraps information for parsing a specific configuration variable, to
 * be stored in a simple array
 */
typedef struct ConfigTableItem {
        /**
         * Section
         */
        const char *section;

        /**
         * Name of the variable
         */
        const char *lvalue;

        /**
         * Function that is called to parse the variable's value
         */
        ConfigParserCallback cb;

        /**
         * Distinguish different variables passed to the same callback
         */
        int ltype;

        /**
         * Where to store the variable's data
         */
        void *data;
} ConfigTableItem;

/**
 * @brief config parser function
 *
 * @param filename full path of config file
 * @param table a table of #ConfigTableItem to parse
 *
 * @return 0 on success, -errno on failure.
 */
int config_parse(const char *filename, void *table);

/**
 * @brief parse all regular config files in directory
 *
 * @param dir dir full path
 * @param fp config parse function.
 * @param data user data to be passed to config parser function
 *
 * @return 0 on success, -errno on failure.
 */
int config_parse_dir(const char *dir, ConfigParseFunc fp, void *data);


/**
 * @brief A common int type rvalue parser.
 *
 * @param filename a parsing config file name
 * @param line a parsing config file line
 * @param section a parsing config file section
 * @param lvalue a parsing config file left value
 * @param ltype a parsing config file left value type. (not used.)
 * @param rvalue a parsing config file rvalue
 * @param data user data
 *
 * @return 0 on success, -errno on failure.
 */
int config_parse_int(const char *filename, unsigned line, const char *section, const char *lvalue, int ltype, const char *rvalue, void *data);

/**
 * @brief A common boolean type rvalue parser.
 *
 * @param filename a parsing config file name
 * @param line a parsing config file line
 * @param section a parsing config file section
 * @param lvalue a parsing config file left value
 * @param ltype a parsing config file left value type. (not used.)
 * @param rvalue a parsing config file rvalue
 * @param data user data
 *
 * @return 0 on success, -errno on failure.
 */
int config_parse_bool(const char *filename, unsigned line, const char *section, const char *lvalue, int ltype, const char *rvalue, void *data);

/**
 * @brief A common string type rvalue parser.
 *
 * @param filename a parsing config file name
 * @param line a parsing config file line
 * @param section a parsing config file section
 * @param lvalue a parsing config file left value
 * @param ltype a parsing config file left value type. (not used.)
 * @param rvalue a parsing config file rvalue
 * @param data user data
 *
 * @return 0 on success, -errno on failure.
 */
int config_parse_string(const char *filename, unsigned line, const char *section, const char *lvalue, int ltype, const char *rvalue, void *data);

/**
 * @brief A common byte type rvalue parser.
 *
 * @param filename a parsing config file name
 * @param line a parsing config file line
 * @param section a parsing config file section
 * @param lvalue a parsing config file left value
 * @param ltype a parsing config file left value type. (not used.)
 * @param rvalue a parsing config file rvalue
 * @param data user data
 *
 * @return 0 on success, -errno on failure.
 */
int config_parse_bytes(const char *filename, unsigned line, const char *section, const char *lvalue, int ltype, const char *rvalue, void *data);

/**
 * @brief A common percent(%) type rvalue parser. The right value has
 * to be end with '%'.
 *
 * @param filename a parsing config file name
 * @param line a parsing config file line
 * @param section a parsing config file section
 * @param lvalue a parsing config file left value
 * @param ltype a parsing config file left value type. (not used.)
 * @param rvalue a parsing config file rvalue
 * @param data user data
 *
 * @return 0 on success, -errno on failure.
 */
int config_parse_percent(const char *filename, unsigned line, const char *section, const char *lvalue, int ltype, const char *rvalue, void *data);

/**
 * @brief A common string vector type rvalue parser.
 *
 * @param filename a parsing config file name
 * @param line a parsing config file line
 * @param section a parsing config file section
 * @param lvalue a parsing config file left value
 * @param ltype a parsing config file left value type. (not used.)
 * @param rvalue a parsing config file rvalue
 * @param data user data
 *
 * @return 0 on success, -errno on failure.
 */
int config_parse_strv(const char *filename, unsigned line, const char *section, const char *lvalue, int ltype, const char *rvalue, void *data);

/**
 * @brief A common float type rvalue parser.
 *
 * @param filename a parsing config file name
 * @param line a parsing config file line
 * @param section a parsing config file section
 * @param lvalue a parsing config file left value
 * @param ltype a parsing config file left value type. (not used.)
 * @param rvalue a parsing config file rvalue
 * @param data user data
 *
 * @return 0 on success, -errno on failure.
 */
int config_parse_float(const char *filename, unsigned line, const char *section, const char *lvalue, int ltype, const char *rvalue, void *data);

#ifdef __cplusplus
}
#endif

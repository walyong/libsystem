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

/**
 * @file glib-util.h
 *
 * GLib utility library
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 */

#pragma once

#include <glib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Iterate for each list nodes.
 *
 * @param c current node
 * @param l list to iterate
 */
#define FOREACH_G_LIST(c, l)                                    \
        for (c = g_list_first(l); c; c = g_list_next(c))

/**
 * @brief Reverse iterate for each list nodes.
 *
 * @param c current node
 * @param l list to iterate
 */
#define FOREACH_G_LIST_REVERSE(c, l)                            \
        for (c = g_list_last(l); c; c = g_list_previous(c))

/**
 * @brief Iterate for each list nodes. #FOREACH_G_LIST_SAFE is similar
 * with #FOREACH_G_LIST but safe for list remove. When you are
 * iterating a list to remove some of list nodes, you have to use
 * #FOREACH_G_LIST_SAFE for safe iteration.
 *
 * @param c current node
 * @param n next node of current iteration, this is used for safe iteration
 * @param l list to iterate
 */
#define FOREACH_G_LIST_SAFE(c, n, l)                                    \
        for (c = g_list_first(l), n = g_list_next(c); c; c = n, n = g_list_next(c))

/**
 * @brief Reverse iterate for each list
 * nodes. #FOREACH_G_LIST_SAFE_REVERSE is similar with
 * #FOREACH_G_LIST_REVERSE but safe for list remove. When you are
 * iterating a list to remove some of list nodes, you have to use
 * #FOREACH_G_LIST_SAFE_REVERSE for safe iteration.
 *
 * @param c current node
 * @param p previous node of current iteration, this is used for safe iteration
 * @param l list to iterate
 */
#define FOREACH_G_LIST_SAFE_REVERSE(c, p, l)                            \
        for (c = g_list_last(l), p = g_list_previous(c); c; c = p, p = g_list_previous(c))

/**
 * @brief Convert GError to errno.
 *
 * @param error GError to convert
 *
 * @return converted errno.
 */
int gerror_to_errno(GError *error);

#ifdef __cplusplus
}
#endif

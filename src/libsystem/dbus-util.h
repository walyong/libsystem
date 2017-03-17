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

/**
 * @file dbus-util.h
 *
 * dbus utility library
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 */

#pragma once

#include <dbus/dbus.h>
#include "libsystem.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline void __dbus_error_free(DBusError *error) {
        if (error && dbus_error_is_set(error))
                dbus_error_free(error);
}

static inline void __dbus_message_unref(DBusMessage **message) {
        if (*message)
                dbus_message_unref(*message);
}

/**
 * Declare value with cleanup attribute. dbus_error_free() is called
 * when is going out the scope.
 */
#define _cleanup_dbus_error_free_ _cleanup_(__dbus_error_free)

/**
 * Declare value with cleanup attribute. dbus_message_unref() is
 * called when is going out the scope.
 */
#define _cleanup_dbus_message_unref_ _cleanup_(__dbus_message_unref)

#ifdef __cplusplus
}
#endif

/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: nil -*-*/

/*
 * libsystem-sd
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
#include <assert.h>
#include <gio/gio.h>
#include <errno.h>

#include "libsystem/libsystem.h"
#include "libsystem/glib-util.h"
#include "libsystem-sd/systemd.h"

#define DBUS_INTERFACE_DBUS_PROPERTIES          DBUS_SERVICE_DBUS ".Properties"
#define DBUS_INTERFACE_DBUS_PEER                DBUS_SERVICE_DBUS ".Peer"
#define DBUS_INTERFACE_DBUS_INTROSPECTABLE      DBUS_SERVICE_DBUS ".Introspectable"

#define ERR_MSG_DUP(to, from) do {      \
        (to) = strdup((from));          \
        assert((to));                   \
} while (0)

static const char* const systemd_unit_type_string[_SYSTEMD_UNIT_TYPE_MAX] = {
        [SYSTEMD_UNIT_SERVICE]          = "service",
        [SYSTEMD_UNIT_SOCKET]           = "socket",
        [SYSTEMD_UNIT_DEVICE]           = "device",
        [SYSTEMD_UNIT_MOUNT]            = "mount",
        [SYSTEMD_UNIT_AUTOMOUNT]        = "automount",
        [SYSTEMD_UNIT_SWAP]             = "swap",
        [SYSTEMD_UNIT_TARGET]           = "target",
        [SYSTEMD_UNIT_PATH]             = "path",
        [SYSTEMD_UNIT_TIMER]            = "timer",
        [SYSTEMD_UNIT_SNAPSHOT]         = "snapshot",
        [SYSTEMD_UNIT_SLICE]            = "slice",
        [SYSTEMD_UNIT_SCOPE]            = "scope",
};

static int systemd_call_sync(GDBusConnection *connection,
                             const char *name,
                             const char *path,
                             const char *iface,
                             const char *method,
                             GVariant *parameters,
                             GVariant **reply,
                             GError **error) {

        GError *err;
        GVariant *gvar;

#if (GLIB_MAJOR_VERSION <= 2 && GLIB_MINOR_VERSION < 36)
        g_type_init();
#endif

        assert(name);
        assert(path);
        assert(iface);
        assert(method);
        assert(reply);
        assert(error && !*error);

        if (connection) {
                err = NULL;
                gvar = g_dbus_connection_call_sync(connection,
                                                   name,
                                                   path,
                                                   iface,
                                                   method,
                                                   parameters,
                                                   NULL,
                                                   G_DBUS_CALL_FLAGS_NONE,
                                                   -1,
                                                   NULL,
                                                   &err);
        } else {
                g_autoptr(GDBusProxy) proxy = NULL;

                err = NULL;
                proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SYSTEM,
                                                      G_DBUS_PROXY_FLAGS_NONE,
                                                      NULL, /* GDBusInterfaceInfo */
                                                      name,
                                                      path,
                                                      iface,
                                                      NULL, /* GCancellable */
                                                      &err);

                if (err) {
                        *error = err;
                        return -err->code;
                }

                err = NULL;
                gvar = g_dbus_proxy_call_sync(proxy,
                                              method,
                                              parameters,
                                              G_DBUS_CALL_FLAGS_NONE,
                                              -1,
                                              NULL, /* GCancellable */
                                              &err);
        }

        if (err) {
                *error = err;
                return -err->code;
        }

        g_assert(gvar != NULL);
        *reply = gvar;

        return 0;
}

int systemd_subscribe(GDBusConnection *connection, char **err_msg) {
        g_autoptr(GVariant) reply = NULL;
        GError *error;
        int r;

        error = NULL;
        r = systemd_call_sync(connection,
                              DBUS_SYSTEMD_BUSNAME,
                              DBUS_SYSTEMD_PATH,
                              DBUS_SYSTEMD_INTERFACE_MANAGER,
                              "Subscribe",
                              NULL,
                              &reply,
                              &error);
        if (error) {
                if (err_msg)
                        ERR_MSG_DUP(*err_msg, error->message);

                g_error_free(error);
        }

        return r;
}

int systemd_unsubscribe(GDBusConnection *connection, char **err_msg) {
        g_autoptr(GVariant) reply = NULL;
        GError *error;
        int r;

        error = NULL;
        r = systemd_call_sync(connection,
                              DBUS_SYSTEMD_BUSNAME,
                              DBUS_SYSTEMD_PATH,
                              DBUS_SYSTEMD_INTERFACE_MANAGER,
                              "Unsubscribe",
                              NULL,
                              &reply,
                              &error);
        if (error) {
                if (err_msg)
                        ERR_MSG_DUP(*err_msg, error->message);

                g_error_free(error);
        }

        return r;
}

int systemd_get_unit(GDBusConnection *connection,
                     const char *name,
                     char **unit,
                     char **err_msg) {

        g_autoptr(GVariant) reply = NULL;
        char *obj = NULL;
        GError *error;
        int r;

        assert(name);
        assert(unit);

        error = NULL;
        r = systemd_call_sync(connection,
                              DBUS_SYSTEMD_BUSNAME,
                              DBUS_SYSTEMD_PATH,
                              DBUS_SYSTEMD_INTERFACE_MANAGER,
                              "GetUnit",
                              g_variant_new("(s)",
                                            name),
                              &reply,
                              &error);
        if (error) {
                if (err_msg)
                        ERR_MSG_DUP(*err_msg, error->message);

                g_error_free(error);

                return r;
        }

        if (!g_variant_is_of_type(reply, G_VARIANT_TYPE("(o)"))) {
                if (err_msg)
                        ERR_MSG_DUP(*err_msg, "reply message is not type of object path");

                return -EBADMSG;
        }

        g_variant_get(reply, "(o)", &obj);

        *unit = obj;

        return 0;
}

int systemd_control_unit(GDBusConnection *connection,
                         const char *method,
                         const char *name,
                         char **job,
                         char **err_msg) {

        g_autoptr(GVariant) reply = NULL;
        char *obj = NULL;
        GError *error;
        int r;

        assert(method);
        assert(name);
        assert(job);

        error = NULL;
        r = systemd_call_sync(connection,
                              DBUS_SYSTEMD_BUSNAME,
                              DBUS_SYSTEMD_PATH,
                              DBUS_SYSTEMD_INTERFACE_MANAGER,
                              method,
                              g_variant_new("(ss)",
                                            name,
                                            "replace"),
                              &reply,
                              &error);
        if (error) {
                if (err_msg)
                        ERR_MSG_DUP(*err_msg, error->message);

                g_error_free(error);

                return r;
        }

        if (!g_variant_is_of_type(reply, G_VARIANT_TYPE("(o)"))) {
                if (err_msg)
                        ERR_MSG_DUP(*err_msg, "reply message is not type of object path");

                return -EBADMSG;
        }

        g_variant_get(reply, "(o)", &obj);

        *job = obj;

        return 0;
}

int systemd_start_unit(GDBusConnection *connection,
                       const char *name,
                       char **job,
                       char **err_msg) {

        assert(name);
        assert(job);

        return systemd_control_unit(connection, "StartUnit", name, job, err_msg);
}

int systemd_stop_unit(GDBusConnection *connection,
                      const char *name,
                      char **job,
                      char **err_msg) {

        assert(name);
        assert(job);

        return systemd_control_unit(connection, "StopUnit", name, job, err_msg);
}

int systemd_reload_unit(GDBusConnection *connection,
                        const char *name,
                        char **job,
                        char **err_msg) {

        assert(name);
        assert(job);

        return systemd_control_unit(connection, "ReloadUnit", name, job, err_msg);
}

int systemd_restart_unit(GDBusConnection *connection,
                         const char *name,
                         char **job,
                         char **err_msg) {

        assert(name);
        assert(job);

        return systemd_control_unit(connection, "RestartUnit", name, job, err_msg);
}

int systemd_try_restart_unit(GDBusConnection *connection,
                             const char *name,
                             char **job,
                             char **err_msg) {

        assert(name);
        assert(job);

        return systemd_control_unit(connection, "TryRestartUnit", name, job, err_msg);
}

int systemd_reload_or_restart_unit(GDBusConnection *connection,
                                   const char *name,
                                   char **job,
                                   char **err_msg) {

        assert(name);
        assert(job);

        return systemd_control_unit(connection, "ReloadOrRestartUnit", name, job, err_msg);
}

int systemd_reload_or_try_restart_unit(GDBusConnection *connection,
                                       const char *name,
                                       char **job,
                                       char **err_msg) {

        assert(name);
        assert(job);

        return systemd_control_unit(connection, "ReloadOrTryRestartUnit", name, job, err_msg);
}

static int systemd_get_property(GDBusConnection *connection,
                                const char *name,
                                const char *path,
                                const char *iface,
                                const char *method,
                                const char *interface,
                                const char *property,
                                GVariant **variant,
                                char **err_msg) {

        GError *error;
        int r;

        assert(name);
        assert(path);
        assert(iface);
        assert(method);
        assert(interface);
        assert(property);
        assert(variant);

        error = NULL;
        r = systemd_call_sync(connection,
                              name,
                              path,
                              iface,
                              method,
                              g_variant_new("(ss)",
                                            interface,
                                            property),
                              variant,
                              &error);

        if (error) {
                if (err_msg)
                        ERR_MSG_DUP(*err_msg, error->message);

                g_error_free(error);
        }

        return r;
}

static int systemd_get_manager_property(GDBusConnection *connection,
                                        const char *iface,
                                        const char *property,
                                        GVariant **variant,
                                        char **err_msg) {

        assert(iface);
        assert(property);
        assert(variant);

        return systemd_get_property(connection,
                                    DBUS_SYSTEMD_BUSNAME,
                                    DBUS_SYSTEMD_PATH,
                                    DBUS_INTERFACE_DBUS_PROPERTIES,
                                    "Get",
                                    iface,
                                    property,
                                    variant,
                                    err_msg);
}

static int systemd_get_unit_property(GDBusConnection *connection,
                                     const char *unit,
                                     const char *property,
                                     GVariant **variant,
                                     char **err_msg) {

        _cleanup_free_ char *systemd_unit_obj_path = NULL;
        int r;

        assert(unit);
        assert(property);
        assert(variant);

        r = systemd_get_unit(connection, unit, &systemd_unit_obj_path, err_msg);
        if (r < 0)
                return r;

        return systemd_get_property(connection,
                                    DBUS_SYSTEMD_BUSNAME,
                                    systemd_unit_obj_path,
                                    DBUS_INTERFACE_DBUS_PROPERTIES,
                                    "Get",
                                    DBUS_SYSTEMD_INTERFACE_UNIT,
                                    property,
                                    variant,
                                    err_msg);
}

static int systemd_get_service_property(GDBusConnection *connection,
                                        const char *unit,
                                        const char *property,
                                        GVariant **variant,
                                        char **err_msg) {

        _cleanup_free_ char *systemd_unit_obj_path = NULL;
        int r;

        assert(unit);
        assert(property);
        assert(variant);

        r = systemd_get_unit(connection, unit, &systemd_unit_obj_path, err_msg);
        if (r < 0)
                return r;

        return systemd_get_property(connection,
                                    DBUS_SYSTEMD_BUSNAME,
                                    systemd_unit_obj_path,
                                    DBUS_INTERFACE_DBUS_PROPERTIES,
                                    "Get",
                                    DBUS_SYSTEMD_INTERFACE_SERVICE,
                                    property,
                                    variant,
                                    err_msg);
}

#define g_variant_get_function_int32_t(v)   g_variant_get_int32(v)
#define g_variant_get_function_int64_t(v)   g_variant_get_int64(v)
#define g_variant_get_function_uint32_t(v)  g_variant_get_uint32(v)
#define g_variant_get_function_uint64_t(v)  g_variant_get_uint64(v)
#define g_variant_get_function_string(v)  g_variant_dup_string(v, NULL)
#define g_variant_get_function_strv(v)    g_variant_dup_strv(v, NULL)

#define DEFINE_SYSTEMD_GET_PROPERTY(iface, type, value)                 \
        int systemd_get_##iface##_property_as_##type(                   \
                        GDBusConnection *connection,                    \
                        const char* target,                             \
                        const char* property,                           \
                        value* result,                                  \
                        char** err_msg) {                               \
                                                                        \
                g_autoptr(GVariant) var = NULL, inner = NULL;           \
                int r;                                                  \
                                                                        \
                assert(target);                                         \
                assert(property);                                       \
                assert(result);                                         \
                                                                        \
                r = systemd_get_##iface##_property(connection,          \
                                                   target,              \
                                                   property,            \
                                                   &var,                \
                                                   err_msg);            \
                if (r < 0)                                              \
                        return r;                                       \
                                                                        \
                g_assert(g_variant_is_of_type(var, G_VARIANT_TYPE("(v)"))); \
                g_variant_get(var, g_variant_get_type_string(var), &inner); \
                *result = g_variant_get_function_##type(inner);         \
                                                                        \
                return 0;                                               \
        }

DEFINE_SYSTEMD_GET_PROPERTY(manager, int32_t, int32_t)
DEFINE_SYSTEMD_GET_PROPERTY(manager, uint32_t, uint32_t)
DEFINE_SYSTEMD_GET_PROPERTY(manager, int64_t, int64_t)
DEFINE_SYSTEMD_GET_PROPERTY(manager, uint64_t, uint64_t)
DEFINE_SYSTEMD_GET_PROPERTY(manager, string, char*)
DEFINE_SYSTEMD_GET_PROPERTY(manager, strv, char**)
DEFINE_SYSTEMD_GET_PROPERTY(unit, int32_t, int32_t)
DEFINE_SYSTEMD_GET_PROPERTY(unit, uint32_t, uint32_t)
DEFINE_SYSTEMD_GET_PROPERTY(unit, int64_t, int64_t)
DEFINE_SYSTEMD_GET_PROPERTY(unit, uint64_t, uint64_t)
DEFINE_SYSTEMD_GET_PROPERTY(unit, string, char*)
DEFINE_SYSTEMD_GET_PROPERTY(unit, strv, char**)
DEFINE_SYSTEMD_GET_PROPERTY(service, int32_t, int32_t)
DEFINE_SYSTEMD_GET_PROPERTY(service, uint32_t, uint32_t)
DEFINE_SYSTEMD_GET_PROPERTY(service, int64_t, int64_t)
DEFINE_SYSTEMD_GET_PROPERTY(service, uint64_t, uint64_t)
DEFINE_SYSTEMD_GET_PROPERTY(service, string, char*)
DEFINE_SYSTEMD_GET_PROPERTY(service, strv, char**)

enum SystemdUnitType systemd_get_unit_type_from_name(const char *unit) {
        enum SystemdUnitType type = _SYSTEMD_UNIT_TYPE_INVALID;
        enum SystemdUnitType t;
        const char *e;

        assert(unit);

        e = strrchr(unit, '.');
        if (!e)
                return _SYSTEMD_UNIT_TYPE_INVALID;

        for (t = 0; t < _SYSTEMD_UNIT_TYPE_MAX; t++) {
                if (endswith(e + 1, systemd_unit_type_string[t])) {
                        type = t;
                        break;
                }
        }

        return type;
}

int systemd_get_service_main_pid(GDBusConnection *connection,
                                 const char *unit,
                                 pid_t *pid,
                                 char **err_msg) {
        uint32_t u;
        int r;

        assert(unit);
        assert(pid);

        r = systemd_get_service_property_as_uint32_t(connection,
                                                     unit,
                                                     "ExecMainPID",
                                                     &u,
                                                     err_msg);
        if (r < 0)
                return r;

        *pid = (pid_t) u;

        return 0;
}

void systemd_unit_status_free(struct systemd_unit_status *unit_status) {

        if (!unit_status)
                return;

        if (unit_status->name)
                free(unit_status->name);

        if (unit_status->description)
                free(unit_status->description);

        if (unit_status->load_state)
                free(unit_status->load_state);

        if (unit_status->active_state)
                free(unit_status->active_state);

        if (unit_status->sub_state)
                free(unit_status->sub_state);

        if (unit_status->followed)
                free(unit_status->followed);

        if (unit_status->obj_path)
                free(unit_status->obj_path);

        if (unit_status->job_type)
                free(unit_status->job_type);

        if (unit_status->job_obj_path)
                free(unit_status->job_obj_path);

        free(unit_status);
}

static void systemd_unit_status_destroy_notify(gpointer data) {
        systemd_unit_status_free((struct systemd_unit_status *)data);
}

void systemd_unit_status_list_free_full(GList *status_list) {

        if (!status_list)
                return;

        g_list_free_full(status_list, systemd_unit_status_destroy_notify);
}

static int systemd_parse_list_units_result(GVariant *result, GList **unit_list) {
        char *name, *description, *load_state, *active_state, *sub_state;
        char *followed, *obj_path, *job_type, *job_obj_path;
        g_autoptr(GVariantIter) iter;
        unsigned int job_id;
        GList *list = NULL;
        int r;

        g_variant_get(result, "(a(ssssssouso))", &iter);

        while (g_variant_iter_loop(iter,
                                   "(ssssssouso)",
                                   &name,
                                   &description,
                                   &load_state,
                                   &active_state,
                                   &sub_state,
                                   &followed,
                                   &obj_path,
                                   &job_id,
                                   &job_type,
                                   &job_obj_path)) {

                struct systemd_unit_status *unit_status = NULL;

                unit_status = new0(struct systemd_unit_status, 1);
                if (!unit_status) {
                        r = -ENOMEM;
                        goto on_error;
                }

                /*
                 * At the first, prepend unit_status to list. If error
                 * occurred, we have to destroy the list fully. To
                 * easily free this unit_status also, prepend this
                 * unit_list to the list.
                 */
                list = g_list_prepend(list, unit_status);

                if (name) {
                        unit_status->name = strdup(name);
                        if (!unit_status->name) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }

                if (description) {
                        unit_status->description = strdup(description);
                        if (!unit_status->description) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }

                if (load_state) {
                        unit_status->load_state = strdup(load_state);
                        if (!unit_status->load_state) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }

                if (active_state) {
                        unit_status->active_state = strdup(active_state);
                        if (!unit_status->active_state) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }
                if (sub_state) {
                        unit_status->sub_state = strdup(sub_state);
                        if (!unit_status->sub_state) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }

                if (followed) {
                        unit_status->followed = strdup(followed);
                        if (!unit_status->followed) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }

                if (obj_path) {
                        unit_status->obj_path = strdup(obj_path);
                        if (!unit_status->obj_path) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }

                unit_status->job_id = job_id;

                if (job_type) {
                        unit_status->job_type = strdup(job_type);
                        if (!unit_status->job_type) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }

                if (job_obj_path) {
                        unit_status->job_obj_path = strdup(job_obj_path);
                        if (!unit_status->job_obj_path) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }
        }

        *unit_list = list;

        return 0;

on_error:
        if (list)
                systemd_unit_status_list_free_full(list);

        return r;
}

int systemd_get_units_list(GDBusConnection *conn, GList **unit_list, char **err_msg) {
        g_autoptr(GVariant) reply = NULL;
        GError *error;
        int r;

        assert(unit_list);

        error = NULL;
        r = systemd_call_sync(conn,
                              DBUS_SYSTEMD_BUSNAME,
                              DBUS_SYSTEMD_PATH,
                              DBUS_SYSTEMD_INTERFACE_MANAGER,
                              "ListUnits",
                              NULL,
                              &reply,
                              &error);
        if (error) {
                if (err_msg)
                        ERR_MSG_DUP(*err_msg, error->message);

                g_error_free(error);

                return r;
        }

        if (!g_variant_is_of_type(reply, G_VARIANT_TYPE("(a(ssssssouso))"))) {
                if (err_msg)
                        *err_msg = strdup("systemd ListUnits() reply message is not type of (a(ssssssouso))");

                return -EBADMSG;
        }

        return systemd_parse_list_units_result(reply, unit_list);
}


void systemd_unit_file_status_free(struct systemd_unit_file_status *unit_file_status) {

        if (!unit_file_status)
                return;

        if (unit_file_status->name)
                free(unit_file_status->name);

        if (unit_file_status->status)
                free(unit_file_status->status);

        free(unit_file_status);
}

static void systemd_unit_file_status_destroy_notify(gpointer data) {
        systemd_unit_file_status_free((struct systemd_unit_file_status *)data);
}

void systemd_unit_file_status_list_free_full(GList *status_list) {
        if (status_list)
                return;

        g_list_free_full(status_list, systemd_unit_file_status_destroy_notify);
}

static int systemd_parse_list_unit_files_result(GVariant *result, GList **unit_files_list) {
        g_autoptr(GVariantIter) iter;
        GList *list = NULL;
        char *name, *status;
        int r;

        g_variant_get(result, "(a(ss))", &iter);

        while (g_variant_iter_loop(iter,
                                   "(ss)",
                                   &name,
                                   &status)) {

                struct systemd_unit_file_status *unit_file_status = NULL;

                unit_file_status = new0(struct systemd_unit_file_status, 1);
                if (!unit_file_status) {
                        r = -ENOMEM;
                        goto on_error;
                }

                /*
                 * At the first, prepend unit_file_status to list. If
                 * error occurred, we have to destroy the list
                 * fully. To easily free this unit_file_status also,
                 * prepend this unit_file_status to the list.
                 */
                list = g_list_prepend(list, unit_file_status);

                if (name) {
                        unit_file_status->name = strdup(name);
                        if (!unit_file_status->name) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }

                if (status) {
                        unit_file_status->status = strdup(status);
                        if (!unit_file_status->status) {
                                r = -ENOMEM;
                                goto on_error;
                        }
                }
        }

        *unit_files_list = list;

        return 0;

on_error:
        if (list)
                systemd_unit_file_status_list_free_full(list);

        return r;
}

int systemd_get_unit_files_list(GDBusConnection *conn, GList **unit_files_list, char **err_msg) {
        g_autoptr(GVariant) reply = NULL;
        GError *error;
        int r;

        assert(conn);
        assert(unit_files_list);

        error = NULL;
        r = systemd_call_sync(conn,
                              DBUS_SYSTEMD_BUSNAME,
                              DBUS_SYSTEMD_PATH,
                              DBUS_SYSTEMD_INTERFACE_MANAGER,
                              "ListUnitFiles",
                              NULL,
                              &reply,
                              &error);
        if (error) {
                if (err_msg)
                        ERR_MSG_DUP(*err_msg, error->message);

                g_error_free(error);

                return r;
        }

        if (!g_variant_is_of_type(reply, G_VARIANT_TYPE("(a(ss))"))) {
                if (err_msg)
                        *err_msg = strdup("systemd ListUnitFiles() reply message is not type of (a(ss))");

                return -EBADMSG;
        }

        return systemd_parse_list_unit_files_result(reply, unit_files_list);
}

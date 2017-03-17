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

/**
 * @file systemd.h
 *
 * systemd utility library
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 */

#pragma once

#include <stdint.h>
#include <dbus/dbus.h>
#include <gio/gio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup SYSTEMD_HELPER_GROUP systemd Helper
 *
 * @brief systemd utility library
 * @{
 */

/**
 * enumerate systemd unit type
 */
enum SystemdUnitType {
        /** service type systemd unit */
        SYSTEMD_UNIT_SERVICE = 0,
        /** socket type systemd unit */
        SYSTEMD_UNIT_SOCKET,
        /** device type systemd unit */
        SYSTEMD_UNIT_DEVICE,
        /** mount type systemd unit */
        SYSTEMD_UNIT_MOUNT,
        /** automount type systemd unit */
        SYSTEMD_UNIT_AUTOMOUNT,
        /** swap type systemd unit */
        SYSTEMD_UNIT_SWAP,
        /** target type systemd unit */
        SYSTEMD_UNIT_TARGET,
        /** path type systemd unit */
        SYSTEMD_UNIT_PATH,
        /** timer type systemd unit */
        SYSTEMD_UNIT_TIMER,
        /** snapshot type systemd unit */
        SYSTEMD_UNIT_SNAPSHOT,
        /** slice type systemd unit */
        SYSTEMD_UNIT_SLICE,
        /** scope type systemd unit */
        SYSTEMD_UNIT_SCOPE,
        /** maxium number of this enum */
        _SYSTEMD_UNIT_TYPE_MAX,
        /** invalid type systemd unit */
        _SYSTEMD_UNIT_TYPE_INVALID = -1
};

/** "org.freedesktop.systemd1" */
#define DBUS_SYSTEMD_BUSNAME            "org.freedesktop.systemd1"
/** "org.freedesktop.systemd1.Manager" */
#define DBUS_SYSTEMD_INTERFACE_MANAGER  DBUS_SYSTEMD_BUSNAME ".Manager"
/** "org.freedesktop.systemd1.Unit" */
#define DBUS_SYSTEMD_INTERFACE_UNIT     DBUS_SYSTEMD_BUSNAME ".Unit"
/** "org.freedesktop.systemd1.Service" */
#define DBUS_SYSTEMD_INTERFACE_SERVICE  DBUS_SYSTEMD_BUSNAME ".Service"
/** "org.freedesktop.systemd1.Target" */
#define DBUS_SYSTEMD_INTERFACE_TARGET   DBUS_SYSTEMD_BUSNAME ".Target"

/** "/org/freedesktop/systemd1" */
#define DBUS_SYSTEMD_PATH               "/org/freedesktop/systemd1"
/** "/org/freedesktop/systemd1/unit" */
#define DBUS_SYSTEMD_UNIT_PATH          DBUS_SYSTEMD_PATH "/unit"
/** "/org/freedesktop/systemd1/unit/" */
#define DBUS_SYSTEMD_UNIT_PATH_PREFIX   DBUS_SYSTEMD_UNIT_PATH "/"

/**
 * @brief Subscribe systemd signals.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_subscribe(GDBusConnection *connection, char **err_msg);

/**
 * @brief Subscribe systemd signals.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_unsubscribe(GDBusConnection *connection, char **err_msg);

/**
 * @brief Get unit DBus object path.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param name systemd unit name
 * @param unit unit object path is filled on success. NULL on
 *   failure. If this value is returned with not NULL, this value has
 *   to be free-ed by caller.
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_unit(GDBusConnection *connection, const char *name, char **unit, char **err_msg);

/**
 * @brief Controls systemd unit. Internally, use method call to
 *   systemd. systemd unit control method calls are only allowed to
 *   root uid. So the caller of this api is also run with root uid.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param method systemd unit control DBus method call name
 * @param name systemd unit name
 * @param job systemd job object path is filled such like
 *   "/org/freedesktop/systemd1/job/2416".
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_control_unit(GDBusConnection *connection, const char *method, const char *name, char **job, char **err_msg);

/**
 * @brief Start systemd unit. Internally, use systemd "StartUnit"
 *   method call to systemd. systemd unit control method calls are
 *   only allowed to root uid. So the caller of this api is also run
 *   with root uid.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param name systemd unit name
 * @param job systemd job object path is filled such like
 *   "/org/freedesktop/systemd1/job/2416".
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_start_unit(GDBusConnection *connection, const char *name, char **job, char **err_msg);

/**
 * @brief Stop systemd unit. Internally, use systemd "StopUnit"
 *   method call to systemd. systemd unit control method calls are
 *   only allowed to root uid. So the caller of this api is also run
 *   with root uid.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param name systemd unit name
 * @param job systemd job object path is filled such like
 *   "/org/freedesktop/systemd1/job/2416".
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_stop_unit(GDBusConnection *connection, const char *name, char **job, char **err_msg);

/**
 * @brief Reload systemd unit. Internally, use systemd "ReloadUnit"
 *   method call to systemd. systemd unit control method calls are
 *   only allowed to root uid. So the caller of this api is also run
 *   with root uid.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param name systemd unit name
 * @param job systemd job object path is filled such like
 *   "/org/freedesktop/systemd1/job/2416".
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_reload_unit(GDBusConnection *connection, const char *name, char **job, char **err_msg);

/**
 * @brief Restart systemd unit. Internally, use systemd "RestartUnit"
 *   method call to systemd. systemd unit control method calls are
 *   only allowed to root uid. So the caller of this api is also run
 *   with root uid.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param name systemd unit name
 * @param job systemd job object path is filled such like
 *   "/org/freedesktop/systemd1/job/2416".
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_restart_unit(GDBusConnection *connection, const char *name, char **job, char **err_msg);

/**
 * @brief Try to restart systemd unit. Internally, use systemd
 *   "TryRestartUnit" method call to systemd. Different with
 *   RestartUnit, if the unit is not running, the unit is not started.
 *   systemd unit control method calls are only allowed to root
 *   uid. So the caller of this api is also run with root uid.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param name systemd unit name
 * @param job systemd job object path is filled such like
 *   "/org/freedesktop/systemd1/job/2416".
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_try_restart_unit(GDBusConnection *connection, const char *name, char **job, char **err_msg);

/**
 * @brief Reload(if unit support) and restart systemd
 *   unit. Internally, use systemd "ReloadOrRestartUnit" method call
 *   to systemd. systemd unit control method calls are only allowed to
 *   root uid. So the caller of this api is also run with root uid.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param name systemd unit name
 * @param job systemd job object path is filled such like
 *   "/org/freedesktop/systemd1/job/2416".
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_reload_or_restart_unit(GDBusConnection *connection, const char *name, char **job, char **err_msg);

/**
 * @brief Reload(if unit support) and try to restart systemd
 *   unit. Internally, use systemd "ReloadOrTryRestartUnit" method
 *   call to systemd. Similar to "TryRestartUnit", if the unit is not
 *   running, the unit is not restarted. systemd unit control method
 *   calls are only allowed to root uid. So the caller of this api is
 *   also run with root uid.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param name systemd unit name
 * @param job systemd job object path is filled such like
 *   "/org/freedesktop/systemd1/job/2416".
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_reload_or_try_restart_unit(GDBusConnection *connection, const char *name, char **job, char **err_msg);

/**
 * @brief Get systemd manager int32 type(int) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param iface systemd manager interface. Generally
 *   DBUS_SYSTEMD_INTERFACE_MANAGER can be used.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_manager_property_as_int32_t(GDBusConnection *connection, const char *iface, const char *property, int32_t *result, char **err_msg);

/**
 * @brief Get systemd manager uint32 type(unsigned int) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param iface systemd manager interface. Generally
 *   DBUS_SYSTEMD_INTERFACE_MANAGER can be used.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_manager_property_as_uint32_t(GDBusConnection *connection, const char *iface, const char *property, uint32_t *result, char **err_msg);

/**
 * @brief Get systemd manager int64 type(long long) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param iface systemd manager interface. Generally
 *   DBUS_SYSTEMD_INTERFACE_MANAGER can be used.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_manager_property_as_int64_t(GDBusConnection *connection, const char *iface, const char *property, int64_t *result, char **err_msg);

/**
 * @brief Get systemd manager uint64 type(unsigned long long) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param iface systemd manager interface. Generally
 *   DBUS_SYSTEMD_INTERFACE_MANAGER can be used.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_manager_property_as_uint64_t(GDBusConnection *connection, const char *iface, const char *property, uint64_t *result, char **err_msg);

/**
 * @brief Get systemd manager string type property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param iface systemd manager interface. Generally
 *   DBUS_SYSTEMD_INTERFACE_MANAGER can be used.
 * @param property Property name
 * @param result Duplicated property result string. This value has to
 *   be free-ed by caller.
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_manager_property_as_string(GDBusConnection *connection, const char *iface, const char *property, char **result, char **err_msg);

/**
 * @brief Get systemd manager string list type property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param iface systemd manager interface. Generally
 *   DBUS_SYSTEMD_INTERFACE_MANAGER can be used.
 * @param property Property name
 * @param result Duplicated string list. This string list has to be
 *   free-ed by caller.
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_manager_property_as_strv(GDBusConnection *connection, const char *iface, const char *property, char ***result, char **err_msg);

/**
 * @brief Get systemd unit int32 type(int) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_unit_property_as_int32_t(GDBusConnection *connection, const char *unit, const char *property, int32_t *result, char **err_msg);

/**
 * @brief Get systemd unit uint32 type(unsigned int) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_unit_property_as_uint32_t(GDBusConnection *connection, const char *unit, const char *property, uint32_t *result, char **err_msg);

/**
 * @brief Get systemd unit int64 type(long long) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_unit_property_as_int64_t(GDBusConnection *connection, const char *unit, const char *property, int64_t *result, char **err_msg);

/**
 * @brief Get systemd unit uint64 type(unsigned long long) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_unit_property_as_uint64_t(GDBusConnection *connection, const char *unit, const char *property, uint64_t *result, char **err_msg);

/**
 * @brief Get systemd unit string type property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name
 * @param property Property name
 * @param result Duplicated property result string. This value has to
 *   be free-ed by caller.
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_unit_property_as_string(GDBusConnection *connection, const char *unit, const char *property, char **result, char **err_msg);

/**
 * @brief Get systemd unit string list type property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name
 * @param property Property name
 * @param result Duplicated string list. This string list has to be
 *   free-ed by caller.
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_unit_property_as_strv(GDBusConnection *connection, const char *unit, const char *property, char ***result, char **err_msg);

/**
 * @brief Get systemd service int32 type(int) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_service_property_as_int32_t(GDBusConnection *connection, const char *unit, const char *property, int32_t *result, char **err_msg);

/**
 * @brief Get systemd service int32 type(int) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_service_property_as_uint32_t(GDBusConnection *connection, const char *unit, const char *property, uint32_t *result, char **err_msg);

/**
 * @brief Get systemd service int32 type(int) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_service_property_as_int64_t(GDBusConnection *connection, const char *unit, const char *property, int64_t *result, char **err_msg);

/**
 * @brief Get systemd service int32 type(int) property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name.
 * @param property Property name
 * @param result Property get result
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_service_property_as_uint64_t(GDBusConnection *connection, const char *unit, const char *property, uint64_t *result, char **err_msg);

/**
 * @brief Get systemd service string type property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name
 * @param property Property name
 * @param result Duplicated property result string. This value has to
 *   be free-ed by caller.
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_service_property_as_string(GDBusConnection *connection, const char *unit, const char *property, char **result, char **err_msg);

/**
 * @brief Get systemd service string list type property.
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd unit name
 * @param property Property name
 * @param result Duplicated string list. This string list has to be
 *   free-ed by caller.
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_service_property_as_strv(GDBusConnection *connection, const char *unit, const char *property, char ***result, char **err_msg);

/**
 * @brief Get systemd unit type from unit name
 *
 * @param unit name
 *
 * @return a #SystemdUnitType. _SYSTEMD_UNIT_TYPE_INVALID for unknown.
 */
enum SystemdUnitType systemd_get_unit_type_from_name(const char *unit);

/**
 * @brief Get systemd service main pid
 *
 * @param connection GDBus connection or NULL. If connection is NULL,
 *   this is working with GDBusProxy.
 * @param unit systemd service name
 * @param pid main pid variable pointer
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_service_main_pid(GDBusConnection *connection, const char *unit, pid_t *pid, char **err_msg);

/**
 * systemd unit status
 */
struct systemd_unit_status {
        /**
         * The primary unit name as string
         */
        char *name;

        /**
         * The human readable description string
         */
        char *description;

        /**
         * The load state (i.e. whether the unit file has been loaded
         * successfully)
         */
        char *load_state;

        /**
         * The active state (i.e. whether the unit is currently
         * started or not)
         */
        char *active_state;

        /**
         * The sub state (a more fine-grained version of the active
         * state that is specific to the unit type, which the active
         * state is not)
         */
        char *sub_state;

        /**
         * A unit that is being followed in its state by this unit, if
         * there is any, otherwise the empty string.
         */
        char *followed;

        /**
         * The unit object path
         */
        char *obj_path;

        /**
         * If there is a job queued for the job unit the numeric job
         * id, 0 otherwise
         */
        unsigned int job_id;

        /**
         * The job type as string
         */
        char *job_type;

        /**
         * The job object path
         */
        char *job_obj_path;
};

/**
 * @brief free #systemd_unit_status struct
 *
 * @param unit_status a #systemd_unit_status struct
 */
void systemd_unit_status_free(struct systemd_unit_status *unit_status);

/**
 * @brief free all #systemd_unit_status struct list
 *
 * @param status_list #systemd_unit_status struct list to free
 */
void systemd_unit_status_list_free_full(GList *status_list);

/**
 * @brief Get currently loaded systemd unit list.
 *
 * @param conn GDBus connection or NULL. If connection is NULL, this
 *   is working with GDBusProxy.
 * @param unit_list loaded systemd unit list is stored to here on
 *   success. This list has to be destroied by called after use with
 *   #systemd_unit_status_list_free_full().
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_units_list(GDBusConnection *conn, GList **unit_list, char **err_msg);

/**
 * systemd unit file status
 */
struct systemd_unit_file_status {
        /**
         * The primary unit name as string
         */
        char *name;

        /**
         * The primary unit name as string
         */
        char *status;
};

/**
 * @brief free #systemd_unit_file_status struct
 *
 * @param unit_file_status a #systemd_unit_file_status struct
 */
void systemd_unit_file_status_free(struct systemd_unit_file_status *unit_file_status);

/**
 * @brief free all #systemd_unit_file_status struct list
 *
 * @param status_list #systemd_unit_file_status struct list to free
 */
void systemd_unit_file_status_list_free_full(GList *status_list);

/**
 * @brief Get an array of unit names plus their enablement
 * status. Note that #systemd_get_units_list() returns a list of units
 * currently loaded into memory, while #systemd_get_unit_files_list()
 * returns a list of unit files that could be found on disk.
 *
 * @param conn GDBus connection or NULL. If connection is NULL, this
 *   is working with GDBusProxy.  @param unit_files_list unit files
 *   list what can be found by systemd. This list has to be destroied
 *   by called after use with
 *   #systemd_unit_file_status_list_free_full().
 * @param err_msg NULL is filled on success, error message is filled
 *   on failure. This value has to be free-ed by caller.
 *
 * @return 0 on success, -errno on failure.
 */
int systemd_get_unit_files_list(GDBusConnection *conn, GList **unit_files_list, char **err_msg);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>

#include "libsystem.h"
#include "proc.h"

ssize_t proc_cmdline_get_str(char **buf, const char *op) {
        _cleanup_free_ char *cmdline = NULL;
        char *s, *w, *state;
        size_t l, ll;
        int r;

        assert(buf);
        assert(op);

        r = read_one_line_from_path("/proc/cmdline", &cmdline);
        if (r < 0)
                return r;

        ll = strlen(op);
        FOREACH_WORD(w, l, cmdline, state)
                if (strneq(op, w, ll)) {
                        s = new0(char, l - ll + 1);
                        if (!s)
                                return -ENOMEM;

                        strncpy(s, w + ll, l - ll + 1);

                        *buf = s;

                        return l - ll + 1;
                }

        return -ENOENT;
}

/* In old kernel, this symbol maybe NOT */
#ifndef TASK_COMM_LEN
#define TASK_COMM_LEN 16
#endif

int proc_pid_of(const char *pname) {
        _cleanup_closedir_ DIR *dir = NULL;
        struct dirent *de;
        int r;

        dir = opendir("/proc");
        if (!dir)
                return -errno;

        FOREACH_DIRENT(de, dir, return -errno) {
                _cleanup_free_ char *path = NULL;
                _cleanup_free_ char *comm = NULL;

                if (de->d_type != DT_DIR)
                        continue;

                if (!is_number(de->d_name, strlen(de->d_name)))
                        continue;

                r = asprintf(&path, "/proc/%s/comm", de->d_name);
                if (r < 0)
                        return -ENOMEM;

                r = read_one_line_from_path(path, &comm);
                if (r < 0)
                        continue;

                if (strneq(pname, comm, TASK_COMM_LEN - 1))
                        return atoi(de->d_name);
        }

        return 0;
}

static void smap_free(struct smap *map) {
        if (!map)
                return;

        if (map->mode)
                free(map->mode);

        if (map->name)
                free(map->name);

        free(map);
}

static inline void smap_freep(struct smap **map) {
        if (*map)
                smap_free(*map);
}

#define _cleanup_smap_free_ _cleanup_ (smap_freep)

void smaps_free(struct smaps *maps) {
        int i;

        if (!maps)
                return;

        for (i = 0; i < maps->n_map; i++)
                smap_free(maps->maps[i]);

        free(maps->maps);
        free(maps);
}

static int add_smap_to_smaps(struct smaps *maps, struct smap *map) {
        int i;

        assert(maps);
        assert(map);

        maps->n_map++;

        maps->maps = (struct smap **) realloc(maps->maps, sizeof(struct smap *) * maps->n_map);
        if (!maps->maps)
                return -ENOMEM;

        maps->maps[maps->n_map - 1] = map;

        for (i = 0; i < SMAPS_ID_MAX; i++)
                maps->sum[i] += map->value[i];

        return 0;
}

int proc_pid_get_smaps(pid_t pid, struct smaps **maps, enum smap_mask mask) {
        _cleanup_smaps_free_ struct smaps *m = NULL;
        _cleanup_free_ char *path = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        char buf[LINE_MAX];
        bool get_line = true;
        int r;

        assert(maps);

        r = asprintf(&path, "/proc/%d/smaps", pid);
        if (r < 0)
                return -ENOMEM;

        r = access(path, F_OK);
        if (r < 0)
                return -errno;

        f = fopen(path, "re");
        if (!f)
                return -errno;

        m = new0(struct smaps, 1);
        if (!m)
                return -ENOMEM;

        for (;;) {
                _cleanup_smap_free_ struct smap *map = NULL;
                int n;

                if (get_line && !fgets(buf, sizeof(buf), f)) {
                        if (ferror(f)) {
                                r = -errno;
                                goto on_error;
                        }
                        break;
                } else
                        get_line = true;

                map = new0(struct smap, 1);
                if (!map) {
                        r = -errno;
                        goto on_error;
                }

                n = sscanf(buf, "%x-%x %ms %*s %*s %*s %ms", &map->start, &map->end, &map->mode, &map->name);

                if (n == 3 && !map->name)
                        map->name = strdup("[anon]");
                else if (n != 4) {
                        r = -EINVAL;
                        goto on_error;
                }

                for (;;) {
                        unsigned int v = 0;
                        enum smap_id id;
                        size_t l;

                        if (!fgets(buf, sizeof(buf), f)) {
                                if (ferror(f)) {
                                        r = -errno;
                                        goto on_error;
                                }
                                break;
                        }

                        if ((*buf >= '0' && *buf <= '9') || (*buf >= 'a' && *buf <= 'f')) {
                                get_line = false;
                                break;
                        }

                        l = strcspn(buf, ":");
                        if (!l)
                                break;

                        buf[l] = 0;

                        id = smap_string_to_id(buf);
                        if (id < 0 || id >= SMAPS_ID_MAX)
                                continue;

                        if (!(mask & (1 << id)))
                                continue;

                        if (sscanf(buf + l + 1, "%d kB", &v) != 1)
                                break;

                        map->value[id] = v;
                }

                r = add_smap_to_smaps(m, map);
                if (r < 0)
                        goto on_error;

                map = NULL;
        }

        *maps = m;
        m = NULL;

        return 0;

on_error:
        return r;
}

static const char* const meminfo_string_lookup[MEMINFO_ID_MAX] = {
        [MEMINFO_ID_MEM_TOTAL]     = "MemTotal",
        [MEMINFO_ID_MEM_FREE]      = "MemFree",
        [MEMINFO_ID_MEM_AVAILABLE] = "MemAvailable",
        [MEMINFO_ID_BUFFERS]       = "Buffers",
        [MEMINFO_ID_CACHED]        = "Cached",
        [MEMINFO_ID_SWAP_CACHED]   = "SwapCached",
        [MEMINFO_ID_ACTIVE]        = "Active",
        [MEMINFO_ID_INACTIVE]      = "Inactive",
        [MEMINFO_ID_ACTIVE_ANON]   = "Active(anon)",
        [MEMINFO_ID_INACTIVE_ANON] = "Inactive(anon)",
        [MEMINFO_ID_ACTIVE_FILE]   = "Active(file)",
        [MEMINFO_ID_INACTIVE_FILE] = "Inactive(file)",
        [MEMINFO_ID_UNEVICTABLE]   = "Unevictable",
        [MEMINFO_ID_MLOCKED]       = "Mlocked",
        [MEMINFO_ID_HIGH_TOTAL]    = "HighTotal",
        [MEMINFO_ID_HIGH_FREE]     = "HighFree",
        [MEMINFO_ID_LOW_TOTAL]     = "LowTotal",
        [MEMINFO_ID_LOW_FREE]      = "LowFree",
        [MEMINFO_ID_SWAP_TOTAL]    = "SwapTotal",
        [MEMINFO_ID_SWAP_FREE]     = "SwapFree",
        [MEMINFO_ID_DIRTY]         = "Dirty",
        [MEMINFO_ID_WRITEBACK]     = "Writeback",
        [MEMINFO_ID_ANON_PAGES]    = "AnonPages",
        [MEMINFO_ID_MAPPED]        = "Mapped",
        [MEMINFO_ID_SHMEM]         = "Shmem",
        [MEMINFO_ID_SLAB]          = "Slab",
        [MEMINFO_ID_SRECLAIMABLE]  = "SReclaimable",
        [MEMINFO_ID_SUNRECLAIM]    = "SUnreclaim",
        [MEMINFO_ID_KERNEL_STACK]  = "KernelStack",
        [MEMINFO_ID_PAGE_TABLES]   = "PageTables",
        [MEMINFO_ID_NFS_UNSTABLE]  = "NFS_Unstable",
        [MEMINFO_ID_BOUNCE]        = "Bounce",
        [MEMINFO_ID_WRITEBACK_TMP] = "WritebackTmp",
        [MEMINFO_ID_COMMIT_LIMIT]  = "CommitLimit",
        [MEMINFO_ID_COMMITTED_AS]  = "Committed_AS",
        [MEMINFO_ID_VMALLOC_TOTAL] = "VmallocTotal",
        [MEMINFO_ID_VMALLOC_USED]  = "VmallocUsed",
        [MEMINFO_ID_VMALLOC_CHUNK] = "VmallocChunk",
};

const char *meminfo_id_to_string(enum meminfo_id id) {
        assert(id >= 0 && id < MEMINFO_ID_MAX);

        return meminfo_string_lookup[id];
}

int proc_get_meminfo(struct meminfo *mi, enum meminfo_mask mask) {
        _cleanup_fclose_ FILE *f = NULL;
        enum meminfo_mask remain_mask = mask;
        char buf[LINE_MAX];

        assert(mi);

        memset(mi, 0x0, sizeof(struct meminfo));

        f = fopen("/proc/meminfo", "r");
        if (!f)
                return -errno;

        if (remain_mask & MEMINFO_MASK_MEM_AVAILABLE)
                remain_mask |= (MEMINFO_MASK_MEM_FREE | MEMINFO_MASK_CACHED);

        while (remain_mask) {
                unsigned int v = 0;
                enum meminfo_id id;
                size_t l;

                if (!fgets(buf, sizeof(buf), f)) {
                        if (ferror(f))
                                return -errno;
                        break;
                }

                l = strcspn(buf, ":");
                if (!l)
                        break;

                buf[l] = 0;

                id = meminfo_string_to_id(buf);
                if (id < 0 || id >= MEMINFO_ID_MAX)
                        continue;

                if (!(remain_mask & (1ULL << id)))
                        continue;

                remain_mask &= ~((1ULL << id));

                if (sscanf(buf + l + 1, "%d", &v) != 1)
                        break;

                mi->value[id] = v;
        }

        if (remain_mask & MEMINFO_MASK_MEM_AVAILABLE) {
                mi->value[MEMINFO_ID_MEM_AVAILABLE] = mi->value[MEMINFO_ID_MEM_FREE]
                        + mi->value[MEMINFO_ID_CACHED];
        }

        return 0;
}

void proc_buddyinfo_free(struct buddyinfo *bi) {
        if (!bi)
                return;

        free(bi->zone);
        free(bi);
}

int proc_get_buddyinfo(const char *zone, struct buddyinfo **bi) {
        _cleanup_fclose_ FILE *f = NULL;
        char buf[LINE_MAX];

        assert(zone);
        assert(bi);

        f = fopen("/proc/buddyinfo", "re");
        if (!f)
                return -errno;

        for (;;) {
                _cleanup_buddyinfo_free_ struct buddyinfo *b = NULL;
                int n;

                if (!fgets(buf, sizeof(buf), f)) {
                        if (ferror(f))
                                return -errno;

                        break;
                }

                b = new0(struct buddyinfo, 1);
                if (!b)
                        return -ENOMEM;

                n = sscanf(buf, "Node %d, zone %m[^ ] %d %d %d %d %d %d %d %d %d %d %d",
                           &b->node,
                           &b->zone,
                           &b->page[PAGE_4K],
                           &b->page[PAGE_8K],
                           &b->page[PAGE_16K],
                           &b->page[PAGE_32K],
                           &b->page[PAGE_64K],
                           &b->page[PAGE_128K],
                           &b->page[PAGE_256K],
                           &b->page[PAGE_512K],
                           &b->page[PAGE_1M],
                           &b->page[PAGE_2M],
                           &b->page[PAGE_4M]);
                if (n != 13)
                        break;

                if (!streq(zone, b->zone))
                        continue;

                *bi = b;
                b = NULL;

                return 0;
        }

        return -ENODATA;
}

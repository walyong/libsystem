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
 * @file proc.h
 *
 * procfs utility library
 *
 * Copyright (c) 2016 Samsung Electronics Co., Ltd. All rights reserved.
 *
 */

#pragma once

#include <sys/types.h>
#include "libsystem.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup PROC_GROUP proc group
 *
 * @brief A set utility library for /proc. Some of library functions
 * are only able to be successful root uid with security permissions.
 *
 * @{
 */

/**
 * @brief Get string with operator from /proc/cmdline. If foo=bar is
 * included in /proc/cmdline and want to get the bar, then:
 * \code{.c}
 char *buf;

 proc_cmdline_get_str(&buf, "foo=");
 * \endcode
 *
 * @param buf The value string is filled to here. This value has to be
 * free-ed by caller.
 * @param op An operator string.
 *
 * @return Result string. This value has to be free-ed by caller.
 */
ssize_t proc_cmdline_get_str(char **buf, const char *op);

/**
 * @brief Get PID of process.
 *
 * @param pname Process name.
 *
 * @return PID on successful find. If not found, 0 is returned. And
 * -errno is returned on failure.
 */
int proc_pid_of(const char *pname);

/**
 * smaps id
 */
enum smap_id {
        SMAPS_ID_INVALID = -1,
        SMAPS_ID_ANON_HUGE_PAGES = 0,
        SMAPS_ID_ANONYMOUS,
        SMAPS_ID_KERNEL_PAGE_SIZE,
        SMAPS_ID_LOCKED,
        SMAPS_ID_MMU_PAGE_SIZE,
        SMAPS_ID_PSWAP,
        SMAPS_ID_PRIVATE_CLEAN,
        SMAPS_ID_PRIVATE_DIRTY,
        SMAPS_ID_PSS,
        SMAPS_ID_REFERENCED,
        SMAPS_ID_RSS,
        SMAPS_ID_SHARED_CLEAN,
        SMAPS_ID_SHARED_DIRTY,
        SMAPS_ID_SIZE,
        SMAPS_ID_SWAP,
        SMAPS_ID_MAX,
};

/**
 * smaps mask
 */
enum smap_mask {
        SMAPS_MASK_ANON_HUGE_PAGES      = 1 << SMAPS_ID_ANON_HUGE_PAGES,
        SMAPS_MASK_ANONYMOUS            = 1 << SMAPS_ID_ANONYMOUS,
        SMAPS_MASK_KERNEL_PAGE_SIZE     = 1 << SMAPS_ID_KERNEL_PAGE_SIZE,
        SMAPS_MASK_LOCKED               = 1 << SMAPS_ID_LOCKED,
        SMAPS_MASK_MMU_PAGE_SIZE        = 1 << SMAPS_ID_MMU_PAGE_SIZE,
        SMAPS_MASK_PSWAP                = 1 << SMAPS_ID_PSWAP,
        SMAPS_MASK_PRIVATE_CLEAN        = 1 << SMAPS_ID_PRIVATE_CLEAN,
        SMAPS_MASK_PRIVATE_DIRTY        = 1 << SMAPS_ID_PRIVATE_DIRTY,
        SMAPS_MASK_PSS                  = 1 << SMAPS_ID_PSS,
        SMAPS_MASK_REFERENCED           = 1 << SMAPS_ID_REFERENCED,
        SMAPS_MASK_RSS                  = 1 << SMAPS_ID_RSS,
        SMAPS_MASK_SHARED_CLEAN         = 1 << SMAPS_ID_SHARED_CLEAN,
        SMAPS_MASK_SHARED_DIRTY         = 1 << SMAPS_ID_SHARED_DIRTY,
        SMAPS_MASK_SIZE                 = 1 << SMAPS_ID_SIZE,
        SMAPS_MASK_SWAP                 = 1 << SMAPS_ID_SWAP,
        SMAPS_MASK_ALL                  = (1 << SMAPS_ID_MAX) - 1,
        SMAPS_MASK_DEFAULT              = (SMAPS_MASK_SIZE |
                                           SMAPS_MASK_RSS |
                                           SMAPS_MASK_PSS |
                                           SMAPS_MASK_SHARED_CLEAN |
                                           SMAPS_MASK_SHARED_DIRTY |
                                           SMAPS_MASK_PRIVATE_CLEAN |
                                           SMAPS_MASK_PRIVATE_DIRTY |
                                           SMAPS_MASK_SWAP |
                                           SMAPS_MASK_PSWAP),
};

/**
 * a smap info
 */
struct smap {
        /**
         * start address
         */
        unsigned int start;
        /**
         * end address
         */
        unsigned int end;
        /**
         * smaps mode
         */
        char *mode;
        /**
         * smaps name
         */
        char *name;
        /**
         * value of each
         */
        unsigned int value[SMAPS_ID_MAX];
};

/**
 * a smaps info of pid
 */
struct smaps {
        /**
         * sum value of each
         */
        unsigned int sum[SMAPS_ID_MAX];
        /**
         * number of maps
         */
        int n_map;
        /**
         * maps
         */
        struct smap **maps;
};

/**
 * @brief Destroy struct smaps
 *
 * @param maps a smaps
 */
void smaps_free(struct smaps *maps);

static inline void smaps_freep(struct smaps **maps)
{
        if (*maps)
                smaps_free(*maps);
}

/**
 * Declare struct smaps with cleanup attribute. Allocated struct smaps
 * is destroyed on going out the scope.
 */
#define _cleanup_smaps_free_ _cleanup_ (smaps_freep)

/**
 * @brief Convert smap id to string
 *
 * @param id smap id
 *
 * @return converted string
 */
const char *smap_id_to_string(enum smap_id id);

/**
 * @brief Convert smap string to id
 *
 * @param str smap string
 *
 * @return converted id
 */
enum smap_id smap_string_to_id(const char *str);

/**
 * @brief Get smaps info of pid
 *
 * @param pid a pid to get
 * @param maps parsed smaps struct. This value has to be destoryed by
 * caller. #_cleanup_smaps_free_ is useful to make allocated struct to
 * autofree.
 * @code{.c}
 {
         _cleanup_smaps_free_ struct smaps *maps;

         proc_pid_get_smaps(pid, &maps, SMAPS_MASK_ALL);
 }
 * @endcode
 * @param mask mask to parse smaps.
 *
 * @return 0 on success, -errno on failure.
 */
int proc_pid_get_smaps(pid_t pid, struct smaps **maps, enum smap_mask mask);

/**
 * meminfo id
 */
enum meminfo_id {
        MEMINFO_ID_INVALID = -1,
        MEMINFO_ID_MEM_TOTAL = 0,
        MEMINFO_ID_MEM_FREE,
        MEMINFO_ID_MEM_AVAILABLE,
        MEMINFO_ID_BUFFERS,
        MEMINFO_ID_CACHED,
        MEMINFO_ID_SWAP_CACHED,
        MEMINFO_ID_ACTIVE,
        MEMINFO_ID_INACTIVE,
        MEMINFO_ID_ACTIVE_ANON,
        MEMINFO_ID_INACTIVE_ANON,
        MEMINFO_ID_ACTIVE_FILE,
        MEMINFO_ID_INACTIVE_FILE,
        MEMINFO_ID_UNEVICTABLE,
        MEMINFO_ID_MLOCKED,
        MEMINFO_ID_HIGH_TOTAL,
        MEMINFO_ID_HIGH_FREE,
        MEMINFO_ID_LOW_TOTAL,
        MEMINFO_ID_LOW_FREE,
        MEMINFO_ID_SWAP_TOTAL,
        MEMINFO_ID_SWAP_FREE,
        MEMINFO_ID_DIRTY,
        MEMINFO_ID_WRITEBACK,
        MEMINFO_ID_ANON_PAGES,
        MEMINFO_ID_MAPPED,
        MEMINFO_ID_SHMEM,
        MEMINFO_ID_SLAB,
        MEMINFO_ID_SRECLAIMABLE,
        MEMINFO_ID_SUNRECLAIM,
        MEMINFO_ID_KERNEL_STACK,
        MEMINFO_ID_PAGE_TABLES,
        MEMINFO_ID_NFS_UNSTABLE,
        MEMINFO_ID_BOUNCE,
        MEMINFO_ID_WRITEBACK_TMP,
        MEMINFO_ID_COMMIT_LIMIT,
        MEMINFO_ID_COMMITTED_AS,
        MEMINFO_ID_VMALLOC_TOTAL,
        MEMINFO_ID_VMALLOC_USED,
        MEMINFO_ID_VMALLOC_CHUNK,
        MEMINFO_ID_MAX,
};

/**
 * meminfo mask
 */
enum meminfo_mask {
        MEMINFO_MASK_MEM_TOTAL          = 1ULL << MEMINFO_ID_MEM_TOTAL,
        MEMINFO_MASK_MEM_FREE           = 1ULL << MEMINFO_ID_MEM_FREE,
        MEMINFO_MASK_MEM_AVAILABLE      = 1ULL << MEMINFO_ID_MEM_AVAILABLE,
        MEMINFO_MASK_BUFFERS            = 1ULL << MEMINFO_ID_BUFFERS,
        MEMINFO_MASK_CACHED             = 1ULL << MEMINFO_ID_CACHED,
        MEMINFO_MASK_SWAP_CACHED        = 1ULL << MEMINFO_ID_SWAP_CACHED,
        MEMINFO_MASK_ACTIVE             = 1ULL << MEMINFO_ID_ACTIVE,
        MEMINFO_MASK_INACTIVE           = 1ULL << MEMINFO_ID_INACTIVE,
        MEMINFO_MASK_ACTIVE_ANON        = 1ULL << MEMINFO_ID_ACTIVE_ANON,
        MEMINFO_MASK_INACTIVE_ANON      = 1ULL << MEMINFO_ID_INACTIVE_ANON,
        MEMINFO_MASK_ACTIVE_FILE        = 1ULL << MEMINFO_ID_ACTIVE_FILE,
        MEMINFO_MASK_INACTIVE_FILE      = 1ULL << MEMINFO_ID_INACTIVE_FILE,
        MEMINFO_MASK_UNEVICTABLE        = 1ULL << MEMINFO_ID_UNEVICTABLE,
        MEMINFO_MASK_MLOCKED            = 1ULL << MEMINFO_ID_MLOCKED,
        MEMINFO_MASK_HIGH_TOTAL         = 1ULL << MEMINFO_ID_HIGH_TOTAL,
        MEMINFO_MASK_HIGH_FREE          = 1ULL << MEMINFO_ID_HIGH_FREE,
        MEMINFO_MASK_LOW_TOTAL          = 1ULL << MEMINFO_ID_LOW_TOTAL,
        MEMINFO_MASK_LOW_FREE           = 1ULL << MEMINFO_ID_LOW_FREE,
        MEMINFO_MASK_SWAP_TOTAL         = 1ULL << MEMINFO_ID_SWAP_TOTAL,
        MEMINFO_MASK_SWAP_FREE          = 1ULL << MEMINFO_ID_SWAP_FREE,
        MEMINFO_MASK_DIRTY              = 1ULL << MEMINFO_ID_DIRTY,
        MEMINFO_MASK_WRITEBACK          = 1ULL << MEMINFO_ID_WRITEBACK,
        MEMINFO_MASK_ANON_PAGES         = 1ULL << MEMINFO_ID_ANON_PAGES,
        MEMINFO_MASK_MAPPED             = 1ULL << MEMINFO_ID_MAPPED,
        MEMINFO_MASK_SHMEM              = 1ULL << MEMINFO_ID_SHMEM,
        MEMINFO_MASK_SLAB               = 1ULL << MEMINFO_ID_SLAB,
        MEMINFO_MASK_SRECLAIMABLE       = 1ULL << MEMINFO_ID_SRECLAIMABLE,
        MEMINFO_MASK_SUNRECLAIM         = 1ULL << MEMINFO_ID_SUNRECLAIM,
        MEMINFO_MASK_KERNEL_STACK       = 1ULL << MEMINFO_ID_KERNEL_STACK,
        MEMINFO_MASK_PAGE_TABLES        = 1ULL << MEMINFO_ID_PAGE_TABLES,
        MEMINFO_MASK_NFS_UNSTABLE       = 1ULL << MEMINFO_ID_NFS_UNSTABLE,
        MEMINFO_MASK_BOUNCE             = 1ULL << MEMINFO_ID_BOUNCE,
        MEMINFO_MASK_WRITEBACK_TMP      = 1ULL << MEMINFO_ID_WRITEBACK_TMP,
        MEMINFO_MASK_COMMIT_LIMIT       = 1ULL << MEMINFO_ID_COMMIT_LIMIT,
        MEMINFO_MASK_COMMITTED_AS       = 1ULL << MEMINFO_ID_COMMITTED_AS,
        MEMINFO_MASK_VMALLOC_TOTAL      = 1ULL << MEMINFO_ID_VMALLOC_TOTAL,
        MEMINFO_MASK_VMALLOC_USED       = 1ULL << MEMINFO_ID_VMALLOC_USED,
        MEMINFO_MASK_VMALLOC_CHUNK      = 1ULL << MEMINFO_ID_VMALLOC_CHUNK,
        MEMINFO_MASK_ALL                = (1ULL << MEMINFO_ID_MAX) - 1,
};

/**
 * meminfo
 */
struct meminfo {
        unsigned int value[MEMINFO_ID_MAX];
};

/**
 * @brief Convert meminfo id to string
 *
 * @param id meminfo id
 *
 * @return converted string
 */
const char *meminfo_id_to_string(enum meminfo_id id);

/**
 * @brief Convert meminfo string to id
 *
 * @param str meminfo string
 *
 * @return converted id
 */
enum meminfo_id meminfo_string_to_id(const char *str);

/**
 * @brief Get system memory info(/proc/meminfo)
 * @code{.c}
 unsigned int get_mem_available(void)
 {
        struct meminfo mi;

        proc_get_meminfo(&mi, MEMINFO_MASK_MEM_AVAILABLE);
 }
 * @endcode
 *
 * @param mi parsed meminfo struct.
 * @param mask mask to get meminfo.
 *
 * @return 0 on success, -errno on failure.
 */
int proc_get_meminfo(struct meminfo *mi, enum meminfo_mask mask);

/**
 * /proc/buddyinfo page index
 */
enum {
        PAGE_4K = 0,
        PAGE_8K,
        PAGE_16K,
        PAGE_32K,
        PAGE_64K,
        PAGE_128K,
        PAGE_256K,
        PAGE_512K,
        PAGE_1M,
        PAGE_2M,
        PAGE_4M,
        PAGE_MAX,
};

/**
 * A zone buddy info
 */
struct buddyinfo {
        /**
         * Zone name
         */
        char *zone;
        /**
         * Node number
         */
        int node;
        /**
         * Each pages size
         */
        int page[PAGE_MAX];
};

/**
 * @brief free struct buddyinfo
 *
 * @param bi a buddyinfo
 */
void proc_buddyinfo_free(struct buddyinfo *bi);

static inline void buddyinfo_freep(struct buddyinfo **bi)
{
        proc_buddyinfo_free(*bi);
}

/**
 * Declare struct buddyinfo with cleanup attribute. Allocated struct
 * buddyinfo is destroyed on going out the scope.
 */
#define _cleanup_buddyinfo_free_ _cleanup_(buddyinfo_freep)

/**
 * @brief Parse a zone in /proc/buddyinfo
 *
 * @param zone A zone to parse such like "Normal"
 *
 * @param bi Allocated and parsed buddyinfo for given zone. This value
 * has to be destroyed by caller. #_cleanup_buddyinfo_free_ is useful
 * to make autofree this value.
 *
 * @return 0 on success, -errno on failure.
 */
int proc_get_buddyinfo(const char *zone, struct buddyinfo **bi);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

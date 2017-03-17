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

#include <errno.h>
#include <assert.h>
#include <glib.h>

static const int gerror_code_table[] = {
        [G_FILE_ERROR_EXIST]       = EEXIST,
        [G_FILE_ERROR_ISDIR]       = EISDIR,
        [G_FILE_ERROR_ACCES]       = EACCES,
        [G_FILE_ERROR_NAMETOOLONG] = ENAMETOOLONG,
        [G_FILE_ERROR_NOENT]       = ENOENT,
        [G_FILE_ERROR_NOTDIR]      = ENOTDIR,
        [G_FILE_ERROR_NXIO]        = ENXIO,
        [G_FILE_ERROR_NODEV]       = ENODEV,
        [G_FILE_ERROR_ROFS]        = EROFS,
        [G_FILE_ERROR_TXTBSY]      = ETXTBSY,
        [G_FILE_ERROR_FAULT]       = EFAULT,
        [G_FILE_ERROR_LOOP]        = ELOOP,
        [G_FILE_ERROR_NOSPC]       = ENOSPC,
        [G_FILE_ERROR_NOMEM]       = ENOMEM,
        [G_FILE_ERROR_MFILE]       = EMFILE,
        [G_FILE_ERROR_NFILE]       = ENFILE,
        [G_FILE_ERROR_BADF]        = EBADF,
        [G_FILE_ERROR_INVAL]       = EINVAL,
        [G_FILE_ERROR_PIPE]        = EPIPE,
        [G_FILE_ERROR_AGAIN]       = EAGAIN,
        [G_FILE_ERROR_INTR]        = EINTR,
        [G_FILE_ERROR_IO]          = EIO,
        [G_FILE_ERROR_PERM]        = EPERM,
        [G_FILE_ERROR_NOSYS]       = ENOSYS,
};

int gerror_to_errno(GError *error) {
        assert(error);

        return gerror_code_table[error->code];
}

/* qmp_helper.c
 *
 * QMP toolstack to stubdomain helper. This simple helper proxies a QMP
 * traffic between a local UNIX socket and a remote V4V QMP chardrv QEMU in
 * the stubdomain. Initially based off of Chris Patterson's atapi_pt_helper.c.
 *
 * Copyright (c) 2016 Assured Information Security, Ross Philipson <philipsonr@ainfosec.com>
 * Copyright (c) 2015 Assured Information Security, Chris Patterson <pattersonc@ainfosec.com>
 * Copyright (c) 2014 Citrix Systems, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "project.h"

#include <syslog.h>

static int pt_log_level = 0;

/**
 * PT_LOG: information to always log (errors & important low-volume events)
 * @param fmt,... printf style arguments
 */
#define PT_LOG(fmt, ...)                                           \
do {                                                               \
        syslog(LOG_NOTICE, "[%s:%s:%d] (stubdom-%d) " fmt,          \
               __FILE__, __FUNCTION__, __LINE__, g_hs.stubdom_id,  \
                 ##__VA_ARGS__);                                   \
    } while (0)

/**
 * PT_DEBUG: debug level
 * @param fmt,... printf style arguments
 */
#define PT_DEBUG(fmt, ...)                                             \
    do {                                                               \
        if (pt_log_level >= 2)                                       \
            PT_LOG(fmt, ## __VA_ARGS__);                                  \
    } while (0)

#define V4V_TYPE 'W'
#define V4VIOCSETRINGSIZE       _IOW (V4V_TYPE,  1, uint32_t)

#define V4V_QH_PORT 5100
#define V4V_CHARDRV_RING_SIZE \
  (V4V_ROUNDUP((((4096)*4) - sizeof(v4v_ring_t)-V4V_ROUNDUP(1))))

struct qmp_helper_state { 
    int v4v_fd;
    v4v_addr_t remote_addr;
    v4v_addr_t local_addr;
    int unix_fg;
    int stubdom_id;
};

/* global helper state */
static struct qmp_helper_state qhs;

static int pending_exit = 0;

/**
 * Time to bail! Will call exit() with exit_code.
 * @param[in] exit_code
 */
static void exit_cleanup(int exit_code)
{
    int i;

    pending_exit = 1;

    /* close local UNIX socket */
    closesocket(qhs.unix_fd);
    qhs.unix_fd = -1;

    /* close v4v channel to stubdom */
    v4v_close(qhs.v4v_fd);
    qhs.v4v_fd = -1;

    /* close syslog */
    closelog();

    /* time to bail */
    exit(exit_code);
}

/**
 * Initializes helper state.
 * @param[in] qhs
 * @returns 0 on succes, otherwise -1.
 */
static int init_helper_state(struct qmp_helper_state *pqhs)
{
    uint32_t v4v_ring_size = V4V_CHARDRV_RING_SIZE;

    pqhs->v4v_fd = v4v_socket(SOCK_DGRAM);
    if (pqhs->v4v_fd == -1) {
        PT_LOG("unable to create a v4vsocket");
        return -1;
    }

    pqhs->local_addr.port = V4V_QH_PORT;
    pqhs->local_addr.domain = V4V_DOMID_ANY;

    hs->remote_addr.port = V4V_PORT_NONE;
    hs->remote_addr.domain = pqhs->stubdom_id;

    /* TODO check for errors */
    ioctl(pqhs->v4v_fd, V4VIOCSETRINGSIZE, &v4v_ring_size);

    if (v4v_bind(pqhs->v4v_fd, &pqhs->local_addr, pqhs->stubdom_id) == -1) {
        PT_LOG("unable to bind the v4vsocket");
        v4v_close(pqhs->v4v_fd);
        pqhs->v4v_fd = -1;
        return -1;
    }

    /* TODO open the UNIX socket */

    return 0;
}

static void signal_handler(int sig)
{
    PT_LOG("handle signal %d", sig);
    exit_cleanup(0);
}

int main(int argc, char *argv[]) {
{
    openlog(NULL, LOG_NDELAY, LOG_DAEMON);

    PT_LOG("starting %s\n", argv[0]);

    memset(&qhs, 0, sizeof(qhs));

    if (argc != 2) {
        PT_LOG("usage: %s <stubdom_id>", argv[0]);
        return -1;
    }

    qhs.stubdom_id = atoi(argv[1]);

    if (qhs.stubdom_id <= 0) {
        PT_LOG("bad stubdom id (%d)", qhs.stubdom_id);
        return -1;
    }

    signal(SIGINT, signal_handler);

    if (init_helper_state(&qhs) != 0) {
        PT_LOG("failed to init helper!\n");
        return -1;
    }

    while (!pending_exit) {
        /* TODO select loop goes here */
    }

    PT_LOG("exiting...\n");
    exit_cleanup(0);
    return 0;
}

/*
 * numautil.c
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

static char rcsid[] = "$Id:$";

/*
 * $Log:$
 */

#include "project.h"
#include <fcntl.h>
#include <sys/ioctl.h>

static void cpus_to_nodes(void)
{
    xc_interface *xch;
    xc_topologyinfo_t tinfo;
    DECLARE_HYPERCALL_BUFFER(xc_cpu_to_core_t, coremap);
    DECLARE_HYPERCALL_BUFFER(xc_cpu_to_socket_t, socketmap);
    DECLARE_HYPERCALL_BUFFER(xc_cpu_to_node_t, nodemap);
    int i;
    int max_cpus;

    xch = xc_interface_open(NULL, NULL, XC_OPENFLAG_NON_REENTRANT);
    if (!xch) {
        printf("Failed to open XC interface - err: %d\n", errno);
        exit(-1);
    }

    max_cpus = xc_get_max_cpus(xch);
    if (max_cpus == 0) {
        printf("Unable to determine number of CPUS\n");
        goto out0;
    }

    coremap = xc_hypercall_buffer_alloc
        (xch, coremap, sizeof(*coremap) * max_cpus);
    socketmap = xc_hypercall_buffer_alloc
        (xch, socketmap, sizeof(*socketmap) * max_cpus);
    nodemap = xc_hypercall_buffer_alloc
        (xch, nodemap, sizeof(*nodemap) * max_cpus);
    if ((coremap == NULL) || (socketmap == NULL) || (nodemap == NULL)) {
        printf("Unable to allocate hypercall arguments\n");
        goto out1;
    }

    memset(coremap, 0xff, sizeof(*coremap) * max_cpus);
    memset(socketmap, 0xff, sizeof(*socketmap) * max_cpus);
    memset(nodemap, 0xff, sizeof(*nodemap) * max_cpus);

    set_xen_guest_handle(tinfo.cpu_to_core, coremap);
    set_xen_guest_handle(tinfo.cpu_to_socket, socketmap);
    set_xen_guest_handle(tinfo.cpu_to_node, nodemap);
    tinfo.max_cpu_index = max_cpus - 1;
    if (xc_topologyinfo(xch, &tinfo) != 0) {
        printf("Topology info hypercall failed");
        goto out1;
    }

    for (i = 0; i < max_cpus; i++) {
	if (nodemap[i] < 0xffffffff)
	        printf("%d:%d\n", i, nodemap[i]);
    }

out1:
    xc_hypercall_buffer_free(xch, coremap);
    xc_hypercall_buffer_free(xch, socketmap);
    xc_hypercall_buffer_free(xch, nodemap);
out0:
    xc_interface_close(xch);
}

static struct option long_options[] = {
    {"cton", required_argument, 0, 'c'},
    {"help", no_argument, 0, 'h'},
    {0, 0, 0, 0}
};

static void usage(void)
{
    printf("Usage:\n");
    printf("-c, --cton  report the CPU to NODE mapping\n");
    printf("-h, --help  prints this message\n");
}

int main(int argc, char *argv[])
{
    int c;
    int option_index = 0;

    if ( argc <= 1 )
    {
        usage();
        return;
    }

    for ( ; ; )
    {
        c = getopt_long(argc, argv, "ch", long_options, &option_index);
        if ( c == -1 )
            break;
        switch ( c )
        {
        case 'c':
            cpus_to_nodes();
            break;
        case 'h':
            usage();
            break;
        case '?':
            usage();
            break;
        default:
            abort();
        }
    }

    return 0;
}


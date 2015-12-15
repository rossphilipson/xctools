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
        c = getopt_long(argc, argv, "b:Bd::i:l:m:r:wh", long_options, &option_index);
        if ( c == -1 )
            break;
        switch ( c )
        {
        case 'c':
            /* TODO */
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


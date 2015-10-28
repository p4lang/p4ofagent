/*
Copyright 2013-present Barefoot Networks, Inc. 

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "cpu_packet.h"
#include "p4ofagent/p4ofagent.h"

static int cpu_sock_fd;
static int cpu_ifindex;

int cpu_packet_sock_fd_get () {
    return cpu_sock_fd;
}

int cpu_packet_ifindex_get () {
    return cpu_ifindex;
}

void
cpu_packet_swap_header(cpu_header_t *hdr, bool flag) {
    if (flag) {
        hdr->w.w2 = ntohs(hdr->w.w2);
        hdr->w.w4 = ntohs(hdr->w.w4);
        hdr->w.w5 = ntohs(hdr->w.w5);
        hdr->w.w6 = ntohs(hdr->w.w6);
        hdr->w.w7 = ntohs(hdr->w.w7);
    } else {
        hdr->w.w2 = htons(hdr->w.w2);
        hdr->w.w4 = htons(hdr->w.w4);
        hdr->w.w5 = htons(hdr->w.w5);
        hdr->w.w6 = htons(hdr->w.w6);
        hdr->w.w7 = htons(hdr->w.w7);
    }
}

void
cpu_packet_init () {
    char *intf_name = "veth251";

    if ((cpu_sock_fd = socket (AF_PACKET, SOCK_RAW, IPPROTO_RAW)) < 0) {
        P4_LOG ("failed to open raw socket");
        exit (1);
    }

    struct ifreq ifr;
    memset (&ifr, 0, sizeof(ifr));
    strncpy (ifr.ifr_name, intf_name, IFNAMSIZ);
    if (ioctl (cpu_sock_fd, SIOCGIFINDEX, (void *)&ifr) < 0) {
        P4_LOG ("failed to get ifindex of cpu interface");
        exit (1);
    }

    cpu_ifindex = ifr.ifr_ifindex;

    struct sockaddr_ll addr;
    memset (&addr, 0, sizeof (addr));
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = cpu_ifindex;
    addr.sll_protocol = htons (ETH_P_ALL);
    if (bind (cpu_sock_fd, (struct sockaddr *)&addr,
             sizeof (struct sockaddr_ll)) < 0) {
        P4_LOG ("bind to cpu interface failed");
        exit (1);
    }

    int sock_flags = fcntl (cpu_sock_fd, F_GETFL, 0);
    if (fcntl (cpu_sock_fd, F_SETFL, sock_flags | O_NONBLOCK) < 0) {
        P4_LOG ("f_setfl on cpu interface failed");
        exit (1);
    }
}


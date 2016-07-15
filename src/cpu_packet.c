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
#include <p4ofagent/p4ofagent.h>

static int cpu_sock_fd;
static int cpu_ifindex;

int cpu_packet_sock_fd_get () {
    return cpu_sock_fd;
}

int cpu_packet_ifindex_get () {
    return cpu_ifindex;
}

void
cpu_packet_swap_fabric(fabric_header_t *fabric_header, bool network_to_host) {
    if (network_to_host) {
        fabric_header->w.w3 = ntohs(fabric_header->w.w3);
    } else {
        fabric_header->w.w3 = htons(fabric_header->w.w3);
    }
}

void
cpu_packet_swap_multicast(fabric_header_multicast_t *multicast_header, bool network_to_host) {
    if (network_to_host) {
        multicast_header->w.w1 = ntohs(multicast_header->w.w1);
        multicast_header->w.w2 = ntohs(multicast_header->w.w2);
        multicast_header->w.w3 = ntohs(multicast_header->w.w3);
    } else {
        multicast_header->w.w1 = htons(multicast_header->w.w1);
        multicast_header->w.w2 = htons(multicast_header->w.w2);
        multicast_header->w.w3 = htons(multicast_header->w.w3);
    }
}

void
cpu_packet_swap_cpu(fabric_header_cpu_t *cpu_header, bool network_to_host) {
    if (network_to_host) {
        cpu_header->w.w1 = ntohs(cpu_header->w.w1);
        cpu_header->w.w2 = ntohs(cpu_header->w.w2);
        cpu_header->w.w3 = ntohs(cpu_header->w.w3);
        cpu_header->w.w4 = ntohs(cpu_header->w.w4);
    } else {
        cpu_header->w.w1 = htons(cpu_header->w.w1);
        cpu_header->w.w2 = htons(cpu_header->w.w2);
        cpu_header->w.w3 = htons(cpu_header->w.w3);
        cpu_header->w.w4 = htons(cpu_header->w.w4);
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


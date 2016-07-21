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

#ifndef _CPU_PACKET_H
#define _CPU_PACKET_H

#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <linux/if_packet.h>
#include <fcntl.h>
#include <netlink/netlink.h>
#include <netlink/msg.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <unistd.h>

// fabric header
typedef struct __attribute__((__packed__)) fabric_header_ {
    union {
        struct __attribute__((__packed__)) {
            uint8_t packetType : 3;
            uint8_t headerVersion : 2;
            uint8_t packetVersion : 2;
            uint8_t pad1 : 1;

            uint8_t fabricColor : 3;
            uint8_t fabricQos : 5;

            uint8_t dstDevice : 8;
            uint16_t dstPortOrGroup : 16;
        } d;
        struct __attribute__((__packed__)) {
            uint8_t w0;
            uint8_t w1;
            uint8_t w2;
            uint16_t w3;
        } w;
    };
} fabric_header_t;

// multicast header
typedef struct fabric_header_multicast_ {
    union {
        struct __attribute__((__packed__)) {
            uint8_t routed : 1;
            uint8_t outerRouted : 1;
            uint8_t tunnelTerminate : 1;
            uint8_t ingressTunnelType : 5;

            uint16_t ingressIfindex : 16;
            uint16_t ingressBd : 16;

            uint16_t mcastGrp : 16;
        } d;
        struct __attribute__((__packed__)) {
            uint8_t w0;
            uint16_t w1;
            uint16_t w2;
            uint16_t w3;
        } w;
    };
} fabric_header_multicast_t;

// cpu header
typedef struct __attribute__((__packed__)) fabric_header_cpu_ {
    union {
        struct __attribute__((__packed__)) {
            uint8_t  egressQueue : 5;
            uint8_t  txBypass    : 1;
            uint8_t  reserved    : 2;

            uint16_t ingressPort    : 16;
            uint16_t ingressIfindex : 16;
            uint16_t ingressBd      : 16;

            uint16_t reasonCode  : 16;
        } d;
        struct __attribute__((__packed__)) {
            uint8_t w0;
            uint16_t w1;
            uint16_t w2;
            uint16_t w3;
            uint16_t w4;
        } w;
    };
} fabric_header_cpu_t;

//fabric payload header
typedef struct __attribute__((__packed__)) fabric_payload_header_ {
    union {
        struct __attribute__((__packed__)) {
            uint16_t etherType : 16;
        } d;
    };
} fabric_payload_header_t;

// fabric header swap
void
cpu_packet_swap_fabric(fabric_header_t *fabric_header, bool network_to_host);

// multicast header swap
void
cpu_packet_swap_multicast(fabric_header_multicast_t *multicast_header, bool network_to_host);

// cpu header swap
void
cpu_packet_swap_cpu(fabric_header_cpu_t *cpu_header, bool network_to_host);

// Returns fd allocated by cpu_packet_init
int
cpu_packet_sock_fd_get ();

// Returns ifindex allocated by cpu_packet_init
int
cpu_packet_ifindex_get ();

// Allocates sock fd and ifindex for veth251
void
cpu_packet_init ();

#endif

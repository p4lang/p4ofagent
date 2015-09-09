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

typedef struct __attribute__((__packed__)) cpu_header_ {
    union {
        struct __attribute__((__packed__)) {
            /* Fabric header */
            uint8_t packetType : 3;
            uint8_t headerVersion : 2;
            uint8_t packetVersion : 2;
            uint8_t pad1 : 1;

            uint8_t fabricColor : 3;
            uint8_t fabricQos : 5;

            uint8_t dstDevice : 8;
            uint16_t dstPortOrGroup : 16;

            uint16_t ingressIfindex : 16;
            uint16_t ingressBd : 16;

            /* CPU header */
            uint8_t egressQueue : 5;
            uint8_t txBypass : 1;
            uint8_t reserved : 2;
            uint32_t reasonCode : 16;

            /* Fabric payload header */
            uint16_t etherType : 16;
        } d;
        struct __attribute__((__packed__)) {
            uint8_t  w0;
            uint16_t w1;
            uint16_t w2;
            uint16_t w3;
            uint16_t w4;
            uint8_t w5;
            uint32_t w6;
        } w;
    };
} cpu_header_t;

/* Returns fd allocated by cpu_packet_init */
int
cpu_packet_sock_fd_get ();

/* Returns ifindex allocated by cpu_packet_init */
int
cpu_packet_ifindex_get ();

/* Converts cpu_header_t to network byte order */
void
cpu_packet_swap_header(cpu_header_t *hdr, bool flag);

/* Allocates sock fd and ifindex for veth251 */
void
cpu_packet_init ();

#endif

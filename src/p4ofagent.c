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

/*
 * Agent initialization and Packet In logic.
 */ 

#include "cpu_packet.h"
#include "state.h"

#include <pthread.h>

#include <p4ofagent/p4ofagent.h>
#include <p4ofagent/openflow-spec1.3.0.h>

#include <OFConnectionManager/ofconnectionmanager.h>
#include <OFStateManager/ofstatemanager.h>
#include <SocketManager/socketmanager.h>
#include <loci/loci_classes.h>

#ifdef _BMV2_
#include <bm/pdfixed/pd_static.h>
#else
#include <p4_sim/pd_static.h>
#endif

void packet_in_handler (int fd, void *unused, int rr, int wr, int err) {
    static unsigned char in_buf[2000];
    static unsigned char out_buf[2000];

    int ret;
    of_packet_in_t *packet_in;

    // read packet into buffer    
    memset (in_buf, 0, sizeof (in_buf));
    if ((ret = read (fd, (void *) in_buf, sizeof (in_buf))) < 0) {
        P4_LOG ("Error reading packet in");
    }

    // extract cpu header
    cpu_header_t cpu;
    memset (&cpu, 0, sizeof (cpu));
    memcpy (&cpu, in_buf + 14, sizeof (cpu));
    cpu_packet_swap_header (&cpu, TRUE);

    // initialize packet in obj
    packet_in = of_packet_in_new (OF_VERSION_1_3); 

    //struct ofp_packet_in fields
    of_octets_t octets = { .data = out_buf, .bytes = ret };

    of_packet_in_buffer_id_set (packet_in, -1);
    of_packet_in_total_len_set (packet_in, ret - sizeof(cpu));
    of_packet_in_reason_set (packet_in, cpu.d.reasonCode >> 8); 
    of_packet_in_table_id_set (packet_in, (uint8_t) cpu.d.reasonCode); 
    of_packet_in_cookie_set (packet_in, 0);

    // struct ofp_match fields
    of_match_t match;
    memset (&match, 0, sizeof (match));
    match.version = OF_VERSION_1_3;
    match.fields.in_port = cpu.d.ingressPort;
    match.masks.in_port = 0xffffffff;
    if (of_packet_in_match_set (packet_in, &match)) {
        P4_LOG ("Error setting match");
    }

    // copy packet to output buffer, skipping cpu header
    memset (out_buf, 0, sizeof (out_buf));
    memcpy (out_buf, in_buf, 12);
    memcpy (out_buf + 12, in_buf + 14 + sizeof (cpu) - 2,
            ret - (14 + sizeof (cpu) - 2));

    // send to controller
    if (!of_packet_in_data_set (packet_in, &octets)) {
        indigo_core_packet_in (packet_in); 
    } else {
        P4_LOG ("Failed to copy data to packet in");
    }
}

void *run_indigo (void *unused) {

    ind_soc_select_and_run (-1);

    /* Tear down modules */
    ind_soc_finish ();
    ind_core_finish ();
    ind_cxn_finish ();

    return NULL;
}

void p4ofagent_init (bool ipv6, char *ctl_ip) {

    ind_soc_config_t socket_config;

    ind_core_config_t core_config = {
        .stats_check_ms = 30
    };

    ind_cxn_config_t cxn_config;

    pthread_t agent;

    // Initialize modules
    if (INDIGO_FAILURE (ind_soc_init (&socket_config))
        || INDIGO_FAILURE (ind_core_init (&core_config))
        || INDIGO_FAILURE (ind_cxn_init (&cxn_config))) {
        P4_LOG ("Failure initializing modules.\n");
        exit(1);
    }

    // Enable modules
    if (INDIGO_FAILURE (ind_soc_enable_set (1))
        || INDIGO_FAILURE (ind_core_enable_set (1))
        || INDIGO_FAILURE (ind_cxn_enable_set (1))) {
        P4_LOG ("Failure enabling modules.\n");
        exit(1);
    }

    if (ctl_ip != NULL) {
        indigo_controller_id_t c_id;
        indigo_cxn_config_params_t conf_p = {
            .periodic_echo_ms = 3000,
            .reset_echo_count = 30,
            .version = OF_VERSION_1_3,
            .listen = 0
        };

        indigo_cxn_protocol_params_t proto_p;

        if (!ipv6) { 
            // Connect to IPV4 controller
            proto_p.header.protocol = INDIGO_CXN_PROTO_TCP_OVER_IPV4;
            proto_p.tcp_over_ipv4.protocol = INDIGO_CXN_PROTO_TCP_OVER_IPV4;
            proto_p.tcp_over_ipv4.controller_port = 6633;

            sprintf (proto_p.tcp_over_ipv4.controller_ip, "%s", ctl_ip);
        } else { 
            // Connect to IPV6 controller
            proto_p.header.protocol = INDIGO_CXN_PROTO_TCP_OVER_IPV6;
            proto_p.tcp_over_ipv6.protocol = INDIGO_CXN_PROTO_TCP_OVER_IPV6;
            proto_p.tcp_over_ipv6.controller_port = 6633;

            sprintf (proto_p.tcp_over_ipv6.controller_ip, "%s", ctl_ip);
        }

        if (INDIGO_FAILURE (indigo_controller_add (&proto_p, &conf_p, &c_id))) {
            P4_LOG ("Failed to add controller.\n");
            exit(1);
        }
    }

    // Listen -- for testing
    indigo_controller_id_t listen_c_id;

    indigo_cxn_config_params_t listen_conf_p = {
        .version = OF_VERSION_1_3,
        .listen = 1
    };

    indigo_cxn_protocol_params_t listen_proto_p = {
        .header.protocol = INDIGO_CXN_PROTO_TCP_OVER_IPV4,
        .tcp_over_ipv4.protocol = INDIGO_CXN_PROTO_TCP_OVER_IPV4,
        .tcp_over_ipv4.controller_port = 6653
    };

    sprintf (listen_proto_p.tcp_over_ipv4.controller_ip, "0.0.0.0");

    if (INDIGO_FAILURE (indigo_controller_add (&listen_proto_p,
                                               &listen_conf_p,
                                               &listen_c_id))) {
        P4_LOG ("Failed to add controller.\n");
        exit(1);
    }

#ifdef _BMV2_
    if (p4_pd_mc_create_session(&P4_PRE_SESSION)) {
        P4_LOG ("Could not start PRE session for openflow\n");
    }
#else
    if (mc_create_session(&P4_PRE_SESSION)) {
        P4_LOG ("Could not start PRE session for openflow\n");
    }
#endif // _BMV2_

#ifdef _BMV2_
    if  (p4_pd_client_init (&P4_PD_SESSION)) {
#else
    if  (p4_pd_client_init (&P4_PD_SESSION, 5)) {
#endif
        P4_LOG ("Could not start PD session for openflow\n");
    }

    // initialize pd lib mappings and openflow module
    state_init ();

    // Get fd and ifindex for packet out
    cpu_packet_init ();

    // Register packet in handler
    ind_soc_socket_register (cpu_packet_sock_fd_get (),
                             &packet_in_handler, NULL);

    // Main event loop
    pthread_create(&agent, NULL, &run_indigo, NULL);
}

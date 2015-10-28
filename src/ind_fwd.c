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
 * Implementation of some OF logic, primarily Packet Out.
 * This file is required by IVS.
 */

#include "cpu_packet.h"
#include "p4ofagent/parse.h"
#include "p4ofagent/p4ofagent.h"
#include "p4ofagent/openflow-spec1.3.0.h"
#include "p4_sim/pd_wrappers.h"
#include "indigo/forwarding.h" 

indigo_error_t
indigo_fwd_forwarding_features_get (of_features_reply_t *fr) {
    of_features_reply_n_buffers_set (fr, 0);
    of_features_reply_n_tables_set (fr, num_openflow_tables ());
    of_features_reply_capabilities_set (fr, 3);
    return INDIGO_ERROR_NONE;
}

#define UNICAST 1
#define MULTICAST 2

indigo_error_t
indigo_fwd_packet_out (of_packet_out_t *packet_out) {
    of_port_no_t in_port;
    of_octets_t data;

    of_packet_out_data_get(packet_out, &data);

    uint8_t *arg;
    uint8_t hdr_field;
    uint16_t type;

    of_list_action_t *actions;
    of_action_t elt;
    int rv, num_actions = 0;

    // get packet original in port
    of_packet_out_in_port_get(packet_out, &in_port);

    // cpu header setup
    cpu_header_t cpu;
    memset (&cpu, 0, sizeof (cpu));
    cpu.w.w0 = 0xa0;
    cpu.d.etherType = *(uint16_t *) (data.data + 12);

    actions = of_packet_out_actions_get(packet_out);

    OF_LIST_ACTION_ITER (actions, &elt, rv) {
        parse_ofpat (&elt, &type, &arg, &hdr_field);

        if (type == OFPAT_OUTPUT) {
            switch (*(uint32_t *) arg) {
                case OFPP_FLOOD:
                case OFPP_ALL:
                    cpu.d.reasonCode = MULTICAST;
                    cpu.d.dstPortOrGroup = AGENT_ETHERNET_FLOOD_MC_HDL;
                    break;
                case OFPP_IN_PORT:
                    cpu.d.reasonCode = UNICAST;
                    cpu.d.dstPortOrGroup = in_port;
                    break;
                case OFPP_TABLE:
                    cpu.d.reasonCode = UNICAST;
                    cpu.d.dstPortOrGroup = 0;
                    break;
                default:
                    cpu.d.reasonCode = UNICAST;
                    cpu.d.dstPortOrGroup = *(uint32_t *) arg;
            }
        } else {
            P4_LOG ("Unsupported: PACKET OUT can only do output");
            return INDIGO_ERROR_BAD_ACTION;
        }
        
        free(arg);

        if (num_actions) {
            P4_LOG ("Unsupported: PACKET OUT can only do one action");
        } else {
            ++num_actions;
        }
    }

    cpu_packet_swap_header (&cpu, TRUE);

    // Fill out output buffer
    static char out_buf[2000];
    memset (out_buf, 0, sizeof (out_buf));

    memcpy (out_buf, data.data, 12);

    *(out_buf + 12) = 0x90;

    memcpy (out_buf + 14, (void *) &cpu, sizeof (cpu));
    memcpy (out_buf + sizeof(cpu) + 14, data.data + 14, data.bytes - 14);

    // Send output buffer to interface
    if (send (cpu_packet_sock_fd_get (), out_buf,
              data.bytes + sizeof (cpu), 0) < 0) {
        P4_LOG ("packet out send failed");
    }

    return INDIGO_ERROR_NONE;
}

indigo_error_t
indigo_fwd_experimenter (of_experimenter_t *ofe, indigo_cxn_id_t id) {
    return INDIGO_ERROR_NOT_SUPPORTED;
}

void
indigo_fwd_pipeline_get (of_desc_str_t pipeline) {
}

indigo_error_t
indigo_fwd_pipeline_set (of_desc_str_t pipeline) {
    return INDIGO_ERROR_NOT_SUPPORTED;
}

void
indigo_fwd_pipeline_stats_get (of_desc_str_t **piplines, int *num_piplines) {
}

indigo_error_t
indigo_fwd_packet_receive (of_port_no_t ingress, uint8_t *data, unsigned len) {
    return INDIGO_ERROR_NOT_SUPPORTED;
}

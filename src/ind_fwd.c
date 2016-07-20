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
#include <p4ofagent/parse.h>
#include <p4ofagent/p4ofagent.h>
#include <p4ofagent/openflow-spec1.3.0.h>
#include <indigo/forwarding.h>

indigo_error_t
indigo_fwd_forwarding_features_get (of_features_reply_t *fr) {
    of_features_reply_n_buffers_set (fr, 0);
    of_features_reply_n_tables_set (fr, num_openflow_tables ());
    of_features_reply_capabilities_set (fr, 3);
    return INDIGO_ERROR_NONE;
}

// PACKET_OUT only supports 1 action right now, OUTPUT

indigo_error_t
indigo_fwd_packet_out (of_packet_out_t *packet_out) {
    of_port_no_t in_port;
    of_octets_t data;

    of_packet_out_data_get(packet_out, &data);
    of_packet_out_in_port_get (packet_out, &in_port);

    uint8_t *arg;
    uint8_t hdr_field;
    uint16_t type;

    of_list_action_t *actions;
    of_action_t elt;
    int rv;

    // fabric header setup
    fabric_header_t fabric_header;
    memset (&fabric_header, 0, sizeof (fabric_header));

    // only used for OFPP_ALL/OFPP_FLOOD
    fabric_header_multicast_t multicast_header;
    memset (&multicast_header, 0, sizeof(multicast_header));

    // only used for OFPP_IN_PORT/unicasting
    fabric_header_cpu_t cpu_header;

    // just an ethertype at the moment
    fabric_payload_header_t payload_header;
    memset (&payload_header, 0, sizeof(payload_header));
    payload_header.d.etherType = *(uint16_t *) (data.data + 12);

    // Init output buffer, copy mac header and fabric ethertype
    int cursor = 12;
    static char out_buf[2000];
    memset (out_buf, 0, sizeof (out_buf));
    memcpy (out_buf, data.data, cursor);
    *(out_buf + cursor) = 0x90;
    cursor += 2;

    actions = of_packet_out_actions_get(packet_out);

    OF_LIST_ACTION_ITER (actions, &elt, rv) {
        parse_ofpat (&elt, &type, &arg, &hdr_field);
        if (type == OFPAT_OUTPUT) {
            switch (*(uint32_t *) arg) {
                case OFPP_FLOOD:
                case OFPP_ALL:
                    fabric_header.w.w0 = 0x40;
                    fabric_header.d.dstDevice = 127;
                    multicast_header.w.w0 = 0x20;
                    multicast_header.d.ingressIfindex = in_port;
                    multicast_header.d.mcastGrp = AGENT_ETHERNET_FLOOD_MC_HDL;
                    cpu_packet_swap_fabric (&fabric_header, FALSE);
                    cpu_packet_swap_multicast (&multicast_header, FALSE);
                    memcpy (out_buf + cursor, &fabric_header, sizeof (fabric_header));
                    cursor += sizeof (fabric_header);
                    memcpy (out_buf + cursor, &multicast_header, sizeof (multicast_header));
                    cursor += sizeof (multicast_header);
                    break;
                default:
                    fabric_header.w.w0 = 0xa0;
                    fabric_header.d.dstPortOrGroup = *(uint32_t *) arg;
                    cpu_header.d.ingressPort = in_port;
                    cpu_header.d.ingressIfindex = cpu_header.d.ingressPort + 1;
                    cpu_packet_swap_fabric (&fabric_header, FALSE);
                    cpu_packet_swap_cpu (&cpu_header, FALSE);
                    memcpy (out_buf + cursor, &fabric_header, sizeof (fabric_header));
                    cursor += sizeof (fabric_header);
                    memcpy (out_buf + cursor, &cpu_header, sizeof (cpu_header));
                    cursor += sizeof (cpu_header);
                    break;
            }
        } else {
            P4_LOG ("Unsupported: PACKET OUT can only do output");
            return INDIGO_ERROR_BAD_ACTION;
        }
        free(arg);
        break;
    }

    memcpy (out_buf + cursor, &payload_header, sizeof (payload_header));
    cursor += sizeof (payload_header);

    memcpy (out_buf + cursor, data.data + 14, data.bytes - 14);

    // Send output buffer to interface
    if (send (cpu_packet_sock_fd_get (), out_buf, data.bytes + cursor - 14, 0) < 0) {
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

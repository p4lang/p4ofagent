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
 * Openflow action parsing/storage in other data structures.
 */

#include "state.h"

#include <p4ofagent/callbacks.h>
#include <p4ofagent/parse.h>
#include <p4ofagent/p4ofagent.h>
#include <p4ofagent/openflow-spec1.3.0.h>

#ifdef _BMV2_
#include <plugin/of/inc/pd_wrappers.h>
#else
#include <p4_sim/pd_wrappers.h>
#endif // _BMV2_

void
parse_ofpat (of_action_t *action, uint16_t *type,
             uint8_t **arg, uint8_t *hdr_field) {
    of_oxm_t *oxm_field;
    of_wire_buffer_u16_get (action->wbuf, action->obj_offset, type);

    switch (*type) {
        case OFPAT_OUTPUT:
            *arg = P4OFAGENT_MALLOC (4);
            of_action_output_port_get (action, (uint32_t *) *arg);
            break;
        case OFPAT_GROUP:
            *arg = P4OFAGENT_MALLOC (4);
            of_action_group_group_id_get (action, (uint32_t *) *arg);
            break;
        case OFPAT_SET_QUEUE:
            *arg = P4OFAGENT_MALLOC (4);
            of_action_set_queue_queue_id_get(action, (uint32_t *) *arg);
            break;
        case OFPAT_SET_MPLS_TTL:
            *arg = P4OFAGENT_MALLOC (1);
            of_action_set_mpls_ttl_mpls_ttl_get (action, *arg);
            break;
        case OFPAT_SET_NW_TTL:
            *arg = P4OFAGENT_MALLOC (1);
            of_action_set_nw_ttl_nw_ttl_get(action, *arg);
            break;
        case OFPAT_SET_FIELD:
            oxm_field = of_action_set_field_field_get (action);
            of_wire_buffer_u8_get (oxm_field->wbuf, oxm_field->obj_offset + 2, 
                                   hdr_field);
            *hdr_field >>= 1;
            if (*hdr_field != OFPXMT_OFB_VLAN_VID) {
                perror ("Unsupported match field for OFPAT_SET_FIELD");
            } else {
                *arg = P4OFAGENT_MALLOC (2);
                of_oxm_vlan_vid_value_get (oxm_field, (uint16_t *) *arg);
            }
            break;
        case OFPAT_PUSH_VLAN:
        case OFPAT_PUSH_MPLS:
        case OFPAT_PUSH_PBB:
        case OFPAT_POP_MPLS:
            break;
    }
}

void
parse_actions (of_list_action_t *actions, Pvoid_t *aargs, uint32_t *sig) {
    uint8_t *arg;
    uint8_t hdr_field;
    uint16_t type;

    int rv, rc;
    of_action_t elt;

    PWord_t pv;
    PWord_t pv1;

    OF_LIST_ACTION_ITER(actions, &elt, rv) {
        parse_ofpat (&elt, &type, &arg, &hdr_field);

        // OFPAT_SET_FIELD indexes another jarray in aargs
        // that goes like match_field -> value
        if (type == OFPAT_SET_FIELD) {
            Pvoid_t pipeline;
            if ((J1T (rc, *aargs, OFPAT_SET_FIELD))) {
                JLG (pv, *aargs, OFPAT_SET_FIELD);
                pipeline = (Pvoid_t) *pv;
                JLI (pv, pipeline, hdr_field);
            } else {
                pipeline = (Pvoid_t) NULL;
                JLI (pv1, *aargs, OFPAT_SET_FIELD);
                JLI (pv, pipeline, hdr_field);
                *pv1 = (Word_t) pipeline;
            }
        } else {
            JLI (pv, *aargs, type);
        }

        signature_set_bit (type, sig);
        *pv = (uint64_t) arg;
    }
}

void
parse_instructions (of_list_instruction_t *list, Pvoid_t *aargs, uint32_t *sig) {
    uint16_t instr;
    uint8_t *arg;
    int rv;

    PWord_t pv;

    of_list_action_t *actions;
    of_instruction_t *elt = of_object_new (OF_VERSION_1_3);

    OF_LIST_INSTRUCTION_ITER (list, elt, rv) {
        of_wire_buffer_u16_get (elt->wbuf, 0, &instr);

        switch (instr) {
            case OFPIT_GOTO_TABLE:
                arg = P4OFAGENT_MALLOC (1);
                of_instruction_goto_table_table_id_get (elt, arg);
                break;
            case OFPIT_WRITE_METADATA:
                arg = P4OFAGENT_MALLOC (8);
                of_instruction_write_metadata_metadata_get (elt, (uint64_t *) arg);
                break;
            case OFPIT_WRITE_ACTIONS:
            case OFPIT_APPLY_ACTIONS:
            case OFPIT_CLEAR_ACTIONS:
                actions = of_instruction_apply_actions_actions_get (elt);
                parse_actions (actions, aargs, sig);
                return;
            case OFPIT_METER:
                arg = P4OFAGENT_MALLOC (4);
                of_instruction_meter_meter_id_get (elt, (uint32_t *) arg);
                break;
            default:
                break;
        }

        JLI (pv, *aargs, instr);
        *pv = (uint64_t) arg;

        // TODO: instruction signature handling
        signature_set_bit (instr, sig);
    }
}

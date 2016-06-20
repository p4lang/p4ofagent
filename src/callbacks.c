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
 * Table callbacks
 */

#include "state.h"

#include <stdlib.h>

#include <p4ofagent/p4ofagent.h>
#include <p4ofagent/parse.h>
#include <p4ofagent/callbacks.h>
#include <p4ofagent/openflow-spec1.3.0.h>

#ifdef _BMV2_
#include <p4ofagent/plugin.h>
#else
#include <p4_sim/pd.h>
#include <p4_sim/openflow.h>
#endif // _BMV2_

/**************
 * Signatures *
 **************/

#define INVALID NULL

void
signature_set_bit (int bit, uint32_t *sig) {
    *sig |= ((uint64_t) 1) << bit;
}

int
signature_check_bit (int bit, uint32_t *sig) {
    return *sig & (((uint64_t) 1) << bit);
}

/************************
 * Normal flow table mods
 ************************/

indigo_error_t
flow_create (void *table_priv, indigo_cxn_id_t cxn_id,
             of_flow_add_t *flow_add, indigo_cookie_t flow_id,
             void **entry_priv) {
    uint8_t  tid;
    uint16_t pr;
    uint16_t mlen;
    uint64_t cookie;
    uint64_t cmask;
    uint32_t ttl = 3;

    uint32_t action_sig = 0;
    uint8_t  no_pipeline = 0;

    Pvoid_t ac; 

    p4_pd_entry_hdl_t eh;

    of_list_instruction_t *instructions;

    of_flow_add_table_id_get (flow_add, &tid);
    of_flow_add_priority_get (flow_add, &pr);
    of_flow_add_cookie_get (flow_add, &cookie);
    of_flow_add_cookie_mask_get (flow_add, &cmask);
    mlen = of_match_bytes (flow_add->wbuf, MH_OFFSET);
    
    if (cmask) { flow_id = cookie | cmask; }
    *entry_priv = (void *) flow_id;

    ac = (Pvoid_t) NULL;
    instructions = of_flow_add_instructions_get (flow_add);
    parse_instructions (instructions, &ac, &action_sig);

    if (mlen - sizeof (struct ofp_match)) {
        of_match_t mf;
        indigo_error_t status;
        flow_add_wrapper_t add_f;

        if ((status = of_flow_add_match_get (flow_add, &mf))) {
            return status;
        }

        if (table_add_get (tid, &add_f)) {
            P4_LOG ("FLOW_CREATE: Bad Table Id");
            return INDIGO_ERROR_BAD_TABLE_ID;
        }

        if (add_f (&mf, &ac, action_sig, flow_id, &eh, &pr,
                   &ttl, P4_PD_SESSION, P4_SINGLE_DEVICE, &no_pipeline)) {
            P4_LOG ("FLOW_CREATE: Bad PD status adding flow");
            return INDIGO_ERROR_UNKNOWN;
        }
    } else {
        flow_def_wrapper_t def_f;

        if (table_def_get (tid, &def_f)) {
            P4_LOG ("FLOW_CREATE: Bad Table Id");
            return INDIGO_ERROR_BAD_TABLE_ID;
        }

        if (def_f (&ac, action_sig, flow_id, &eh,
                   P4_PD_SESSION, P4_SINGLE_DEVICE, &no_pipeline)) {
            P4_LOG ("FLOW_CREATE: Bad PD status adding flow");
            return INDIGO_ERROR_UNKNOWN;
        }
    }

    if (!no_pipeline) {
        ofpat_pipeline_key_t key;
        ofpat_pipeline_key_new (&flow_id, INVALID, INVALID, &key);
        if (ofpat_pipeline_add (action_sig, &key, &ac)) {
            P4_LOG ("FLOW_CREATE: Bad PD status adding pipeline");
            return INDIGO_ERROR_UNKNOWN;
        }
    }

    flow_id_entry_hdl_set (flow_id, eh);
    flow_id_table_id_set (flow_id, tid);

    return INDIGO_ERROR_NONE;
}

indigo_error_t
flow_modify (void *table_priv, indigo_cxn_id_t cxn_id,
             void *entry_priv, of_flow_modify_strict_t *flow_modify) {
    uint8_t  tid;
    uint64_t flow_id;

    uint32_t action_sig = 0;
    uint8_t  no_pipeline = 0;
    
    Pvoid_t ac;

    p4_pd_entry_hdl_t   eh;
    flow_mod_wrapper_t  mod_f;

    of_list_instruction_t *instructions;

    ac = (Pvoid_t) NULL;
    instructions = of_flow_modify_instructions_get (flow_modify);
    parse_instructions (instructions, &ac, &action_sig);

    of_flow_modify_table_id_get (flow_modify, &tid);

    flow_id = (uint64_t) entry_priv;

    if (flow_id_entry_hdl_get (flow_id, &eh)
        || flow_id_table_id_get (flow_id, &tid)) {
        P4_LOG ("FLOW_MODIFY: Bad Flow ID");
        return INDIGO_ERROR_NOT_FOUND;
    }

    if (table_mod_get (tid, &mod_f)) {
        P4_LOG ("FLOW_MODIFY: Bad Table ID");
        return INDIGO_ERROR_BAD_TABLE_ID;
    }
    
    if (mod_f (&ac, action_sig, flow_id,
               eh, P4_PD_SESSION, P4_DEVICE_ID, &no_pipeline)) {
        P4_LOG ("FLOW_MODIFY: Bad PD status modifying flow");
        return INDIGO_ERROR_UNKNOWN;
    }

    if (!no_pipeline) {
        ofpat_pipeline_key_t key;
        ofpat_pipeline_key_new (&flow_id, INVALID, INVALID, &key);
        if (ofpat_pipeline_mod (action_sig, &key, &ac)) {
            P4_LOG ("FLOW_MODIFY: Bad PD status modifying pipeline");
            return INDIGO_ERROR_UNKNOWN;
        }
    }

    return INDIGO_ERROR_NONE;
}

indigo_error_t
flow_delete (void *table_priv, indigo_cxn_id_t cxn_id, void *entry_priv,
             indigo_fi_flow_stats_t *fs) {
    uint8_t  tid;
    uint64_t flow_id;

    flow_del_t        del_f;
    p4_pd_entry_hdl_t eh;

    flow_id = (uint64_t) entry_priv;
    
    if (flow_id_entry_hdl_get (flow_id, &eh)
        || flow_id_table_id_get (flow_id, &tid)) {
        P4_LOG ("FLOW_MODIFY: Bad Flow ID");
        return INDIGO_ERROR_NOT_FOUND;
    }

    if (table_del_get (tid, &del_f)) {
        P4_LOG ("FLOW_MODIFY: Bad Table ID");
        return INDIGO_ERROR_BAD_TABLE_ID;
    }

    if (del_f (P4_PD_SESSION, P4_DEVICE_ID, eh)) {
        P4_LOG ("FLOW_DELETE: Bad PD Status");
        return INDIGO_ERROR_UNKNOWN;
    }

    ofpat_pipeline_key_t key;  
    ofpat_pipeline_key_new (&flow_id, INVALID, INVALID, &key);
    if (ofpat_pipeline_del (&key)) {
        P4_LOG ("FLOW_DELETE: Error deleting key");
        return INDIGO_ERROR_UNKNOWN;
    }

    return INDIGO_ERROR_NONE;
}

/************
 * Counters *
 ************/

indigo_error_t
table_stats_get (void *table_priv, indigo_cxn_id_t cxn_id,
                 indigo_fi_table_stats_t *table_stats) {
    uint64_t hit_count;
    uint64_t miss_count;
    uint64_t tid = (uint64_t) table_priv;

    table_stats_get_t get_missed_f;
    table_stats_get_t get_hit_f;

    if (table_packets_hit_get (tid, &get_hit_f)
        || table_packets_missed_get (tid, &get_missed_f)) {
        P4_LOG ("TABLE_STATS_GET: Bad Table ID");
        return INDIGO_ERROR_BAD_TABLE_ID;
    }

    hit_count = get_hit_f (P4_PD_SESSION, P4_SINGLE_DEVICE);
    miss_count = get_missed_f (P4_PD_SESSION, P4_SINGLE_DEVICE);

    table_stats->matched_count = hit_count;
    table_stats->lookup_count = hit_count + miss_count;

    return INDIGO_ERROR_NONE;
}

indigo_error_t
flow_stats_get (void *table_priv, indigo_cxn_id_t cxn_id,
                void *entry_priv, indigo_fi_flow_stats_t *flow_stats) {
    uint64_t tid = (uint64_t) table_priv;
    uint64_t fid = (uint64_t) entry_priv;
 
    flow_stats_get_t bytes_counter_f;
    flow_stats_get_t packets_counter_f;

    if (flow_packets_get (tid, &packets_counter_f)) {
        P4_LOG ("FLOW_STATS: Bad Table ID");
        return INDIGO_ERROR_BAD_TABLE_ID;
    } else {
        flow_stats->packets =
            packets_counter_f (P4_PD_SESSION, P4_SINGLE_DEVICE, fid);
    }

    if (flow_bytes_get (tid, &bytes_counter_f)) {
        P4_LOG ("FLOW_STATS: Bad Table ID");
        return INDIGO_ERROR_BAD_TABLE_ID;
    } else {
        flow_stats->bytes =
            bytes_counter_f (P4_PD_SESSION, P4_SINGLE_DEVICE, fid);
    }

    return INDIGO_ERROR_NONE;
}

/**********
 * Groups *
 **********/

indigo_error_t
group_create (void *table_priv, indigo_cxn_id_t cxn_id, uint32_t group_id,
              uint8_t group_type, of_list_bucket_t *buckets, void **entry_priv) {
    if (group_type == OFPGT_ALL || group_type == OFPGT_INDIRECT) {
        ofpat_group_alloc (group_id, buckets, group_type);
        ofpat_group_create (group_id, group_type);
    } else {
        P4_LOG ("Controller trying to add unsupported group type.");
        return INDIGO_ERROR_NOT_SUPPORTED;
    }

    *entry_priv = (void *) ((group_type << 31) + (uint64_t) (group_id));

    return INDIGO_ERROR_NONE;
}

indigo_error_t
group_modify (void *table_priv, indigo_cxn_id_t cxn_id, void *entry_priv,
              of_list_bucket_t *buckets) {
    uint32_t group_id = (uint32_t) (uint64_t) entry_priv;

    enum ofp_group_type group_type =
        (enum ofp_group_type) ((uint64_t) entry_priv >> 31);

    if (ofpat_group_delete (group_id, group_type)) {
        return INDIGO_ERROR_UNKNOWN;
    }

    ofpat_group_alloc (group_id, buckets, group_type);
    ofpat_group_create (group_id, group_type);

    return INDIGO_ERROR_NONE;
}

indigo_error_t 
group_delete (void *table_priv, indigo_cxn_id_t cxn_id, void *entry_priv) {
    if (ofpat_group_delete ((uint32_t) (uint64_t) entry_priv,
                            (uint8_t) ((uint64_t) entry_priv >> 31))) {
        return INDIGO_ERROR_UNKNOWN;
    }

    return INDIGO_ERROR_NONE;
}

indigo_error_t 
group_stats (void *table_priv, void *entry_priv, of_group_stats_entry_t *stats) {
    P4_LOG ("Unsupported: group_stats_get\n");
    return INDIGO_ERROR_NONE;
}

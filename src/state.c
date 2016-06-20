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
 * State agent must keep track of
 */

#include "state.h"

static Pvoid_t flow_id_to_entry_hdl = (Pvoid_t) NULL;
static Pvoid_t flow_id_to_table_id  = (Pvoid_t) NULL;

static Pvoid_t table_add_fn_pointers     = (Pvoid_t) NULL;
static Pvoid_t table_mod_fn_pointers     = (Pvoid_t) NULL;
static Pvoid_t table_set_def_fn_pointers = (Pvoid_t) NULL;

static Pvoid_t table_delete_fn_pointers = (Pvoid_t) NULL;

static Pvoid_t bytes_hit_fn_pointers    = (Pvoid_t) NULL;
static Pvoid_t bytes_missed_fn_pointers = (Pvoid_t) NULL;
static Pvoid_t pkts_hit_fn_pointers     = (Pvoid_t) NULL;
static Pvoid_t pkts_missed_fn_pointers  = (Pvoid_t) NULL;
static Pvoid_t per_flow_stats_bytes     = (Pvoid_t) NULL;
static Pvoid_t per_flow_stats_packets   = (Pvoid_t) NULL;

static PWord_t pv;
static int rc;

int
flow_id_entry_hdl_get (uint64_t flow_id, p4_pd_entry_hdl_t *hdl) {
    if (!(J1T (rc, flow_id_to_entry_hdl, flow_id))) {
        return 1;
    }

    JLG (pv, flow_id_to_entry_hdl, flow_id);
    *hdl = (p4_pd_entry_hdl_t) *pv;
    return 0;
}

void
flow_id_entry_hdl_set (uint64_t flow_id, p4_pd_entry_hdl_t hdl) {
    JLI (pv, flow_id_to_entry_hdl, flow_id);
    *pv = (uint64_t) hdl;
}

int
flow_id_table_id_get (uint64_t flow_id, uint8_t *table_id) {
    if (!(J1T (rc, flow_id_to_table_id, flow_id))) {
        return 1;
    }

    JLG (pv, flow_id_to_table_id, flow_id);
    *table_id = (uint8_t) *pv;
    return 0;
}

void
flow_id_table_id_set (uint64_t flow_id, uint8_t table_id) {
    JLI (pv, flow_id_to_table_id, flow_id);
    *pv = table_id;
}

int
table_add_get (uint8_t table_id, flow_add_wrapper_t *add_f) {
    if (!(J1T (rc, table_add_fn_pointers, table_id))) {
        return 1;
    }

    JLG (pv, table_add_fn_pointers, table_id);
    *add_f = (flow_add_wrapper_t) *pv;
    return 0;
}

int
table_mod_get (uint8_t table_id, flow_mod_wrapper_t *mod_f) {
    if (!(J1T (rc, table_mod_fn_pointers, table_id))) {
        return 1;
    }

    JLG (pv, table_mod_fn_pointers, table_id);
    *mod_f = (flow_mod_wrapper_t) *pv;
    return 0;
}

int
table_def_get (uint8_t table_id, flow_def_wrapper_t *def_f) {
    if (!(J1T (rc, table_set_def_fn_pointers, table_id))) {
        return 1;
    }

    JLG (pv, table_set_def_fn_pointers, table_id);
    *def_f = (flow_def_wrapper_t) *pv;
    return 0; 
}

int
table_del_get (uint8_t table_id, flow_del_t *del_f) {
    if (!(J1T (rc, table_delete_fn_pointers, table_id))) {
        return 1;
    }

    JLG (pv, table_delete_fn_pointers, table_id);
    *del_f = (flow_del_t) *pv;
    return 0;
}

int
table_bytes_hit_get (uint8_t table_id, table_stats_get_t *hit_bytes) {
    if (!(J1T (rc, bytes_hit_fn_pointers, table_id))) {
        return 1;
    }
    
    JLG (pv, bytes_hit_fn_pointers, table_id);
    *hit_bytes = (table_stats_get_t) *pv;
    return 0;
}

int
table_bytes_missed_get (uint8_t table_id, table_stats_get_t *missed_bytes) {
    if (!(J1T (rc, bytes_missed_fn_pointers, table_id))) {
        return 1;
    }

    JLG (pv, bytes_missed_fn_pointers, table_id);
    *missed_bytes = (table_stats_get_t) *pv;
    return 0;
}

int
table_packets_hit_get (uint8_t table_id, table_stats_get_t *hit_pkts) {
    if (!(J1T (pv, pkts_hit_fn_pointers, table_id))) {
        return 1;
    }

    JLG (pv, pkts_hit_fn_pointers, table_id);
    *hit_pkts = (table_stats_get_t) *pv;
    return 0;
}

int
table_packets_missed_get (uint8_t table_id, table_stats_get_t *missed_pkts) {
    if (!(J1T (rc, pkts_missed_fn_pointers, table_id))) {
        return 1;
    }

    JLG (pv, pkts_missed_fn_pointers, table_id);
    *missed_pkts = (table_stats_get_t) *pv;
    return 0; 
}

int
flow_bytes_get (uint64_t flow_id, flow_stats_get_t *flow_stats) {
    if (!(J1T (rc, per_flow_stats_bytes, flow_id))) {
        return 1;
    }

    JLG (pv, per_flow_stats_bytes, flow_id);
    *flow_stats = (flow_stats_get_t) *pv;
    return 0;
}

int
flow_packets_get (uint64_t flow_id, flow_stats_get_t *flow_stats) {
    if (!(J1T (rc, per_flow_stats_packets, flow_id))) {
        return 1;
    }

    JLG (pv, per_flow_stats_packets, flow_id);
    *flow_stats = (flow_stats_get_t) *pv;
    return 0;
}

void
state_init () {
    openflow_init (
        &table_add_fn_pointers,
        &table_mod_fn_pointers,
        &table_set_def_fn_pointers,
        &table_delete_fn_pointers,
        &bytes_hit_fn_pointers,
        &bytes_missed_fn_pointers,
        &pkts_hit_fn_pointers,
        &pkts_missed_fn_pointers,
        &per_flow_stats_bytes,
        &per_flow_stats_packets
    );
}

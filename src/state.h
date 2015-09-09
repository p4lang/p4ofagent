#ifndef _STATE_H_
#define _STATE_H_

/*
 * Wrapper typedefs and state accessor declarations
 */

#include <Judy.h>
#include "p4ofagent/openflow-spec1.3.0.h"
#include "p4_sim/pd.h"
#include "p4_sim/pre.h"
#include "loci/loci.h"

/**********************************************************************
 * Typedefs for wrapper functions. Arguments go as follows:
 *
 * @param action_args Judy array of OFPAT -> OFPAT arg val
 * @param packet_signature The packet signature described in callbacks.h
 * @param flow_id The flow id
 * @param entry hdl entry handle for table entry
 * @param priority Priority of entry, relevant if ternary
 * @param ttl Time to live
 * @param sess_hdl
 * @param dev_tgt
 * @param dev_id
 *
 **********************************************************************/

/* Table add wrapper type */
typedef p4_pd_status_t (*flow_add_wrapper_t)
(
    of_match_t *match_fields,
    Pvoid_t *action_args,
    uint64_t packet_signature,
    uint64_t flow_id,
    p4_pd_entry_hdl_t *entry_hdl,
    uint16_t *priority,
    uint32_t *ttl,
    p4_pd_sess_hdl_t sess_hdl,
    p4_pd_dev_target_t dev_tgt,
    uint8_t *signal
);

/* Table modify wrapper type */
typedef p4_pd_status_t (*flow_mod_wrapper_t)
(
    Pvoid_t *action_args,
    uint64_t packet_signature,
    uint64_t flow_id,
    p4_pd_entry_hdl_t entry_hdl,
    p4_pd_sess_hdl_t sess_hdl,
    uint8_t dev_id,
    uint8_t *signal
);

/* Table set default wrapper type */
typedef p4_pd_status_t (*flow_def_wrapper_t)
(
    Pvoid_t *action_args,
    uint64_t packet_signature,
    uint64_t flow_id,
    p4_pd_entry_hdl_t *entry_hdl,
    p4_pd_sess_hdl_t sess_hdl,
    p4_pd_dev_target_t dev_tgt,
    uint8_t *signal
);

/* Table delete wrapper type */
typedef p4_pd_status_t (*flow_del_t)
(
    p4_pd_sess_hdl_t sess_hdl,
    uint8_t dev_id,
    p4_pd_entry_hdl_t entry_hdl
);

/* Global table stats get wrapper type */
typedef uint64_t (*table_stats_get_t)
(
    p4_pd_sess_hdl_t sess_hdl,
    p4_pd_dev_target_t dev_tgt
);

/* Flow stats get wrapper type */
typedef uint64_t (*flow_stats_get_t)
(
    p4_pd_sess_hdl_t sess_hdl,
    p4_pd_dev_target_t dev_tgt,
    p4_pd_entry_hdl_t entry_hdl
);

/*********************************************************************
 * Agent state accessor functions
 *********************************************************************/

/* Get an entry handle from a flow id.
 * @param flow_id The flow id
 * @param hdl Pointer to entry handle
 * @return 0 on success, 1 on failure
 */
int
flow_id_entry_hdl_get (uint64_t flow_id, p4_pd_entry_hdl_t *hdl);

/* Associate an entry handle with a flow id.
 * @param flow_id The flow id
 * @param hdl The entry handle
 */
void
flow_id_entry_hdl_set (uint64_t flow_id, p4_pd_entry_hdl_t hdl);

/* Get an table_id from a flow id.
 * @param flow_id The flow id
 * @param table_id Pointer to table id
 * @return 0 on success, 1 on failure
 */
int
flow_id_table_id_get (uint64_t flow_id, uint8_t *table_id);

/* Associate an table_id with a flow id.
 * @param flow_id The flow id
 * @param table_id The table_id 
 */
void
flow_id_table_id_set (uint64_t flow_id, uint8_t table_id);

/* Remember group type.
 * @param group_id Id of the group
 * @param type One of OFPGT_* from openflow spec
 */
void
group_set_type (uint32_t group_id, enum ofp_group_type type);

/* Return type of group
 * @param group_id Id of the group
 */
enum ofp_group_type
group_get_type (uint32_t group_id);

/* Get table add function pointer from table id.
 * @param table_id The table id
 * @param add_f Pointer to wrapper function
 * @return 0 on success, 1 on failure
 */
int
table_add_get (uint8_t table_id, flow_add_wrapper_t *add_f);

/* Get table modify function pointer from table id.
 * @param table_id The table id
 * @param mod_f Pointer to wrapper function
 * @return 0 on success, 1 on failure
 */
int
table_mod_get (uint8_t table_id, flow_mod_wrapper_t *mod_f);

/* Get table set default function pointer from table id.
 * @param table_id The table id
 * @param def_f Pointer to wrapper function
 * @return 0 on success, 1 on failure
 */
int
table_def_get (uint8_t table_id, flow_def_wrapper_t *def_f);

/* Get table delete function pointer from table id.
 * @param table_id The table id
 * @param del_f Pointer to wrapper function
 * @return 0 on success, 1 on failure
 */
int
table_del_get (uint8_t table_id, flow_del_t *del_f);

/* Get bytes_hit counter function pointer from table id.
 * @param table_id The table id
 * @param hit_bytes Pointer to counter function
 * @return 0 on success, 1 on failure
 */
int
table_bytes_hit_get (uint8_t table_id, table_stats_get_t *hit_bytes);

/* Get bytes_missed counter function pointer from table id.
 * @param table_id The table id
 * @param missed_bytes Pointer to counter function
 * @return 0 on success, 1 on failure
 */
int
table_bytes_missed_get (uint8_t table_id, table_stats_get_t *missed_bytes);

/* Get pkts_hit counter function pointer from table id.
 * @param table_id The table id
 * @param hit_pkts Pointer to counter function
 * @return 0 on success, 1 on failure
 */
int
table_packets_hit_get (uint8_t table_id, table_stats_get_t *hit_pkts);

/* Get pkts_missed counter function pointer from table id.
 * @param table_id The table id
 * @param missed_pkts Pointer to counter function
 * @return 0 on success, 1 on failure
 */
int
table_packets_missed_get (uint8_t table_id, table_stats_get_t *missed_pkts);

/* Get packet counter function pointer from table id.
 * @param table_id The table id
 * @param flow_stats Pointer to counter function
 * @return 0 on success, 1 on failure
 */
int
flow_packets_get (uint64_t flow_id, flow_stats_get_t *flow_stats);

/* Get byte counter function pointer from table id.
 * @param table_id The table id
 * @param flow_stats Pointer to counter function
 * @return 0 on success, 1 on failure
 */
int
flow_bytes_get (uint64_t flow_id, flow_stats_get_t *flow_stats);

/* Initializes agent state per p4 program */
void
state_init ();

#endif /* _STATE_H_ */

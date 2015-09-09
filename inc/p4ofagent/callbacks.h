#ifndef _P4OFAGENT_TABLE_CALLBACKS_H_
#define _P4OFAGENT_TABLE_CALLBACKS_H_

#include <stdint.h>

#include "indigo/indigo.h"
#include "indigo/fi.h"

#define MH_OFFSET 48
#define OXMTLV_HEADER_LEN 4

/* Various functions use bit-signatures to represent action-sets 
 * specified by the controller. Here are functions for setting or
 * checking bits in a uint32_t */

void
signature_set_bit (int bit, uint32_t *sig);

int
signature_check_bit (int bit, uint32_t *sig);

/* Generic callbacks for table operations. Operations are demuxed to their
 * proper tables from within. */

indigo_error_t
flow_create (void *table_priv, indigo_cxn_id_t cxn_id, of_flow_add_t *flow_add,
             indigo_cookie_t flow_id, void **entry_priv);

indigo_error_t
flow_modify (void *table_priv, indigo_cxn_id_t cxn_id, void *entry_priv,
             of_flow_modify_strict_t *flow_modify);

indigo_error_t
flow_delete (void *table_priv, indigo_cxn_id_t cxn_id, void *entry_priv,
             indigo_fi_flow_stats_t *flow_stats);

indigo_error_t
table_stats_get (void *table_priv, indigo_cxn_id_t cxn_id,
                 indigo_fi_table_stats_t *table_stats);

indigo_error_t
flow_stats_get (void *table_priv, indigo_cxn_id_t cxn_id, void *entry_priv,
                indigo_fi_flow_stats_t *flow_stats);

indigo_error_t
group_create (void *table_priv, indigo_cxn_id_t cxn_id, uint32_t group_id,
              uint8_t group_type, of_list_bucket_t *buckets, void **entry_priv);

indigo_error_t
group_modify (void *table_priv, indigo_cxn_id_t cxn_id, void *entry_priv,
              of_list_bucket_t *buckets);

indigo_error_t
group_delete (void *table_priv, indigo_cxn_id_t cxn_id, void *entry_priv);

indigo_error_t
group_stats (void *table_priv, void *entry_priv, of_group_stats_entry_t *stats);

#endif /* _P4OFAGENT_CALLBACKS_H_ */

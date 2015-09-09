/*
 * Implementation of some OF logic. This file is equired by IVS.
 */

#include "indigo/error.h"
#include "indigo/types.h"
#include "indigo/fi.h"
#include "indigo/port_manager.h"
#include "p4_sim/pd_wrappers.h"

indigo_error_t
indigo_port_features_get (of_features_reply_t *fr) {
    of_wire_buffer_u8_set (fr->wbuf, 16, 0);
    of_wire_buffer_u8_set (fr->wbuf, 20, num_openflow_tables ());
    of_wire_buffer_u32_set (fr->wbuf, 23, 3);
    return INDIGO_ERROR_NONE;
}

indigo_error_t
indigo_port_desc_stats_get (of_port_desc_stats_reply_t *reply) {
    return INDIGO_ERROR_NOT_SUPPORTED;
}

indigo_error_t
indigo_port_interface_list (indigo_port_info_t** list) {
    return INDIGO_ERROR_NOT_SUPPORTED;
}

void
indigo_port_interface_list_destroy (indigo_port_info_t* list) {
}

indigo_error_t
indigo_port_modify (of_port_mod_t *port_mod) {
    return INDIGO_ERROR_NOT_SUPPORTED;
}

indigo_error_t
indigo_port_stats_get (of_port_stats_request_t *request, of_port_stats_reply_t **reply) {
    *reply = of_port_stats_reply_new (request->version);
    return INDIGO_ERROR_NOT_SUPPORTED;
}

indigo_error_t
indigo_port_queue_config_get (of_queue_get_config_request_t *request,
                             of_queue_get_config_reply_t **reply) {
    return INDIGO_ERROR_NOT_SUPPORTED;
}

indigo_error_t
indigo_port_queue_stats_get (of_queue_stats_request_t *request,
                             of_queue_stats_reply_t **reply) {
    return INDIGO_ERROR_NOT_SUPPORTED;
}

indigo_error_t
indigo_port_experimenter (of_experimenter_t *e, indigo_cxn_id_t cxn_id) {
    return INDIGO_ERROR_NOT_SUPPORTED;
}

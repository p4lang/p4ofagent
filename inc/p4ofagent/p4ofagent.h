#ifndef _P4OFAGENT_H_
#define _P4OFAGENT_H_

#include <stdbool.h>
#include "p4_sim/pre.h"

/* Memory allocation */
#define P4OFAGENT_MALLOC malloc

/* Device target and ID */
#define P4_SINGLE_DEVICE ((p4_pd_dev_target_t) { 0, 0xffff })
#define P4_DEVICE  0xffff
#define P4_DEVICE_ID 0

/* Loggin macro */
#define P4_LOG perror

/* Just handy */
#define TRUE  1
#define FALSE 0

/* Session IDs for transactional state changes */
p4_pd_sess_hdl_t P4_PD_SESSION;
p4_pd_sess_hdl_t P4_PRE_SESSION;

/* Common multicast ID for flooding */
mc_mgrp_hdl_t AGENT_ETHERNET_FLOOD_MC_HDL;

/* Entry point for the program. Starts the Indigo select/accept
 * loop in a seperate thread. 'ipv6' indicates whether the
 * 'ip_ctl' string is an ipv4 address or ipv6 address. */
void p4ofagent_init (bool ipv6, char *ip_ctl);

#endif /* _P4OFAGENT_H_ */

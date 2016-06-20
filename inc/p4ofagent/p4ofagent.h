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

#ifndef _P4OFAGENT_H_
#define _P4OFAGENT_H_

#include <stdbool.h>

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

/* Header changes for bmv2 */
#ifdef _BMV2_
#include <bm/pdfixed/pd_pre.h>
p4_pd_entry_hdl_t AGENT_ETHERNET_FLOOD_MC_HDL;
#else
#include <p4_sim/pre.h>
mc_mgrp_hdl_t AGENT_ETHERNET_FLOOD_MC_HDL;
#endif // _BMV2_

/* Session IDs for transactional state changes */
p4_pd_sess_hdl_t P4_PD_SESSION;
p4_pd_sess_hdl_t P4_PRE_SESSION;

/* Entry point for the program. Starts the Indigo select/accept
 * loop in a seperate thread. 'ipv6' indicates whether the
 * 'ip_ctl' string is an ipv4 address or ipv6 address. */
void p4ofagent_init (bool ipv6, char *ip_ctl);

#endif /* _P4OFAGENT_H_ */

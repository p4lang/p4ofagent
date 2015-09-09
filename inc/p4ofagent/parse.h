#ifndef _PARSE_H_
#define _PARSE_H

#include <Judy.h>
#include <stdint.h>

#include "indigo/types.h"
#include "indigo/error.h"
#include "indigo/fi.h"

/* Parses an of_action_t into more managable variables.
 * @param action The action to parse
 * @param type the type of action this is (OFPAT_*)
 * @param arg a pointer to the address of the value of the argument of
          this action. This function populates that memory.
 * @param hdr_field The packet field type referred to 
          by an OFPAT_SET_FIELD action. Clearly will not always contain
          useful information.
 */
void
parse_ofpat (of_action_t *action, uint16_t *type,
             uint8_t **arg, uint8_t *hdr_field);

/* Parses an action list, storing action -> action arg val pairs in aargs.
 * @param actions The list of actions
 * @param aargs A Judy array to be filled in with action type -> arg pairs
 * @param sig A bit-signature (bitmap) indicating which actions are present
          in the action list.
 */
void
parse_actions (of_list_action_t *actions, Pvoid_t *aargs,
               uint32_t *sig);

/* Parses instructions in a similar fasion to parse_actions. */
void
parse_instructions (of_list_instruction_t *instructions,
                    Pvoid_t *actions, uint32_t *sig);

#endif /* _PARSE_H_ */

/*
 * cc_hal.h
 *
 *  Created on: Mar 4, 2016
 *      Author: pmatthews
 */

#ifndef CC_HAL_H_
#define CC_HAL_H_

#include "types.h"


typedef struct {
uint32_t base_freq;					//

} RF_CONFIG;





// Commands
uint8_t cc_reset(void);
uint8_t cc_cmd_rx(void);

uint8_t cc_status_update(void);

int cc_set_base_frequency(uint32_t base_freq);


// Public functions
int cc_radio_config(RF_CONFIG *config);
uint8_t StrToCmd(char *cmd);
int cc_state_to_str(uint8_t state, char *s);


#endif /* CC_HAL_H_ */

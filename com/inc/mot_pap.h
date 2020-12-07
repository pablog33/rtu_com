#ifndef MOT_PAP_H_
#define MOT_PAP_H_

#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "semphr.h"
//#include "pid.h"
//#include "tmr.h"

#ifdef __cplusplus
extern "C" {
#endif

struct mot_pap *pole_get_status(void);
struct mot_pap *arm_get_status(void);
void arm_init();
void pole_init();

enum mot_pap_direction {
	MOT_PAP_DIRECTION_CW, MOT_PAP_DIRECTION_CCW,
};

enum mot_pap_type {
	MOT_PAP_TYPE_FREE_RUNNING, MOT_PAP_TYPE_CLOSED_LOOP, MOT_PAP_TYPE_STOP
};


struct mot_pap_msg {
	enum mot_pap_type type;
	enum mot_pap_direction free_run_direction;
	uint32_t free_run_speed;
	uint16_t closed_loop_setpoint;
};

struct mot_pap {
//	char *name;
	enum mot_pap_type type;
	enum mot_pap_direction dir;
	uint16_t posCmd;
	uint16_t posAct;
	uint32_t freq;
	uint16_t cwLimit;
	uint16_t ccwLimit;
//	volatile bool cwLimitReached;
//	volatile bool ccwLimitReached;
	volatile bool stalled;
//	struct ad2s1210 *rdc;
//	struct pid *pid;
	SemaphoreHandle_t supervisor_semaphore;
	//struct mot_pap_gpios gpios;
	//struct tmr tmr;
//	enum mot_pap_direction last_dir;
//	uint32_t half_pulses;			// counts steps from the last call to supervisor task
//	uint16_t offset;
};



#ifdef __cplusplus
}
#endif

#endif /* MOT_PAP_H_ */

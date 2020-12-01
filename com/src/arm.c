#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* SM13 includes */
//#include "mot_pap.h"
#include "rtu_com_hmi.h"
#include "debug.h"

#define arm_TASK_PRIORITY ( configMAX_PRIORITIES - 2 )

static struct mot_pap_status status;
static void arm_supervisor_task();


static void arm_task(void* par)
{
	mpap_t *msg_rcv;

	while (1) {
		if (xQueueReceive(arm_queue, &msg_rcv, portMAX_DELAY) == pdPASS)
		{
			lDebug(Debug, "arm: command received");

			switch (msg_rcv->type)
			{

			case MOT_PAP_MSG_TYPE_FREE_RUNNING:
				lDebug(Info, "Arm free_run_direction:", msg_rcv->free_run_direction);
				/*	cw/ccw limits!!	*/
				if (msg_rcv->free_run_direction) { lDebug(Debug, "Giro Anti-horario"); }else { lDebug(Debug, "Giro Horario"); }
				lDebug(Info, "Arm free_run_speed:", msg_rcv->free_run_speed);
				break;
			case MOT_PAP_MSG_TYPE_CLOSED_LOOP:	//PID
				lDebug(Info, "Arm closed_loop_setpoint:", msg_rcv->closed_loop_setpoint);
				//calcular error de posici�n
				break;
			default:
				lDebug(Info, "STOP arm.c");
				break;
			}

			vPortFree(msg_rcv);

		}
	}

}

static void arm_supervisor_task()
{
	status.dir = MOT_PAP_STATUS_STOP;
	status.posCmd = 0;
	status.posAct = 0xEEEE;
	status.vel = 4;
	status.cwLimit = 0;
	status.ccwLimit = 0;
}


void arm_init()
{
	
	arm_queue = xQueueCreate(5, sizeof(struct mot_pap_msg*));
	//xTaskCreate(arm_task, "arm", configMINIMAL_STACK_SIZE, NULL, arm_TASK_PRIORITY, NULL);
	xTaskCreate(arm_task, "arm", configMINIMAL_STACK_SIZE*2, NULL, 4, NULL);
	lDebug(Debug, "arm.c", "arm_task - TaskCreate"); //Pablo Priority Debug: Borrar
}

struct mot_pap_status arm_get_status(void)
{
	arm_supervisor_task();
	return status;
}

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

#define POLE_TASK_PRIORITY ( configMAX_PRIORITIES - 2 )

static struct mot_pap_status status;
static void pole_supervisor_task();
static uint16_t pole_simu; bool cw, ccw;
static void pole_task(void* par)
{
	mpap_t *msg_rcv;

	while (1) {
		if (xQueueReceive(pole_queue, &msg_rcv, portMAX_DELAY) == pdPASS)
		{
			lDebug(Info, "pole: command received");

			switch (msg_rcv->type)
			{
			case MOT_PAP_MSG_TYPE_FREE_RUNNING:
				lDebug(Info, "Pole free_run_direction:", msg_rcv->free_run_direction);
				if (msg_rcv->free_run_direction) { lDebug(Info, "Giro Anti-horario"); ccw = true; }
				else { lDebug(Info, "Giro Horario"); cw = true; }
				lDebug(Info, "Pole free_run_speed:",msg_rcv->free_run_speed);
				break;

			case MOT_PAP_MSG_TYPE_CLOSED_LOOP:	//PID
				lDebug(Info,"Pole closed_loop_setpoint:",msg_rcv->closed_loop_setpoint);
				break;

			default:
				cw, ccw = false;
				lDebug(Info, "STOP Pole");
				break;
			}

			vPortFree(msg_rcv);

		}
	}

}

void pole_supervisor_task()
{	

	status.dir = MOT_PAP_STATUS_STOP;
	status.posCmd = 0;
	if (cw) { if (pole_simu == 0xFFFF) {} else { ++pole_simu; } }
	else if (ccw) { if (pole_simu == 0x0000) {} else { --pole_simu; } }
	status.posAct = pole_simu;
	status.vel = 5;
	if (pole_simu == 0xFFFF) { status.cwLimit = 1; } else { status.cwLimit = 0; }
	if (pole_simu == 0x0000) { status.ccwLimit = 1; } else { status.ccwLimit = 0; }
}


void pole_init()
{
	pole_queue = xQueueCreate(5, sizeof(struct mot_pap_msg*));
	//xTaskCreate(pole_task, "Pole", configMINIMAL_STACK_SIZE, NULL, POLE_TASK_PRIORITY, NULL);
	xTaskCreate(pole_task, "Pole", configMINIMAL_STACK_SIZE*2, NULL, 4, NULL);
	lDebug(Debug, "pole.c", "pole_task - TaskCreate"); //Pablo Priority Debug: Borrar
}

struct mot_pap_status pole_get_status(void)
{
	pole_supervisor_task();
	return status;
}

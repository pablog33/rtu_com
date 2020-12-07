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
#include "mot_pap.h"
#include "rtu_com_hmi.h"
#include "debug.h"

#define POLE_TASK_PRIORITY ( configMAX_PRIORITIES - 2 )

struct mot_pap pole;
static void pole_supervisor_task();
static uint16_t cw, ccw;

static void pole_task(void* par)
{
	struct mot_pap_msg *msg_rcv;

	while (1) {
		if (xQueueReceive(pole_queue, &msg_rcv, portMAX_DELAY) == pdPASS)
		{
			lDebug(Debug, "pole: command received");

			switch (msg_rcv->type)
			{
			case MOT_PAP_TYPE_FREE_RUNNING:
				lDebug(Debug, "Pole free_run_direction: %d", msg_rcv->free_run_direction);
				if (msg_rcv->free_run_direction) { lDebug(Info, "Giro Anti-horario"); ccw = true; }
				else { lDebug(Info, "Giro Horario"); cw = true; }
				lDebug(Info, "Pole free_run_speed: %d",msg_rcv->free_run_speed);
				break;

			case MOT_PAP_TYPE_CLOSED_LOOP:	//PID
				lDebug(Info,"Pole closed_loop_setpoint: %x",msg_rcv->closed_loop_setpoint);
				break;

			default:
				cw = false;
				ccw = false;
				lDebug(Info, "STOP POLE");
				break;
			}

			vPortFree(msg_rcv);

		}
	}

}

void pole_supervisor_task()
{	

	pole.dir = MOT_PAP_TYPE_STOP;
	pole.posCmd = 0xFFEE;
	pole.posAct = 0xFFEE;
	pole.freq = 5;

}


void pole_init()
{
	pole_queue = xQueueCreate(5, sizeof(struct mot_pap_msg*));
	//xTaskCreate(pole_task, "Pole", configMINIMAL_STACK_SIZE, NULL, POLE_TASK_PRIORITY, NULL);
	xTaskCreate(pole_task, "Pole", configMINIMAL_STACK_SIZE*2, NULL, 4, NULL);
	lDebug(Debug, "pole.c", "pole_task - TaskCreate"); //Pablo Priority Debug: Borrar
}

struct mot_pap *pole_get_status(void)
{
	pole_supervisor_task();
	return &pole;
}

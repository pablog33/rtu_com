#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "stdbool.h"
//#include "relay.h"
#include "rtu_com_hmi.h"
#include "debug.h"



#define LIFT_TASK_PRIORITY ( configMAX_PRIORITIES - 2 )

static struct lift_status status;
static void lift_supervisor_task();

static void lift_task(void *par)
{
	lift_t* msg_rcv;

	while (1) {

		if (xQueueReceive(lift_queue, &msg_rcv, portMAX_DELAY) == pdPASS) {

				switch (msg_rcv->type) {
				case LIFT_TYPE_UP:
					lDebug(Error, "lift.c UP");

					break;
				case LIFT_TYPE_DOWN:
					lDebug(Error, "lift.c DOWN");
					break;
				default:
					lDebug(Error, "lift.c STOP");
					break;
				}

			//free(msg_rcv);

		} else {
			lDebug(Error, "lift: no command received");
		}
	}
}


static void lift_supervisor_task()
{
	status.type = LIFT_TYPE_STOP;
	status.upLimit = false;
	status.downLimit = false;
}

void lift_init()
{
	lift_queue = xQueueCreate(5, sizeof(struct lift_msg*));
	xTaskCreate(lift_task, "lift", configMINIMAL_STACK_SIZE*2, NULL, 4, NULL);
}

liftstatus_t lift_get_status(void)
{
	lift_supervisor_task();
	return status;
}

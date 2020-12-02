
#include <rtu_com_hmi.h>
#include "debug.h"

#include "lwip/opt.h"

#if LWIP_NETCONN

#include "lwip/sys.h"
#include "lwip/api.h"
#include<string.h>
/*-----------------------------------------------------------------------------------*/
static void 
tcp_thread(void *arg)
{
	LWIP_UNUSED_ARG(arg);
	struct netconn *conn, *newconn;
	err_t err_accept, err_recv, err_send, err_dataBuf;
	void *unused;

	/* Nuevo identificador de conexion -conn- */
	conn = netconn_new(NETCONN_TCP);

	/* Enlace de la conexión en puerto 5020 */
	netconn_bind(conn, NULL, 5020);

	/* Socket generado en modo escucha */
	netconn_listen(conn);
	lDebug(Info, "Listening on port: 5020 \n\r");

	for(;;) /* Bloqueo de hilo, en espera de nueva conexion */
	{
		/* Aceptar nueva conexion */
		if (err_accept = netconn_accept(conn, &newconn) == ERR_OK)
		{
			struct netbuf *buf;
			uint16_t uiLenRecvData;
			HMIData_t *pHMIData;
			HMICmd_t HMICmd;
			RTUData_t RTUDataTx;
			uint16_t res;
			mpapstatus_t ArmStatus;
			mpapstatus_t PoleStatus;
			liftstatus_t LiftStatus;
			uint16_t iServerStatus = 0x00;
			uint32_t cycleCount = 0;

			newconn->recv_timeout = RCV_TIMEO;

			arm_init();
			pole_init();
			lift_init();

			while ((err_recv = netconn_recv(newconn, &buf)) == ERR_OK)
			{
				do
				{
					++cycleCount;

					if( (err_dataBuf = netbuf_data(buf, &pHMIData, &uiLenRecvData) ) != ERR_OK)
					{	lDebug(Error, "Error en funcion NETCONN -netbuf_data-"); prvNetconnError(err_dataBuf);
						iServerStatus = ERROR_NETCONN;
						break;
					}

					if( ( iServerStatus = NetValuesReceivedFromHMI(pHMIData, &HMICmd, uiLenRecvData) ) != ERR_OK)
					{ break; }

					TaskTriggerMsg(&HMICmd);

					NetValuesToSendFromRTU(iServerStatus, &RTUDataTx, &ArmStatus, &PoleStatus, &LiftStatus);

					lDebug(Debug, "%d", cycleCount);

					if((err_send = netconn_write(newconn, RTUDataTx.buffer, sizeof(RTUDataTx.buffer), NETCONN_COPY)) != ERR_OK)
					{	lDebug(Error, "Error en funcion NETCONN -netbuf_data-"); prvNetconnError(err_send);
						iServerStatus = ERROR_NETCONN;
						break;
					}

				} while (netbuf_next(buf) >= 0);

				netbuf_delete(buf);

				if( ( ( iServerStatus && 0x80  ) != ERR_OK ) )
				{	break;	}

			  } /* while-netconn_recv */



			if( err_recv )	{	lDebug(Error, "Error en funcion NETCONN -netconn_recv-"); prvNetconnError(err_recv);	}

			prvEmergencyStop();

			lDebug(Debug, "	- ** 	Desconexion RTU 	** - ");

			/* Close connection and discard connection identifier. */
			netconn_close(newconn);
			netconn_delete(newconn);

		} /*	if-netconn_accept	*/

	} /* for(;;) */

} /* tcp_thread() */
/*-----------------------------------------------------------------------------------*/
void stackIp_ThreadInit(void)
{
  sys_thread_new("tcp_thread", tcp_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/
/**
 * @brief 	Only prints out errors related with NETCONN LWIP module.
 * @param 	LWIP error code
 * @return	never
 * @note	Only prints. Variable iServerStatus for hanlde error events, #define ERROR_NETCONN 0x84.
 */
void prvNetconnError(err_t err)
{
	switch(err)
	{
	case(ERR_TIMEOUT):
		lDebug(Error, "\n ENET - RecvTimeOut - Se detienen procesos!! \n");
	case(ERR_ARG):
			lDebug(Error, "\n ENET - Argumento de funcion -netconn_recv- ilegal - Se detienen procesos!! \n");
	case(ERR_CONN):
			lDebug(Error, "\n Problemas de conexion - Se detienen procesos!! \n");
	case(ERR_CLSD):
			lDebug(Error, "\n Closed Connection - Se detienen procesos!! \n");
	case(ERR_BUF):
		lDebug(Error, "\n Error al generar Buffer - Se detienen procesos!! \n");
	}
	return;
}

#endif /* LWIP_NETCONN */

///*-----------------------------------------------------------*/
/**
 * @brief 	Stops movements for all three axis on exit ethernet loop with HMI.
 * @param 	none.
 * @return	never.
 * @note	Stops movements on HMI communication lost.
 */
static void prvEmergencyStop(void)
{
	mpap_t* pArmMsg;
	mpap_t* pPoleMsg;
	lift_t *pLiftMsg;

	pArmMsg = (mpap_t*)pvPortMalloc(sizeof(mpap_t));
	pArmMsg->type = MOT_PAP_MSG_TYPE_STOP;
	if (xQueueSend(arm_queue, &pArmMsg, portMAX_DELAY) == pdPASS) { lDebug(Info, " Comando enviado a arm.c exitoso!"); }
				else { lDebug(Info, "Comando NO PUDO ser enviado a arm.c"); }

	pPoleMsg = (mpap_t*)pvPortMalloc(sizeof(mpap_t));
	pPoleMsg->type = MOT_PAP_MSG_TYPE_STOP;
	if (xQueueSend(pole_queue, &pPoleMsg, portMAX_DELAY) == pdPASS) { lDebug(Info, "Comando enviado a pole.c exitoso!"); }
				else { lDebug(Info, "Comando NO PUDO ser enviado a pole.c"); }

	pLiftMsg = (lift_t*)pvPortMalloc(sizeof(lift_t));
	pLiftMsg->type = LIFT_TYPE_STOP;
	if (xQueueSend(lift_queue, &pLiftMsg, portMAX_DELAY) == pdPASS) { lDebug(Info, "Comando enviado a lift.c exitoso!"); }
	else { lDebug(Info, "Comando NO PUDO ser enviado a lift.c"); }

	return;

}

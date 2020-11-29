
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
	err_t err;
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
		if (err = netconn_accept(conn, &newconn) == ERR_OK)
		{
			struct netbuf *buf;
			u16_t len_recvData;
			HMIData_t *pHMIData;
			HMICmd_t HMICmd;
			RTUData_t RTUDataTx;
			uint16_t res;
			mpapstatus_t ArmStatus;
			mpapstatus_t PoleStatus;
			liftstatus_t LiftStatus;
			uint16_t iServerStatus = 0x00;

			newconn->recv_timeout = RCV_TIMEO;

			arm_init();
			pole_init();
			lift_init();

			while ((err = netconn_recv(newconn, &buf)) == ERR_OK)
			{
				do
				{
					netbuf_data(buf, &pHMIData, &len_recvData);

					NetValuesReceivedFromHMI(pHMIData, &HMICmd);
	/* ------------------------------------------------------------------------*/
//
					NetValuesToSendFromRTU(iServerStatus, &RTUDataTx, &ArmStatus, &PoleStatus, &LiftStatus);

					err = netconn_write(newconn, RTUDataTx.buffer, sizeof(RTUDataTx.buffer), NETCONN_COPY);

					lDebug(Debug, "%d", err);

					//RTUData.pos = 0xFE;
//					 snprintf(RTUData.cmd, 5, "%s", "hola");
//
//					 snprintf(RTUData.buffer, 8, "%x %s", RTUData.pos, RTUData.cmd);
//					 err = netconn_write(newconn, RTUData.buffer, sizeof(RTUData.buffer), NETCONN_COPY);


				} while (netbuf_next(buf) >= 0);

				netbuf_delete(buf);

			  }

			lDebug(Debug, "Desconexion RTU - ");

			lDebug(Debug, "%d", err);

			prvDebugErrorTxRx(err);

			/*printf("Got EOF, looping\n");*/
			  /* Close connection and discard connection identifier. */
			  netconn_close(newconn);
			  netconn_delete(newconn);
			  //tcp_thread(unused);

		} /*	while	*/

	} /* while(1) */

} /* tcp_thread() */
/*-----------------------------------------------------------------------------------*/
void stackIp_ThreadInit(void)
{
  sys_thread_new("tcp_thread", tcp_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/

void prvDebugErrorTxRx(err_t err)
{
	switch(err)
	{
	case(ERR_TIMEOUT):
		lDebug(Error, "RecvTimeOut - Se detienen procesos!! \n");
	case(ERR_ARG):
			lDebug(Error, "Argumento de funcion -netconn_recv- ilegal - Se detienen procesos!! \n");
	case(ERR_CONN):
			lDebug(Error, "Problemas de conexion - Se detienen procesos!! \n");
	case(ERR_CLSD):
			lDebug(Error, "Closed Connection - Se detienen procesos!! \n");
	}
	return;
}

#endif /* LWIP_NETCONN */

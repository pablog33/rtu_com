
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
			RTUData_t RTUData;
			uint16_t res;

			newconn->recv_timeout = RCV_TIMEO;

			while ((err = netconn_recv(newconn, &buf)) == ERR_OK)
			{
				do
				{
					netbuf_data(buf, &pHMIData, &len_recvData);

					NetValuesReceivedFromHMI(pHMIData, &HMICmd);

					//lDebug(Debug, "%d", ppHMIData->posCmdArm);

//					 if (!(res = strncmp(ppHMIData->pos, "ho", 2)))
//					 {
//						 lDebug(Debug, "ho");
//					 }
//					 if (!(res = strncmp(ppHMIData->cmd, "la", 2)))
//					 {
//						 lDebug(Debug, "la");
//					 }

//					HMICmd.posCmdArm = pHMIData->posCmdArm;
//					HMICmd.velCmdArm = pHMIData->velCmdArm;
//					HMICmd.posCmdPole = pHMIData->posCmdPole;
//					HMICmd.velCmdPole = pHMIData->velCmdPole;
//
//
//					/*		-- HMICmd Frame Parsing --	*/
//					/*	A partir de los descriptores -*pHMIData- se obtienen los valores para HMICmd contenidos en la trama recibida.
//					*/
//
//					/*	-- mode --	*/
//					if(!strncmp(pHMIData->mode, "STOP;", HMI_NETVAR_SIZE))	{	HMICmd.mode = eStop;	}
//					else if (!strncmp(pHMIData->mode, "FRUN;", HMI_NETVAR_SIZE)) {	HMICmd.mode = eFree_run;	}
//					else if (!strncmp(pHMIData->mode, "AUTO;", HMI_NETVAR_SIZE)) {	HMICmd.mode = eAuto; }
//					else if (!strncmp(pHMIData->mode, "LIFT;", HMI_NETVAR_SIZE)) { HMICmd.mode = eLift; }
//					else { lDebug(Error,"error- HMICmd.mode"); }
//
//					/*	--	freeRunAxis --	*/
//					if (!strncmp(pHMIData->freeRunAxis, "ARM_;", HMI_NETVAR_SIZE)) { HMICmd.freeRunAxis = eArm; }
//					else if (!strncmp(pHMIData->freeRunAxis, "POLE;", HMI_NETVAR_SIZE)) { HMICmd.freeRunAxis = ePole; }
//					else { lDebug(Error,"error- HMICmd.freeRunAxis"); }
//
//					/*	--	freeRunDir --	*/
//					if (!strncmp(pHMIData->freeRunDir, "CW__;", HMI_NETVAR_SIZE)) { HMICmd.freeRunDir = eCW; }
//					else if (!strncmp(pHMIData->freeRunDir, "CCW_;", HMI_NETVAR_SIZE)) { HMICmd.freeRunDir = eCCW; }
//					else { lDebug(Error,"error- HMICmd.freeRunDir"); }
//
//					/*	-- ctrlEn --	*/
//					if (!strncmp(pHMIData->ctrlEn, "CTLE;", HMI_NETVAR_SIZE)) { HMICmd.ctrlEn = eEnable; }
//					else if (!strncmp(pHMIData->ctrlEn, "DCTL;", HMI_NETVAR_SIZE)) { HMICmd.ctrlEn = eDesable; }
//					else { lDebug(Error,"error- HMICmd.ctrlEn"); }
//
//					/*	-- stallEn --	*/
//					if (!strncmp(pHMIData->stallEn, "STLE;", HMI_NETVAR_SIZE)) { HMICmd.stallEn = eEnable; }
//					else if (!strncmp(pHMIData->stallEn, "DSTL;", HMI_NETVAR_SIZE)) { HMICmd.stallEn = eDesable; }
//					else { lDebug(Error,"error- HMICmd.stallEn"); }
//
//					/*	-- liftDir --	*/
//					if (!strncmp(pHMIData->liftDir, "LFUP;", HMI_NETVAR_SIZE)) { HMICmd.liftDir = eUp; }
//					else if (!strncmp(pHMIData->liftDir, "LFDW;", HMI_NETVAR_SIZE)) { HMICmd.liftDir = eDown; }
//					else { lDebug(Error,"error- HMICmd.liftDir"); }
//
//					/*	-- clientID --*/
//					if (!strncmp(pHMIData->clientId, "SM13;", HMI_NETVAR_SIZE)) { HMICmd.clientID = eSigned; }
//					else { lDebug(Error,"error- HMICmd.ClientID", HMI_NETVAR_SIZE); HMICmd.clientID = eUnsigned; }


	/* ------------------------------------------------------------------------*/
					 RTUData.pos = 0xFE;
					 snprintf(RTUData.cmd, 5, "%s", "hola");

					 snprintf(RTUData.buffer, 8, "%x %s", RTUData.pos, RTUData.cmd);
					 err = netconn_write(newconn, RTUData.buffer, sizeof(RTUData.buffer), NETCONN_COPY);


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

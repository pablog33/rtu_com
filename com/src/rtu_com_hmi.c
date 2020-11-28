/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
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
			HMIData_t HMIData;
			RTUData_t RTUData;
			HMIData_t *pHMIData;
			uint16_t res;

			newconn->recv_timeout = RCV_TIMEO;

			while ((err = netconn_recv(newconn, &buf)) == ERR_OK)
			{
				do
				{
					netbuf_data(buf, &pHMIData, &len_recvData);

					 if (!(res = strncmp(pHMIData->pos, "ho", 2)))
					 {
						 lDebug(Debug, "ho");
					 }
					 if (!(res = strncmp(pHMIData->cmd, "la", 2)))
					 {
						 lDebug(Debug, "la");
					 }

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

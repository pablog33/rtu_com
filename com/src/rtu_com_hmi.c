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
  struct netconn *conn, *newconn;
  err_t err;
  LWIP_UNUSED_ARG(arg);
  void *unused;


  /* Create a new connection identifier. */
  conn = netconn_new(NETCONN_TCP);

  /* Bind connection to well known port number 7. */
  netconn_bind(conn, NULL, 5020);

  /* Tell connection to go into listening mode. */
  netconn_listen(conn);
  printf("Listening on port 5020 %p\n\r", newconn);

  while (1)
  {

	/* Grab new connection. */
	err = netconn_accept(conn, &newconn);				/* GPa 201124 1440 Quito del lazo infinito */
	lDebug(Info, "accepted new connection %p\n", newconn);

	if (err == ERR_OK) {

	struct netbuf *buf;
	u16_t len_recvData;
	HMIData_t HMIData;
	RTUData_t RTUData;
	HMIData_t *pHMIData;
	uint16_t res;

	/* Process the new connection. */

	  buf = netbuf_new(); /* create a new netbuf */
	  netbuf_alloc(buf, 50); /* allocate 100 bytes of buffer */
	  lDebug(Debug, "buf creado");

	  newconn->recv_timeout = 100;

		while ((err = netconn_recv(newconn, &buf)) == ERR_OK)
		{

			lDebug(Debug,"conexion recibida");
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
				 err = netconn_write(newconn, RTUData.buffer, sizeof(RTUData.buffer), NETCONN_COPY); /* GPa 201123 1430 Reemplazo data por tempBuffer */
				 //printf("%s", data);

			} while (netbuf_next(buf) >= 0);

			netbuf_delete(buf);
			lDebug(Debug, "bufer eliminado");

		  }

		lDebug(Debug, "%s", err);

		/*printf("Got EOF, looping\n");*/
		  /* Close connection and discard connection identifier. */
		  netconn_close(newconn);
		  netconn_delete(newconn);
		  //tcp_thread(unused);

		} /*	while	*/



  } /* while(1) */

} /* tcp_thread() */
/*-----------------------------------------------------------------------------------*/
void
stackIp_init(void)
{
  sys_thread_new("tcp_thread", tcp_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);
}
/*-----------------------------------------------------------------------------------*/

#endif /* LWIP_NETCONN */

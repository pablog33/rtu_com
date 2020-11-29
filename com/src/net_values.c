/* SM13 Remote Terminal Unit -RTU- Prototype TCP Server
  Automatically connects to HMI running on remote client */


  /* SM13 Remote Terminal Unit -RTU- Prototype TCP Server
	Automatically connects to HMI running on remote client */



	/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


/*	RTUcomHMI	*/
#include "rtu_com_hmi.h"
#include "debug.h"

/*	Motor Control	*/
#include "mot_pap.h"

//void NetValuesToSendFromRTU(int16_t iServerStatus, RTUData_t *pRTUDataTx, mpapstatus_t* pArmStatus, mpapstatus_t* pPoleStatus, liftstatus_t* pLiftStatus)
//{
//
//	*(pPoleStatus) = pole_get_status();	/*	Obtengo los valores de los dispositivos internos -Arm, Pole, LIFT-	*/
//	*(pArmStatus) = arm_get_status();
//	*(pLiftStatus) = lift_get_status();
//
//	pRTUDataTx->resActArm = pArmStatus->posAct;
//	pRTUDataTx->resActPole = pPoleStatus->posAct;
//	pRTUDataTx->velActArm = pArmStatus->vel;
//	pRTUDataTx->velActPole = pPoleStatus->vel;
//
//	/*	-- cwLimitArm --	*/
//	if (pArmStatus->cwLimit)	{	sprintf(pRTUDataTx->cwLimitArm, "%s", "ACW_LIM;");	}
//	else {	sprintf(pRTUDataTx->cwLimitArm, "%s", "ACW_RUN;");	}
//
//	/*	-- ccwLimitArm --	*/
//	if (pArmStatus->ccwLimit)	{	sprintf(pRTUDataTx->ccwLimitArm, "%s", "ACC_LIM;");	}
//	else {	sprintf(pRTUDataTx->ccwLimitArm, "%s", "ACC_RUN;");	}
//
//	/*	-- cwLimitPole --	*/
//	if (pPoleStatus->cwLimit)	{	sprintf(pRTUDataTx->cwLimitPole, "%s", "PCW_LIM;");	}
//	else {	sprintf(pRTUDataTx->cwLimitPole, "%s", "PCW_RUN;");	}
//
//	/*	-- ccwLimitPole --	*/
//	if (pPoleStatus->ccwLimit)	{	sprintf(pRTUDataTx->ccwLimitPole, "%s", "PCC_LIM;");	}
//	else {	sprintf(pRTUDataTx->ccwLimitPole, "%s", "PCC_RUN;");	}
//
//	/*	-- limitUp --	*/
//	if (pLiftStatus->upLimit)	{	sprintf(pRTUDataTx->limitUp, "%s", "LUP_LIM;");	}
//	else {	sprintf(pRTUDataTx->limitUp, "%s", "LUP_RUN;");	}
//
//	/*	-- limitDown --	*/
//	if (pLiftStatus->downLimit)	{	sprintf(pRTUDataTx->limitDown, "%s", "LDW_LIM;");	}
//	else {	sprintf(pRTUDataTx->limitDown, "%s", "LDW_RUN;");	}
//
//	/*	-- stallAlm --	*/
//	if (pArmStatus->stalled||pPoleStatus->stalled)	{	sprintf(pRTUDataTx->stallAlm, "%s", "STL_ALM;");	}
//	else {	sprintf(pRTUDataTx->stallAlm, "%s", "STL_RUN;");	}
//
//	/*	-- status --	*/
//	if (iServerStatus) { pRTUDataTx->status = iServerStatus; }
//	else { pRTUDataTx->status = 0x00; }
//
//
//	sprintf_s(pRTUDataTx->buffer, 100, "%d %d %d %d %s %s %s %s %s %s %s %d ",
//	pRTUDataTx->resActArm, pRTUDataTx->velActArm, pRTUDataTx->resActPole, pRTUDataTx->velActPole,
//	pRTUDataTx->cwLimitArm, pRTUDataTx->ccwLimitArm, pRTUDataTx->cwLimitPole, pRTUDataTx->ccwLimitPole,
//	pRTUDataTx->limitUp, pRTUDataTx->limitDown, pRTUDataTx->stallAlm, pRTUDataTx->status);
//
//	return;
//}
/*-----------------------------------------------------------*/
void NetValuesReceivedFromHMI(HMIData_t *HMIData, HMICmd_t *HMICmd)
{	/*	HMI_NETVAR_SIZE + 1: Para tener en cuenta el fin de cadena en caracteres '\0' 	*/
	int x;

	/*	Se asignan en forma directa los valores enteros correspondientes a objetivos para los resolvers	*/
	HMICmd->posCmdArm = HMIData->posCmdArm;
	HMICmd->velCmdArm = HMIData->velCmdArm;
	HMICmd->posCmdPole = HMIData->posCmdPole;
	HMICmd->velCmdPole = HMIData->velCmdPole;


	/*		-- HMICmd Frame Parsing --	*/
	/*	A partir de los descriptores -*HMIData- se obtienen los valores para HMICmd contenidos en la trama recibida.
	*/

	/*	-- mode --	*/
	if(!strncmp(HMIData->mode, "STOP;", HMI_NETVAR_SIZE))	{	HMICmd->mode = eStop;	}
	else if (!strncmp(HMIData->mode, "FRUN;", HMI_NETVAR_SIZE)) {	HMICmd->mode = eFree_run;	}
	else if (!strncmp(HMIData->mode, "AUTO;", HMI_NETVAR_SIZE)) {	HMICmd->mode = eAuto; }
	else if (!strncmp(HMIData->mode, "LIFT;", HMI_NETVAR_SIZE)) { HMICmd->mode = eLift; }
	else { lDebug(Error,"error- HMICmd->mode"); }

	/*	--	freeRunAxis --	*/
	if (!strncmp(HMIData->freeRunAxis, "ARM_;", HMI_NETVAR_SIZE)) { HMICmd->freeRunAxis = eArm; }
	else if (!strncmp(HMIData->freeRunAxis, "POLE;", HMI_NETVAR_SIZE)) { HMICmd->freeRunAxis = ePole; }
	else { lDebug(Error,"error- HMICmd->freeRunAxis"); }

	/*	--	freeRunDir --	*/
	if (!strncmp(HMIData->freeRunDir, "CW__;", HMI_NETVAR_SIZE)) { HMICmd->freeRunDir = eCW; }
	else if (!strncmp(HMIData->freeRunDir, "CCW_;", HMI_NETVAR_SIZE)) { HMICmd->freeRunDir = eCCW; }
	else { lDebug(Error,"error- HMICmd->freeRunDir"); }

	/*	-- ctrlEn --	*/
	if (!strncmp(HMIData->ctrlEn, "CTLE;", HMI_NETVAR_SIZE)) { HMICmd->ctrlEn = eEnable; }
	else if (!strncmp(HMIData->ctrlEn, "DCTL;", HMI_NETVAR_SIZE)) { HMICmd->ctrlEn = eDesable; }
	else { lDebug(Error,"error- HMICmd->ctrlEn"); }

	/*	-- stallEn --	*/
	if (!strncmp(HMIData->stallEn, "STLE;", HMI_NETVAR_SIZE)) { HMICmd->stallEn = eEnable; }
	else if (!strncmp(HMIData->stallEn, "DSTL;", HMI_NETVAR_SIZE)) { HMICmd->stallEn = eDesable; }
	else { lDebug(Error,"error- HMICmd->stallEn"); }

	/*	-- liftDir --	*/
	if (!strncmp(HMIData->liftDir, "LFUP;", HMI_NETVAR_SIZE)) { HMICmd->liftDir = eUp; }
	else if (!strncmp(HMIData->liftDir, "LFDW;", HMI_NETVAR_SIZE)) { HMICmd->liftDir = eDown; }
	else { lDebug(Error,"error- HMICmd->liftDir"); }

	/*	-- clientID --*/
	if (!strncmp(HMIData->clientId, "SM13;", HMI_NETVAR_SIZE)) { HMICmd->clientID = eSigned; }
	else { lDebug(Error,"error- HMICmd->ClientID", HMI_NETVAR_SIZE); HMICmd->clientID = eUnsigned; }

	return;
}

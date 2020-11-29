#ifndef RTU_COM_HMI_H
#define RTU_COM_HMI_H

/* SM13 Remote Terminal Unit -RTU- Prototype TCP Server
  Automatically connects to HMI running on remote client */

  /* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/* LWIP includes */
#include "lwip/err.h"
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

//#include "mot_pap.h"

/**
 * @def Timeout -ms- para intervalo entre ciclos de recepcion y transmision.
 * @brief Espera de nueva trama desde HMI dentro de este intrvalo de tiempo.
 * @note Al generarse timeout, se produce la desconexion por parte de la RTU.
 */
#define	RCV_TIMEO		100

/**
 * @def Puerto de conexion del RTU - socket
 */
#define PORT_NUMBER		5020

void stackIp_ThreadInit(void);
													 
void prvDebugErrorTxRx(err_t err);
					

/*	----------------------------------------------------------------------------------------- */
/*	----------------------------	-	From HMI	-	------------------------------------- */
/*	----------------------------------------------------------------------------------------- */
/**
* @enum	Tamaño de variables de red -NETVAR- recibidos en tramas desde la aplicacion HMI.
* @brief 	Corresponde a datos utilizados en tramas ethernet recibidas desde la aplicacion HMI.
*/
enum { HMI_NETVAR_SIZE = 5 };

/**
 * @struct 	HMIData
 * @brief	Descriptor del bufer de recepcion.
																  
 */
typedef struct HMIDATA
{
	uint16_t posCmdArm;
	uint16_t posCmdPole;
	uint8_t velCmdArm;
	uint8_t velCmdPole;
	unsigned char mode[HMI_NETVAR_SIZE];		/*	-- mode --			STOP; FRUN; AUTO; LIFT; */
	unsigned char freeRunAxis[HMI_NETVAR_SIZE];	/*	-- freeRunAxis	--	POLE; ARM_;				*/
	unsigned char freeRunDir[HMI_NETVAR_SIZE];	/*	-- freeRunDir  --	CW__; CCW_;				*/
	unsigned char ctrlEn[HMI_NETVAR_SIZE];		/*	-- ctrlEn --		CTLE; DCTL;				*/
	unsigned char stallEn[HMI_NETVAR_SIZE];		/*	-- stallEn --		STLE; DSTL;				*/
	unsigned char liftDir[HMI_NETVAR_SIZE];		/*	-- LiftDir --		LFUP; LFDW;				*/
	unsigned char clientId[HMI_NETVAR_SIZE];	/*	-- clientId --		SM13; 					*/

} HMIData_t;

/**
 * @enum HMICmd
 * @brief Definicion enumerada de los modos de funcionamiento comandados por el HMI.
 */
typedef enum
{
	eStop = 0,
	eFree_run = 1,
	eAuto = 2,
	eLift = 3,

}mode_t;
typedef enum { eArm, ePole } freeRunAxis_t;
typedef enum { eCW, eCCW } freeRunDir_t;
typedef enum { eDesable, eEnable } enable_t;
typedef enum { eDown, eUp } liftDir_t;
typedef enum { eUnsigned, eSigned } sign_t;

/**
 * @struct 	HMICmd
 * @brief	Comandos HMI. Obtenidos y traducidos desde HMIData
 */		  
typedef struct
{
	uint16_t posCmdArm;
	uint16_t posCmdPole;
	uint8_t velCmdArm;
	uint8_t velCmdPole;
	mode_t mode;		
	freeRunAxis_t freeRunAxis;
	freeRunDir_t freeRunDir;
	enable_t ctrlEn;
	enable_t stallEn;
	liftDir_t liftDir;
	sign_t clientID;

} HMICmd_t;

void NetValuesReceivedFromHMI(HMIData_t *HMIData, HMICmd_t *HMICmd);

/*	----------------------------------------------------------------------------------------- */
/*	----------------------------	-	From RTU	-	------------------------------------- */
/*	----------------------------------------------------------------------------------------- */
/**
* @enum	Tamaño de variables de red -NETVAR- enviados en tramas desde la RTU.
* @brief 	Corresponde a datos utilizados en tramas ethernet enviadas hacia la aplicacion HMI.
*/
enum { RTU_NETVAR_SIZE = 9 };

/**
 * @struct 	RTUData
 * @brief	Estado actual del RTU y sus dispositivos asociados. Datos enviados en
 * 			tramas ethernet hacia aplicacion HMI.
 */
 typedef struct
{
	uint16_t resActArm;
	uint16_t resActPole;
	uint16_t velActArm;
	uint16_t velActPole;
	char cwLimitArm[RTU_NETVAR_SIZE];
	char ccwLimitArm[RTU_NETVAR_SIZE];
	char cwLimitPole[RTU_NETVAR_SIZE];
	char ccwLimitPole[RTU_NETVAR_SIZE];
	char limitUp[RTU_NETVAR_SIZE];
	char limitDown[RTU_NETVAR_SIZE];
	char stallAlm[RTU_NETVAR_SIZE];
	uint8_t status;
	char buffer[100];

} RTUData_t;

enum mot_pap_direction {
	MOT_PAP_DIRECTION_CW = 0, MOT_PAP_DIRECTION_CCW = 1,
};
typedef struct mot_pap_msg {
	enum {
		MOT_PAP_MSG_TYPE_FREE_RUNNING,
		MOT_PAP_MSG_TYPE_CLOSED_LOOP,
		MOT_PAP_MSG_TYPE_STOP
	} type;
	enum mot_pap_direction free_run_direction;
	uint32_t free_run_speed;
	uint32_t closed_loop_setpoint;
}mpap_t;
typedef struct mot_pap_status {
	enum {
		MOT_PAP_STATUS_CW = 0, MOT_PAP_STATUS_CCW = 1, MOT_PAP_STATUS_STOP
	} dir;
	int32_t posCmd;
	int32_t posAct;
	uint32_t vel;
	volatile bool cwLimit;
	volatile bool ccwLimit;
	volatile bool stalled;
}mpapstatus_t;

QueueHandle_t pole_queue;

QueueHandle_t arm_queue;


/*	----------------------------------------------------------------------------------------- */
/*	----------------------------	-	LIFT	-	----------------------------------------- */
/*	----------------------------------------------------------------------------------------- */

/*	--	lift	--	*/
typedef enum {LIFT_TYPE_UP = 0, LIFT_TYPE_DOWN = 1, LIFT_TYPE_STOP} eLift_t;

typedef struct
{
	eLift_t type;
}lift_t;

typedef struct lift_status {
	eLift_t type;
	volatile bool upLimit;
	volatile bool downLimit;
}liftstatus_t;

/*	-- Declaracion de funciones compartidas --	*/
mpapstatus_t pole_get_status(void);	/* Funciones que utilizamos para obtener los valores de las estructuras mot_pap_status */
mpapstatus_t arm_get_status(void);
liftstatus_t lift_get_status(void);

void NetValuesToSendFromRTU(int16_t iServerStatus,RTUData_t* pRTUDataTx, mpapstatus_t* pArmStatus, mpapstatus_t* pPoleStatus, liftstatus_t* pLiftStatus);

/*	-- Declaraci�n de objetos --	*/
QueueHandle_t lift_queue;

///*	-- Declaraci�n de funciones RTUcomHMI --	*/
///*
//	Asigna valores a RTUDATA seg�n el contenido de RTUVAL.
//	Esta conversi�n de valores se realiza con el fin de
//	transmitir valores "Human readable" por ethernet.
//	De esta forma se simplifica el debbug, mediante captura
//	de tramas ethernet.
//*/
//
///*
//	-- DataRxTx --
//	Intercambio de tramas con el software HMI.
// */
//static void prvDataRxTxTask(void* pvParameters);
///*
//	-- Desentramado HMIDataRx --
//	Se trabaja la trama recibida desde HMI para conformar los datos y comandos HMI.
//
///*	-- Conexi�n del servidor --
//		Queda a la espera por solicitud de conexi�n del Software HMI en el puerto 5020.
// */
//static void prvServerConnectTask(void* pvParameters);
///**/
////static int16_t prvStatusHandlerRecv(HMICmd_t* pHMICmd, int16_t iServerStatus, Socket_t xConnectedSocket, uint16_t usLenHMIDataRx, uint16_t iRecv);
///**/
//static int16_t prvStatusHandlerSend(int16_t iServerStatus, int lSent, Socket_t xConnectedSocket);
///**/
//void TaskTriggerMsg(HMICmd_t* pHMICmd, int16_t iServerStatus);
///*	LECCION: TODOS LOS ARGUMENTOS QUE SE INCLUYEN EN LAS FUNCIONES DEBEN NECESARIAMENTE COMPARTIR EL MISMO ARCHIVO .H EN DONDE
//	SE DECLARA LA FUNCI�N. POR EJEMPLO, LA FUNCI�N -prvTrip- SE DECLARA EN ESTE ARCHIVO -RTUcomHMI.h- POR LO QUE LA ESTRUCTURA
//	DE TIPO mpap_t, NO PUEDE DECLARARSE EN EL ARCHIVO -MOT_PAP.h-. POR ESO MOV� LA DECLARACI�N A ESTE ARCHIVO.
//*/
/*-----------------------------------------------------------*/
#endif

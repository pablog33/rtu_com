#ifndef __RTU_COM_HMI_H__
#define __RTU_COM_HMI_H__

#include <stdint.h>
#include <stdbool.h>
#include "lwip/err.h"

#define	RCV_TIMEO		100 	/* 	Intrvalo de espera -100ms- entre ciclos de recepcion y transmision
								 	De no recibir paquetes del HMI pasado este intervalo, se produce
								 	desconexion de la RTU.
								 */

void stackIp_ThreadInit(void);

void prvDebugErrorTxRx(err_t err);

/**
 * @enum	Tamaño de variables de red -NETVAR-
 * @bref 	Corresponde a datos utilizados en tramas ethernet intercambiadas con HMI
 * 			la aplicacion HMI.
 */
enum { HMI_NETVAR_SIZE = 5 };
enum { RTU_NETVAR_SIZE = 9 };
enum { RTUDATA_BUFFER_SIZE = 100 };
enum { SERVERTASK_STACK_SIZE = 100 };

/**
 * @struct 	HMIData
 * @brief	Descriptor del bufer de recepción.
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

/*		--	HMICMD	--
	 Estructura que almacena la recepción de datos en la trama que envía
	el software HMI de acuerdo a los comandos efectuados por el operador. */
typedef enum	/*	Definición enumerada de los modos de funcionamiento comandados por el HMI.	*/
{
	eStop = 0,
	eFree_run = 1,
	eAuto = 2,
	eLift = 3,

}mode_t;
/**
 * @enum 	HMICmd
 */
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

/* ---------------------	-	FIN / FROM HMI	-	----------------- */




typedef struct
{
	uint16_t pos;
	unsigned char cmd[5];
	unsigned char buffer[8];
}
RTUData_t;

#endif /* __RTU_COM_HMI_H__ */

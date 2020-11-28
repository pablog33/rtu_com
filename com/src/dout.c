
#include <stdbool.h>

#include "dout.h"
#include "board.h"

/**
 * @brief 	initializes DOUTs
 * @return	nothing
 */
void dout_init()
{
	Chip_SCU_PinMuxSet( 4, 8, SCU_MODE_FUNC4 );			//DOUT4 P4_8 	PIN15  	GPIO5[12]   ARM_DIR
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 12);

	Chip_SCU_PinMuxSet( 4, 9, SCU_MODE_FUNC4 );			//DOUT5 P4_9  	PIN33	GPIO5[13] 	ARM_PULSE
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 13);

	Chip_SCU_PinMuxSet( 4, 10, SCU_MODE_FUNC4 );		//DOUT6 P4_10 	PIN35	GPIO5[14] 	POLE_DIR
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 5, 14);

	Chip_SCU_PinMuxSet( 1, 5, SCU_MODE_FUNC0 );			//DOUT7 P1_5 	PIN48 	GPIO1[8]   	POLE_PULSE
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 1, 8);

	dout_init_cero();

	return;

}

void dout_init_cero()
{
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 12);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 12);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 12);
	Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT, 5, 12);

	return;
}


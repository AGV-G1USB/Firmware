/* ###################################################################
**     Filename    : ProcessorExpert.c
**     Project     : ProcessorExpert
**     Processor   : MCF51QE128CLK
**     Version     : Driver 01.00
**     Compiler    : CodeWarrior ColdFireV1 C Compiler
**     Date/Time   : 2018-05-02, 15:35, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file ProcessorExpert.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/         
/*!
**  @addtogroup ProcessorExpert_module ProcessorExpert module documentation
**  @{
*/         
/* MODULE ProcessorExpert */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "TI1.h"
#include "AS1.h"
#include "AS2.h"
#include "PWM1.h"
#include "PWM2.h"
#include "Bit1.h"
#include "Bit2.h"
#include "AD1.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
extern unsigned char dato = 0;

unsigned char cam[10];
unsigned short *tam;

void main(void)
{
  /* Write your local variable definition here */

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/

  /* Write your code here */
  /* For example: for(;;) { } */

  for(;;){
  	  
  	  switch(dato){
  	  	  
  	  	  case esperar:
  	  		  
  	  		  break;
  	  		  
  	  	  case pedir:

  	  		  AS2_SendChar('G');
  	  		  AS2_SendChar('M');
  	  		  AS2_SendChar(92);
  	  		  AS2_SendChar('r');

  	  		  dato = recibir;
  	  		  break;
  	  		  
  	  	  case recibir:
  	  		  
  	  		  AS2_RecvBlock(cam,11,tam);
  	  		  AS1_SendBlock(cam,11,tam);
  	  		  
  	  		  dato = accion;
  	  		  break;
  	  		  
  	  	  case accion:
  	  		  
  	  		  PWM1_Enable();
  	  		  PWM2_Enable();
  	  		  dato = esperar;
  	  		  break;
  	  		  
  	  	  default:
  	  		  break;
  	  		
  	  }
  	  
    }
  
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END ProcessorExpert */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.3 [05.08]
**     for the Freescale ColdFireV1 series of microcontrollers.
**
** ###################################################################
*/

/* ###################################################################
**     Filename    : ProcessorExpert.c
**     Project     : ProcessorExpert
**     Processor   : MCF51QE128CLK
**     Version     : Driver 01.00
**     Compiler    : CodeWarrior ColdFireV1 C Compiler
**     Date/Time   : 2018-05-16, 13:00, # CodeGen: 0
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
#include "AS1.h"
#include "AS2.h"
#include "AD1.h"
#include "PWM1.h"
#include "PWM2.h"
#include "Bit1.h"
#include "Bit2.h"
#include "Bit3.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "Math.h"
#include "String.h"

/* User includes (#include below this line is not maintained by Processor Expert) */

// Maquina de estados.
#define sharp 0
#define leer 1
#define accion 2
#define parar 3
unsigned int estado=0;

// Comandos cámara.
unsigned char RS[4] = {13, 'R', 'S', 13};       // Resetear Cámara.
unsigned char L1[5] = {'L', '1', ' ', '1', 13}; // Encender Led.
unsigned char L0[5] = {'L', '1', ' ', '0', 13}; // Apagar Led.
unsigned char GM[3] = {'G', 'M', 13};           // Recibir Media de color.
unsigned char TC[22] = {'T', 'C', ' ', '5', '0', ' ', '1', '5', '0', ' ', '1', '0', ' ', '2', '0', ' ', '1', '0', ' ', '2', '0', 13}; // Recibir Centroide.
unsigned char TW[3] = {'T', 'W', 13};           // Track Window.
unsigned char barrar[1] = {13};

// Información para el bluetooth.
unsigned char left[5] = {'L', 'e', 'f', 't', 13};
unsigned char right[6] = {'R', 'i', 'g', 'h', 't', 13};
unsigned char lLeft[6] = {'L', 'L', 'e', 'f', 't', 13};
unsigned char lRight[7] = {'L', 'R', 'i', 'g', 'h', 't', 13};
unsigned char reverse[8] = {'R', 'e', 'v', 'e', 'r', 's', 'e', 13};
unsigned char start[6] = {'S', 't', 'a', 'r', 't', 13};
unsigned char stop[5] = {'S', 't', 'o', 'p', 13};
unsigned char sharpA[7] = {'S', 'h', 'a', 'r', 'p', 'A', 13};
unsigned char band;

// PRUEBAS
unsigned char hola[5] = {'H', 'o', 'l', 'a', 13};

// Dato de la cámara.
unsigned char data[26], data1[15];

// Constantes.
int h = 0, r = 0, d = 0, x = 0, y = 0;
int u, j;

// Para Centroide.
unsigned char MX[2], BIT1[1],BIT2[1], PWM1[3],PWM2[3];           
int mx , pwm1, pwm2, bit1, bit2;

// Sharp y Motores.
unsigned short ADC1;
unsigned int distancia;
unsigned int A;

void sendComand(unsigned char comand[], int length, int flag){ // Para enviar arreglos por las salidas serial.
	unsigned int i;
	
	if(flag == 1){  // Para enviar arreglo al serial de la cámara.
		for(i=0;i<length;i++){
			AS2_SendChar(comand[i]);
		}
	}else{          // Para enviar arreglo al serial del bluetooth.
		for(i=0;i<length;i++){
			if(comand[i] != 00){
				AS1_SendChar(comand[i]);
			}
		}
	}
}

int convertAsciiDec(unsigned char value[], int length){ // Para convertir arreglos de Ascii en enteros decimal.
	
	int i = 0, v = 0, a = 0;
	
	for(i=0;i<length;i++){  // Para recorrer el arreglo.
		if(value[i] != 00){
			v += (value[i]-48)*pow(10,length-i-1); // Ecuación de conversión Ascii-Decimal.
			a++;
		}
	}
	if(a == 2 && length == 3){ // Si el tamaño del arreglo recibido es mayor a 2.
		v = v - 100;
	}
	
	return v;  
}

void main(void)
{
	  /* Write your local variable definition here */

	  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
	  PE_low_level_init();
	  /*** End of Processor Expert internal initialization.                    ***/

	  /* Write your code here */
	  /* For example: for(;;) { } */
	  
	  for(;;){
		  
		  switch(estado){
		 
		  	  case sharp:

		  		  break;
		 		 
		  	  case leer:
			  		  h = 0;
			  		  
			  		  if( data1[h] == 'F' && data1[h+1] == ' ' ){
			  			  h = 2;
			  		  } else if ( data1[h] == 'F' && data1[h+1] == 'F' ){
			  			  h = 3; 
			  		  }
			  			  
			  		  while(data1[h] != ' ' && r < 3){   // Guardamos en un arreglo el valor del PWM1.
			  			  PWM1[r] = data1[h];
			  			  r++;
			  			  h++;
			  		  }
			  		  	  h++;
			  		  while(data1[h] != ' ' && d < 1){   // Guardamos en un arreglo la dirección del PWM1.
			  			  BIT1[d] = data1[h];
			  			  d++;
			  			  h++;
			  		  }
			  		  h++;
			  		  while(data1[h] != ' ' && x < 3){   // Guardamos en un arreglo el valor del PWM2.
			  			  PWM2[x] = data1[h];
			  			  x++;
			  			  h++;
			  		  }
			  		  h++;
			  		  while(data1[h] != ' ' && y < 1){   // Guardamos en un arreglo la dirección del PWM2.
			  			  BIT2[y] = data1[h];
			  			  y++;
			  			  h++;
			  		  }
			  		  
			  		  // Se convierten a decimal los arreglos obtenidos.
			  		  pwm1 = convertAsciiDec(PWM1, r);
			  		  bit1 = convertAsciiDec(BIT1, d);
			  		  pwm2 = convertAsciiDec(PWM2, x);
			  		  bit2 = convertAsciiDec(BIT2, y);
			  		  r = 0;
			  		  d = 0;
			  		  x = 0;
			  		  y = 0;
			  		  
			  		  estado = accion;

		  		  break; 
		 		 
		  	  case accion:
		  		  
		  		  // Se asignan los valores recibidos a los PWM.
		  		  PWM1_Enable();
		  		  PWM2_Enable();
		  		  Bit1_PutVal(bit1); 
		  		  Bit2_PutVal(bit2);
		  		  PWM2_SetDutyUS(pwm2);
		  		  PWM1_SetDutyUS(pwm1);
		  		  
		  		  estado = sharp;
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

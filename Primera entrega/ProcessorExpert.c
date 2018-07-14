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

// PRUEBAS
unsigned char hola[5] = {'H', 'o', 'l', 'a', 13};

// Dato de la cámara.
unsigned char data[26];

// Constantes.
int h = 0, r = 0;
int u, j;

// Para Centroide.
unsigned char MX[2];           
int mx;

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

void movSharp(int d){

	Bit1_PutVal(1);
	Bit2_PutVal(1);
	PWM1_Enable();
	PWM2_Enable();
	
	if(d<50){
		
		if(d>9 && d<20){
			sendComand(stop, sizeof(stop), 0);
						
			PWM1_Disable();
			PWM2_Disable();
		}
		else{
			if(d<=9){
				
				sendComand(reverse, sizeof(reverse), 0);
				
				Bit1_PutVal(1);
				Bit2_PutVal(1);
				PWM1_SetDutyUS(300);
				PWM2_SetDutyUS(500);				
			}
			else{
				
				sendComand(start, sizeof(start), 0);
				
				Bit1_PutVal(0);
				Bit2_PutVal(0);
				PWM1_SetDutyUS(650);
				PWM2_SetDutyUS(600);
			}
		}
	}
	else{
		
		PWM1_Disable();
		PWM2_Disable();
	}
	
}

void lookingMove(int looking){
	
	if(looking == 0){

		sendComand(lLeft, sizeof(lLeft), 0);
		
		Bit2_PutVal(1);
		PWM2_SetDutyUS(600);
		
	}else if(looking == 1){

		sendComand(lRight, sizeof(lRight), 0);
		
		Bit2_PutVal(1);
		PWM2_SetDutyUS(400);
		
	}
	
}

void control(int mx, int dist){
	
	int looking = 0, mxRef = 40, kp = 9, error = 0, front = 0; 
	int error_ANT;
	int PWM_MAX=750;
	int PWM_MIN=250;
	int PWM_init = 500, PWM = 0;
	
	error = kp*(mxRef-mx);
	PWM = PWM_init + error;
	if(front == 1){
		movSharp(dist);
		front=0;
		return;
	}
	if(mx != 0 && front == 0){
		if(error<-5 || error>5){
			if(PWM<PWM_MIN){
				PWM = PWM_MIN;
			}
			if(PWM>PWM_MAX){
				PWM = PWM_MAX;
			}
			error_ANT = error;
			Bit2_PutVal(0);	
			Bit1_PutVal(0);
			PWM1_SetDutyUS(550);
			PWM2_SetDutyUS(PWM);
			return;
		}else{
			front=1;
			return;
		}
	}else{
		if(error_ANT < 0){
			looking = 0;
		}else{
			looking = 1;
		}
		lookingMove(looking);
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
	  
	  Cpu_Delay100US(20000); 
	  
	  sendComand(RS, sizeof(RS), 1); 
	  
	  Cpu_Delay100US(2000);
	  
	  sendComand(TW, sizeof(TW), 1);
	  
	  estado = sharp;
	  
	  for(;;){
		  
		  switch(estado){
		  
		  	  case sharp:
		  		  
		  		  AD1_MeasureChan(TRUE, 0);
		  		  AD1_GetChanValue16(0, &ADC1);    // Se obtiene el valor ADC del Sharp.
		  		  ADC1 = ADC1 >> 4;                // Se shiftea 4 veces hacia la derecha para obtener el valor real.

		  		  distancia = 49.686*exp(-0.0009*ADC1);  // Ecuación lineal de caracterización del sharp.
		  		
		  		  break;
		  		
		  	  case leer:
		  		  
		  		  h = 3;
		  		  
  				  while(data[h] != ' ' && r < 2){   // Guardamos en un arreglo el valor del centroide en X.
		  			  MX[r] = data[h];
		  			  r++;
		  			  h++;
  				  }
		  		  
		  		  estado = accion;
		  		  
		  		  break;
		  		  
		  	  case accion:
		  		  
		  		  mx = convertAsciiDec(MX, r);
		  		  
				  sendComand(MX, r, 0);        // Enviamos el valor tomado como centroide por bluetooth para verificar que funciona correctamente.
				  sendComand(barrar, 1, 0);
				  r = 0;
				  control(mx, distancia);
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

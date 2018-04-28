/*
 * File:   main.c
 * Author: Pablo Salgado
 *
 * Created on April 5, 2018, 4:20 AM
 */

#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 20000000      // Fosc  frequency for _delay()  library

/* Se define usar los 8 pines del puerto B para el teclado*/
#include "keypad.h"
/* 
 * Se define usar los 8 pines del puerto D para los datos del display y los pines
 * RC0, RC1 y RC2 para el control del mismo. De hecho el puerto C se ha configurado
 * de salida totalmente.
 */
#include "lcd.h"
#include "delay.h"

/* Arreglo de n�mero de c�dula de los integrantes del grupo*/
char ccs[5][8] = {
    {'7', '9', '5', '3', '5', '9', '3', '9'},
    {'5', '2', '2', '6', '8', '8', '6', '4'},
    {'7', '9', '5', '3', '5', '9', '3', '0'},
    {'7', '9', '5', '3', '5', '9', '3', '1'},
    {'7', '9', '5', '3', '5', '9', '3', '2'}
};

/* Arreglo de nombres de los integrantes del grupo*/
char *nombres [5] = {
    "Pablo",
    "Luisa",
    "Pablo0",
    "Pablo1",
    "Pablo2"
};

/* Arreglo que mantiene el n�mero de c�dula le�do*/
char cc[8] = {'0', '0', '0', '0', '0', '0', '0', '0'};

/* Program to demonstrate the hex-Keypad interface*/
int  main()
{
    /* 
     * El puerto C est� conectado a los LED y los pines de control del display. 
     * Inicialmente todos los LED deben estar apagados, de modo que simplemente
     * colocamos a 0 el puerto C al iniciar el programa.
     */
    PORTC = 0x00;
    
    /* Llamar rutinas de inicio del teclado y el display*/
    LCD_Init(8,2,16);
    KEYPAD_Init();
        
    /* Desplegar nombre del curso y grupo*/
    LCD_DisplayString("Micros grupo 26");
    __delay_ms(2000);
    
    /* Ciclo infinito para leer el n�mero de c�dula y buscar*/
	while(1)
	{
        LCD_Clear();
        LCD_DisplayString("C�dula:");
        LCD_GoToNextLine();

        leer_cedula();

        __delay_ms(2000);
        LCD_Clear();
        LCD_DisplayString("Buscando ...");
        LCD_GoToNextLine();
        
        buscar_cedula();
        
        __delay_ms(2000);
	}

	return 0;
}

/* Lee 8 caracteres desde el teclado y los almacena en el arreglo cc*/
int leer_cedula() {
    uint8_t i = 0;
    
    /* Leer exactamente 8 caracteres desde el teclado*/
    while(i < 8) {
        uint8_t key;    
        
        /* Esperar por la pulsaci�n de una tecla. Se obtiene el c�digo ASCII
         * correspondiente al caracter digitado*/
		key = KEYPAD_GetKey();
        
        /* 
         * Si se ha pulsado la primera tecla se borrar el display para comenzar
         * a desplegar el n�mero de c�dula.
         */
        if (0 == i) LCD_Clear();
        
        /* Despliega el caracter correspondiente a la tecla pulsada*/
		LCD_DisplayChar(key);
        
        /* Se guarda el c�digo ASCII correspondiente a la tecla pulsada*/
        cc[i] = key;
        
        /* Siguiente tecla ....*/
        i++;
    }
}

/* 
 * Busca en el arreglo de n�meros de c�dula si existe el n�mero de c�dula digitado,
 * escribe el nombre de la persona en la segunda fila del display y activa el
 * LED correspondiente a cada persona.
 */
int buscar_cedula() {
    uint8_t i, j, k = 0;
    
    /* Se recorre el arreglo de n�meros de c�dula por filas (variable i). */
    for (i = 0; i < 5; i++) {
        
        /* 
         * Se cuenta en la variable k, cada car�cter coincidente encontrado en 
         * la fila actual de  n�meros de c�dula con el n�mero de c�dula digitado
         */
        k = 0;
        
        /* Se recorren las columnas de los arreglos ccs y cc (variable j*/
        for (j = 0; j < 8; j++) {
            /* 
             * Si los valores de las columnas son iguales, se cuenta una 
             * coincidencia
             */
            if(ccs[i][j] == cc[j]) k++;
        }
        
        /* 
         * Si se tienen 8 coincidencias, quiere decir que el n�mero de c�dula 
         * digitado se encuentra en el arreglo ccs, de modo que se interrumpe la
         * ejecuci�n del ciclo, de este modo la variable i indica la fila en la
         * que se encontr� el n�mero de c�dula.
         */
        if (8 == k) break;
    }
    
    if(8 == k) {
        /* 
         * Si se encontr� un n�mero de c�dula, la variable i indica en que fila 
         * del arreglo de nombres est� el nombre de la persona. Se despliega el
         * nombre.
         */
        LCD_DisplayString(nombres[i]);
        
        /* 
         * Se enciende el LED correspondiente a la persona. Los pines a usar son 
         * los C<3:7>, de modo que se deben conservar los valores de los tres
         * bits menos significativos del puerto C y se apagan todos los LED
         */
        uint8_t portc = PORTC & 0x07;
        
        /* 
         * Conservando los 3 bits menos significativos del puerto C, se activa 
         * solo el bit correspondiente a la fila en la que se encontr� la 
         * persona. Por ejemplo si la persona se encontr� en la fila 3 se tiene:
         * 
         * portc = 0000 0xxx
         * 
         * En el caso 3 se hace la operaci�n:
         * 
         * portc | 0x47 -> 0000 0xxx | 0100 0000 = 0100 0xxx
         * 
         * La cual enciende el LED del bit RC6 y mantiene los bits RC0, RC1 y RC2
         * en sus valores actuales
         */
        switch(i) {
            case 0: PORTC = portc | 0x08; break;
            case 1: PORTC = portc | 0x10; break;
            case 2: PORTC = portc | 0x20; break;
            case 3: PORTC = portc | 0x40; break;
            case 4: PORTC = portc | 0x80; break;
        }        
    } else {
        /* 
         * En cualquier otro caso se informa que no se ha encontrado el n�mero 
         * de c�dula
         */
        LCD_DisplayString("No se encontro.");
    }
}
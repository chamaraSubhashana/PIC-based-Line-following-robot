#include <xc.h>

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

#define _XTAL_FREQ 20000000

#define RIGHT 0
#define LEFT 1

volatile unsigned int temp1 = 0;
volatile unsigned int temp2 = 0;
volatile int turn = -1;

void __interrupt() ISR(){
    
    if(PIR1bits.TMR1IF && PIE1bits.TMR1IE){
        temp1++;
        if(temp1 == 5){
            CCPR1L = 90;
            __delay_ms(70);
            CCPR1L = 45;
            temp1 = 0;
        }
        PIR1bits.TMR1IF = 0;
    }

    if (PORTDbits.RD2==1 && PORTDbits.RD3==1){
        temp2++;
        if(temp2 == 9){
            if(turn == RIGHT){
                PORTCbits.RC4=1; PORTCbits.RC5=1; //Motor 1 stop
                PORTCbits.RC6=1; PORTCbits.RC7=0; //Motor 2 forward
                CCPR1L = 90;
                __delay_ms(70);
            }if(turn == LEFT){
                PORTCbits.RC4=0; PORTCbits.RC5=1; //Motor 1 forward
                PORTCbits.RC6=1; PORTCbits.RC7=1; //Motor 2 stop
                 CCPR1L = 90;
                __delay_ms(70);
            }
            temp2 = 0;
        }
        
    }
    
    
}

void main()
{
     TRISDbits.TRISD2 = 1; TRISDbits.TRISD3 = 1; //Bath the IR sensor pins are declared as input
     TRISCbits.TRISC4 = 0; TRISCbits.TRISC5 = 0; //Motor 1 pins declared as output
     TRISCbits.TRISC6 = 0; TRISCbits.TRISC7 = 0; //Motor 2 pins declared as output
     TRISCbits.TRISC2 = 0; //pwm
     
    //timer1-interrupt
    INTCONbits.PEIE= 1;
    INTCONbits.GIE = 1;
    PIE1bits.TMR1IE = 1;
    PIR1bits.TMR1IF = 0;
    //timer1
    T1CONbits.T1CKPS1 = 1;
    T1CONbits.T1CKPS0 = 1;
    T1CONbits.TMR1CS = 0;
    T1CONbits.TMR1ON = 1;
     
    //timer2
    PR2 = 155;
    CCPR1L = 45;
    T2CONbits.TOUTPS3 = 0;
    T2CONbits.TOUTPS2 = 0;
    T2CONbits.TOUTPS1 = 0;
    T2CONbits.TOUTPS0 = 0;
    T2CONbits.T2CKPS0 = 1;
    T2CONbits.T2CKPS1 = 1;
    T2CONbits.TMR2ON = 1;
    
    //cpp1
    CCP1CONbits.CCP1M = 0xF;
   
     while(1)
         {
         
         if (PORTDbits.RD2 == 0 && PORTDbits.RD3 == 0) //Both sensor not over balck line
         {
         PORTCbits.RC4=0; PORTCbits.RC5=1; //Motor 1 forward
         PORTCbits.RC6=1; PORTCbits.RC7=0; //Motor 2 forward
         turn = -1;
         }
         
         else if (PORTDbits.RD2==1 && PORTDbits.RD3==0) //Left sensor is over black line
         {
         PORTCbits.RC4=1; PORTCbits.RC5=1; //Motor 1 stop
         PORTCbits.RC6=1; PORTCbits.RC7=0; //Motor 2 forward
         turn = RIGHT;
         } 
         
         else if (PORTDbits.RD2==0 && PORTDbits.RD3==1) //Right sensor is over black line
         {
         PORTCbits.RC4=0; PORTCbits.RC5=1; //Motor 1 forward
         PORTCbits.RC6=1; PORTCbits.RC7=1; //Motor 2 stop
         turn = LEFT;
         } 
         
         else //Both Sensor over black line
         {
         PORTCbits.RC4=1; PORTCbits.RC5=1; //Motor 1 stop
         PORTCbits.RC6=1; PORTCbits.RC7=1; //Motor 2 stop 
         
         }
         
         }
}

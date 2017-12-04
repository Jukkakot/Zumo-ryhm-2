/**
* @mainpage ZumoBot Project
* @brief    You can make your own ZumoBot with various sensors.
* @details  <br><br>
    <p>
    <B>General</B><br>
    You will use Pololu Zumo Shields for your robot project with CY8CKIT-059(PSoC 5LP) from Cypress semiconductor.This 
    library has basic methods of various sensors and communications so that you can make what you want with them. <br> 
    <br><br>
    </p>
    
    <p>
    <B>Sensors</B><br>
    &nbsp;Included: <br>
        &nbsp;&nbsp;&nbsp;&nbsp;LSM303D: Accelerometer & Magnetometer<br>
        &nbsp;&nbsp;&nbsp;&nbsp;L3GD20H: Gyroscope<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Reflectance sensor<br>
        &nbsp;&nbsp;&nbsp;&nbsp;Motors
    &nbsp;Wii nunchuck<br>
    &nbsp;TSOP-2236: IR Receiver<br>
    &nbsp;HC-SR04: Ultrasonic sensor<br>
    &nbsp;APDS-9301: Ambient light sensor<br>
    &nbsp;IR LED <br><br><br>
    </p>
    
    <p>
    <B>Communication</B><br>
    I2C, UART, Serial<br>
    </p>
*/

#include <project.h>
#include <stdio.h>
#include <stdlib.h>
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "I2C_made.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "IR.h"
#include "Ambient.h"
#include "Beep.h"



int rread(void);
void blinking_led ();
void check_battery ();
void move();
int threshold_calculator (int black, int white);
    int left1 = 1;
    int left3 = 1;
    int right1 = 1;
    int right3 = 1;

    int far_left_white;
    int left_white;
    int right_white;
    int far_right_white;

    int previous_right3 = 0;
    int previous_left3 = 0;

    int blacklines = 0;

    int is_on_blackline = 0;

void move_smooth();
void move_to_blackline ();

/**
 * @file    main.c
 * @brief   
 * @details  ** You should enable global interrupt for operating properly. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/

void check_battery () {
    int16 adcresult =0;
    float volts = 0.0;

        ADC_Battery_StartConvert();
        if(ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT)) {   // wait for get ADC converted value
            adcresult = ADC_Battery_GetResult16();
            volts = 1.5 * ADC_Battery_CountsTo_Volts(adcresult);                  // convert value to Volts
        
            // If you want to print value
            printf("%d %f\r\n",adcresult, volts);
             
        }
        if (volts < 4.0) {
            printf("Battery low!\n");
            blinking_led();  
        }
}
void blinking_led () {
    printf("Blinking\n");
       while(1) {
       BatteryLed_Write(1); // Switch led on 
       CyDelay(50);
       BatteryLed_Write(0); // Switch led off
       CyDelay(500);
    }
    
}
int main()
{
    struct sensors_ ref;
    ADC_Battery_Start();
    CyGlobalIntEnable; 
    UART_1_Start();
  
    sensor_isr_StartEx(sensor_isr_handler);
    
    reflectance_start();
    reflectance_set_threshold(14700, 14750, 14600, 15950);

    IR_led_Write(1);
    
    CyDelay(10);
   
    BatteryLed_Write(1); // Switch led on
    check_battery();
    
    while (SW1_Read() == 1); //Start calibrate 
    
    reflectance_read(&ref);
    
    far_left_white = ref.l3;
    left_white = ref.l1;
    right_white = ref.r1;
    far_right_white = ref.r3;

       BatteryLed_Write(1); // Switch led on 
       CyDelay(50);
       BatteryLed_Write(0); // Switch led off
       CyDelay(500);
       BatteryLed_Write(1); // Switch led on 
       CyDelay(50);
       BatteryLed_Write(0); // Switch led off
    
    while (SW1_Read() == 1); //Move to blackline
    BatteryLed_Write(1);    // Switch led on 
    motor_start();
    
    while(is_on_blackline == 0) {
        motor_forward(100,5);
        reflectance_read(&ref);
       if (ref.l3 > 23500 && ref.r3 > 23500) {
         is_on_blackline = 1; 
        }
       }
    motor_stop();
    BatteryLed_Write(0);    // Switch led off
    CyDelay(500);
    
    while (SW1_Read() == 1); //Start the race
    left1 = 0, right1 = 0;
    BatteryLed_Write(1);    // Switch led on 
    motor_start();
    while(blacklines<=3){
        move_smooth();
        //printf("%d", blacklines);
    } 
    motor_stop();
    return 0;
}

void move_smooth(){
    struct sensors_ ref;
    struct sensors_ dig;
    reflectance_read(&ref);
    reflectance_digital(&dig);
    
    if (left3 == 1 && right3 == 1 && (dig.l1 ==  0 && dig.r1 == 0 && dig.r3 == 0 && dig.l3 == 0) ){
            blacklines +=1;
    }
    /*if (ref.r3 > 10000 && ref.l3 > 10000 && previous_left3 < 10000 && previous_right3 < 10000 && ref.r1>10000 && ref.l1>10000) {
        blacklines++;
    }
    if(blacklines >= 3 && ref.r3 > 10000 && ref.l3 > 10000  && ref.r1>10000 && ref.l1>10000) {
        motor_forward(0,0);
        motor_stop();
    }*/
    
    int right = (int)((((double)ref.r1 - (double)right_white)/(23999.0 - (double)right_white))*255.0);
    int left = (int)((((double)ref.l1 - (double)left_white)/(23999.0 - (double)left_white))*255.0);
    int far_right = (int)((((double)ref.r3 - (double)far_right_white)/(23999.0 - (double)far_right_white))* 255.0);
    int far_left = (int)((((double)ref.l3 - (double)far_left_white)/(23999.0 - (double)far_left_white)) * 255.0);
    
    
    if(right <1) right = 1;
    if(left <1) left = 1;
    if(far_left<1) far_left = 1;
    if(far_right<1) far_right = 1;
   
    if(left >10 || right >10) {
        if(left < right) {
         right1 = 1;
         left1 = 0;
        }else{
            right1 = 0;
            left1 = 1;
        }
    }
  //  printf("%d %d %d %d\n",far_left,left,right,far_right);
    int right_motor_speed = (int)(4.0*((double)left/((double)left+(double)right)) + 5.0 *((double)far_left/((double)far_left+(double)far_right)));
    int left_motor_speed = (int)(4.0*((double)right/((double)left+(double)right)) + 5.0 *((double)far_right/((double)far_left+(double)far_right)));
    double highest;
    double multiplier;

    if (left_motor_speed > right_motor_speed) {
        highest = left_motor_speed;
        
    } else {
        highest = right_motor_speed;
    }
    multiplier = 255.0/highest;
    
    left_motor_speed =(int)((double)left_motor_speed* multiplier);
    right_motor_speed = (int)((double)right_motor_speed*multiplier);
    
    //printf("Left motor %d  Right motor %d\n", left_motor_speed, right_motor_speed);
    
    if(left_motor_speed - right_motor_speed > 120) {
        motor_turnHardRight(left_motor_speed,right_motor_speed,1);
    } else if( right_motor_speed - left_motor_speed > 120) {
        motor_turnHardLeft(left_motor_speed,right_motor_speed,1);
    } else {
        motor_turn(left_motor_speed ,right_motor_speed,1);
    }
    
    previous_left3 = ref.l3;
    previous_right3 = ref.r3;
    
    left1 = dig.l1;
    left3 = dig.l3;
    right1 = dig.r1;
    right3 = dig.r3;
}

void move_to_blackline (){
    struct sensors_ ref;
    reflectance_read(&ref);

    move_smooth();
        if (ref.l3 > 23500 && ref.r3 > 23500) {
         is_on_blackline = 1; 
        } 
}
//*/



//reflectance//
/*int main()
{
    ADC_Battery_Start();
    CyGlobalIntEnable; 
    UART_1_Start();
  
    sensor_isr_StartEx(sensor_isr_handler);
    
    reflectance_start();

    IR_led_Write(1);
    
    CyDelay(10);
   
    BatteryLed_Write(1); // Switch led on
    check_battery();
    
    while (SW1_Read() == 1); // Calibrate
    CyDelay(500);
    struct sensors_ ref;

    
    reflectance_read(&ref);
    
    int blackl3 = ref.l3;
    int blackl1 = ref.l1;
    int blackr1 = ref.r1;
    int blackr3 = ref.r3;
    motor_start();
    motor_forward(100,500);
    motor_stop();
    reflectance_read(&ref);
    int whitel3 = ref.l3;
    int whitel1 = ref.l1;
    int whiter1 = ref.r1;
    int whiter3 = ref.r3;
    
    
    reflectance_set_threshold(threshold_calculator(blackl3,whitel3),
                              threshold_calculator(blackl1,whitel1),
                              threshold_calculator(blackr1,whiter1),
                              threshold_calculator(blackr3,whiter3));
    while (SW1_Read() == 1); //Start to move
    CyDelay(1000);
    
    long timer=0;
    motor_start();
    while(blackLines < 3)
    {
        move();
        //CyDelay(10);
        timer += 1;
    }
    
    motor_stop();
    while(1){
    printf("%ld\n",timer);
    CyDelay(1000);
    }
    return 0;
    
}
int threshold_calculator (int black, int white) {
 return white + ((black - white)/2);   
}*/
/*
void move () {

        struct sensors_ dig;

        reflectance_digital(&dig);      
   
        if( (dig.l1 == 0 && dig.r1 == 0 && dig.l3 == 1 && dig.r3 == 1) || (dig.l1 == 0 && dig.r1 == 0 && dig.l3 == 0 && dig.r3 == 0)){
            motor_forward(255,1);
        }
        else if (dig.l1 == 1 && dig.r1 == 0) {
            motor_turn(255,170,1);            
        }
        else if (dig.l1 == 0 && dig.r1 == 1) {
            motor_turn(170,255,1);   
        }else if ((dig.l3  == 0 || dig.r3 == 0) && (dig.l1 == 1 && dig.r1 == 1)) {
            if(dig.l3 == 0) {
             //   motor_turn(5,255,1);
                motor_turnHardLeft(70,255,1);
            } if (dig.r3 == 0) {
             //   motor_turn(255,5,1);   
                motor_turnHardRight(255,70,1);
            }
        }
       
}
*/

    



#if 0
int rread(void)
{
    SC0_SetDriveMode(PIN_DM_STRONG);
    SC0_Write(1);
    CyDelayUs(10);
    SC0_SetDriveMode(PIN_DM_DIG_HIZ);
    Timer_1_Start();
    uint16_t start = Timer_1_ReadCounter();
    uint16_t end = 0;
    while(!(Timer_1_ReadStatusRegister() & Timer_1_STATUS_TC)) {
        if(SC0_Read() == 0 && end == 0) {
            end = Timer_1_ReadCounter();
        }
    }
    Timer_1_Stop();
    
    return (start - end);
}
#endif

/* Don't remove the functions below */
int _write(int file, char *ptr, int len)
{
    (void)file; /* Parameter is not used, suppress unused argument warning */
	int n;
	for(n = 0; n < len; n++) {
        if(*ptr == '\n') UART_1_PutChar('\r');
		UART_1_PutChar(*ptr++);
	}
	return len;
}

int _read (int file, char *ptr, int count)
{
    int chs = 0;
    char ch;
 
    (void)file; /* Parameter is not used, suppress unused argument warning */
    while(count > 0) {
        ch = UART_1_GetChar();
        if(ch != 0) {
            UART_1_PutChar(ch);
            chs++;
            if(ch == '\r') {
                ch = '\n';
                UART_1_PutChar(ch);
            }
            *ptr++ = ch;
            count--;
            if(ch == '\n') break;
        }
    }
    return chs;
}
/* [] END OF FILE */

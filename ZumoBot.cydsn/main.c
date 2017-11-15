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
    int left = 1;
    int left3 = 1;
    int right = 1;
    int right3 = 1;

    int blackLines=0;
/**
 * @file    main.c
 * @brief   
 * @details  ** You should enable global interrupt for operating properly. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/


//battery level//

/*int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    ADC_Battery_Start();        
    //int16 adcresult =0;
    //float volts = 0.0;

    printf("\nBoot\n");
    
    //BatteryLed_Write(1); // Switch led on 
    //BatteryLed_Write(0); // Switch led off

    //uint8 button;
    //button = SW1_Read(); // read SW1 on pSoC board
    while (SW1_Read() == 1);
    printf("Startting program\n");
    BatteryLed_Write(1); // Switch led on
    check_battery();
    CyDelay(2000);
        
    motor_start();                  // motor start
        
    motor_turn(230,250,1800);       //go forward
    motor_turn(250,10,335);          //turn 90 degrees right
    motor_turn(230,250,1300);       //go forward
    motor_turn(250,10,335);          //turn 90 degrees right
    motor_turn(230,250,1400);       //go forward
    motor_turn(250,10,375);          //turn 90+ degrees right
    motor_turn(200,150,2000);       //turn smooth right
    motor_turn(233,250,300);
  //motor_backward(50,2000);        //moving backward
       
    motor_stop();                   // motor stop
    
    // test comment
    
    return 0;
 }*/
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
//*/


/*//ultra sonic sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    Ultra_Start();                          // Ultra Sonic Start function
    while(1) {
        //If you want to print out the value  
        printf("distance = %5.0f\r\n", Ultra_GetDistance());
        CyDelay(1000);
    }
}   
//*/


/*//nunchuk//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    nunchuk_start();
    nunchuk_init();
    
    for(;;)
    {    
        nunchuk_read();
    }
}   
//*/


/*//IR receiver//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    
    unsigned int IR_val; 
    
    for(;;)
    {
       IR_val = get_IR();
       printf("%x\r\n\n",IR_val);
    }    
 }   
//*/


/*//Ambient light sensor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
    
    I2C_Start();
    
    I2C_write(0x29,0x80,0x00);          // set to power down
    I2C_write(0x29,0x80,0x03);          // set to power on
    
    for(;;)
    {    
        uint8 Data0Low,Data0High,Data1Low,Data1High;
        Data0Low = I2C_read(0x29,CH0_L);
        Data0High = I2C_read(0x29,CH0_H);
        Data1Low = I2C_read(0x29,CH1_L);
        Data1High = I2C_read(0x29,CH1_H);
        
        uint8 CH0, CH1;
        CH0 = convert_raw(Data0Low,Data0High);      // combine Data0
        CH1 = convert_raw(Data1Low,Data1High);      // combine Data1

        double Ch0 = CH0;
        double Ch1 = CH1;
        
        double data = 0;
        data = getLux(Ch0,Ch1);
        
        // If you want to print out data
        //printf("%lf\r\n",data);    
    }    
 }   
//*/


/*//accelerometer//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
  
    uint8 X_L_A, X_H_A, Y_L_A, Y_H_A, Z_L_A, Z_H_A;
    int16 X_AXIS_A, Y_AXIS_A, Z_AXIS_A;
    
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL1_REG, 0x37);           // set accelerometer & magnetometer into active mode
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL7_REG, 0x22);
    
    
    for(;;)
    {
        //print out accelerometer output
        X_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_X_L_A);
        X_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_X_H_A);
        X_AXIS_A = convert_raw(X_L_A, X_H_A);
        
        Y_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_Y_L_A);
        Y_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_Y_H_A);
        Y_AXIS_A = convert_raw(Y_L_A, Y_H_A);
        
        Z_L_A = I2C_read(ACCEL_MAG_ADDR, OUT_Z_L_A);
        Z_H_A = I2C_read(ACCEL_MAG_ADDR, OUT_Z_H_A);
        Z_AXIS_A = convert_raw(Z_L_A, Z_H_A);
        
        printf("ACCEL: %d %d %d %d %d %d \r\n", X_L_A, X_H_A, Y_L_A, Y_H_A, Z_L_A, Z_H_A);
        value_convert_accel(X_AXIS_A, Y_AXIS_A, Z_AXIS_A);
        printf("\n");
        
        CyDelay(50);
    }
}   
//*/


//reflectance//
int main()
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
}
void move () {
    struct sensors_ ref;
    struct sensors_ dig;
    
    reflectance_read(&ref);
        //printf("%d %d %d %d \r\n", ref.l3, ref.l1, ref.r1, ref.r3);       //print out each period of reflectance sensors
        reflectance_digital(&dig);      //print out 0 or 1 according to results of reflectance period
       // printf("%d %d %d %d \r\n", dig.l3, dig.l1, dig.r1, dig.r3);        //print out 0 or 1 according to results of reflectance period
       // CyDelay(100);
        if (!(left == 0 && left3 == 0 && right == 0 && right3 == 0) && (dig.l1 ==  0 && dig.r1 == 0 && dig.r3 == 0 && dig.l3 == 0) ){
            blackLines +=1;
        }
        
        
        
        
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
        }else {
            if(left == 0){
                motor_turn(10,255,1);  
            } else if (right == 0) {
                motor_turn(255,10,1);  
            }
        }
        if(!(dig.l1 ==  1 && dig.r1 == 1 && dig.r3 == 1 && dig.l3 == 1)){
            left = dig.l1;
            right = dig.r1;
            right3 = dig.r3;
            left3 = dig.l3;
        }
}
//*/

 /* //motor//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();

    motor_start();              // motor start

    motor_forward(100,2000);     // moving forward
    motor_turn(200,50,2000);     // turn
    motor_turn(50,200,2000);     // turn
    motor_backward(100,2000);    // movinb backward
       
    motor_stop();               // motor stop
    
    for(;;)
    {

    }
}
//*/
    

/*//gyroscope//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
  
    uint8 X_L_G, X_H_G, Y_L_G, Y_H_G, Z_L_G, Z_H_G;
    int16 X_AXIS_G, Y_AXIS_G, Z_AXIS_G;
    
    I2C_write(GYRO_ADDR, GYRO_CTRL1_REG, 0x0F);             // set gyroscope into active mode
    I2C_write(GYRO_ADDR, GYRO_CTRL4_REG, 0x30);             // set full scale selection to 2000dps    
    
    for(;;)
    {
        //print out gyroscope output
        X_L_G = I2C_read(GYRO_ADDR, OUT_X_AXIS_L);
        X_H_G = I2C_read(GYRO_ADDR, OUT_X_AXIS_H);
        X_AXIS_G = convert_raw(X_H_G, X_L_G);
        
        
        Y_L_G = I2C_read(GYRO_ADDR, OUT_Y_AXIS_L);
        Y_H_G = I2C_read(GYRO_ADDR, OUT_Y_AXIS_H);
        Y_AXIS_G = convert_raw(Y_H_G, Y_L_G);
        
        
        Z_L_G = I2C_read(GYRO_ADDR, OUT_Z_AXIS_L);
        Z_H_G = I2C_read(GYRO_ADDR, OUT_Z_AXIS_H);
        Z_AXIS_G = convert_raw(Z_H_G, Z_L_G);
     
        // If you want to print value
        printf("%d %d %d \r\n", X_AXIS_G, Y_AXIS_G, Z_AXIS_G);
        CyDelay(50);
    }
}   
//*/


/*//magnetometer//
int main()
{
    CyGlobalIntEnable; 
    UART_1_Start();
  
    I2C_Start();
   
    uint8 X_L_M, X_H_M, Y_L_M, Y_H_M, Z_L_M, Z_H_M;
    int16 X_AXIS, Y_AXIS, Z_AXIS;
    
    I2C_write(GYRO_ADDR, GYRO_CTRL1_REG, 0x0F);             // set gyroscope into active mode
    I2C_write(GYRO_ADDR, GYRO_CTRL4_REG, 0x30);             // set full scale selection to 2000dps
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL1_REG, 0x37);           // set accelerometer & magnetometer into active mode
    I2C_write(ACCEL_MAG_ADDR, ACCEL_CTRL7_REG, 0x22);
    
    
    for(;;)
    {
        X_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_X_L_M);
        X_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_X_H_M);
        X_AXIS = convert_raw(X_L_M, X_H_M);
        
        Y_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_Y_L_M);
        Y_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_Y_H_M);
        Y_AXIS = convert_raw(Y_L_M, Y_H_M);
        
        Z_L_M = I2C_read(ACCEL_MAG_ADDR, OUT_Z_L_M);
        Z_H_M = I2C_read(ACCEL_MAG_ADDR, OUT_Z_H_M);
        Z_AXIS = convert_raw(Z_L_M, Z_H_M);
        
        heading(X_AXIS, Y_AXIS);
        printf("MAGNET: %d %d %d %d %d %d \r\n", X_L_M, X_H_M, Y_L_M, Y_H_M, Z_L_M, Z_H_M);
        printf("%d %d %d \r\n", X_AXIS,Y_AXIS, Z_AXIS);
        CyDelay(50);      
    }
}   
//*/


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

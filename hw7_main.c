#include "nu32dip.h" // constants, functions for startup and UART
#include "i2c_master_noint.h"
#include "mpu6050.h"
#include <stdio.h>

#define data_pts 1

//ADDRESS of the IMU: 0b1101000 = 0x68
int main(void) {
    NU32DIP_Startup();
    i2c_master_setup();
    init_mpu6050();
    NU32DIP_YELLOW = 0;
	NU32DIP_GREEN = 0;
    
	char m_in[10]; //small char array to detect when to print
    char m_out[100]; //to print out data
    
    //float arrays for acceleration and gyroscopic data
    float ax[data_pts],
        ay[data_pts],
        az[data_pts],
        gx[data_pts],
        gy[data_pts],
        gz[data_pts],
        temp[data_pts];
    
    char w = whoami();
    if(w != 0x68){
        //Light the yellow LED
        NU32DIP_YELLOW = 1;
        
        //can't proceed if we aren't talking to the IMU
        while(1){
            ;
        }
    }
    NU32DIP_YELLOW = 1;
    
    //each data point is 2 bytes, we have 6 acceleration data points
    // and 1 temperature data point
    //2*7 = 14 bytes
    char IMU_BUF[14];
    
    //get a newline before printing
    //NU32DIP_ReadUART1(m_in,10);

    while (1) {
		// use core timer for exactly 100Hz loop
        _CP0_SET_COUNT(0);
        
        // read IMU, convert and store data
        for(int i = 0; i < data_pts ; i++){
           
            //24MHz core timer
            _CP0_SET_COUNT(0);
            
            burst_read_mpu6050(IMU_BUF);
            ax[i] = conv_xXL(IMU_BUF);
            ay[i] = conv_yXL(IMU_BUF);
            az[i] = conv_zXL(IMU_BUF);
            gx[i] = conv_xG(IMU_BUF);
            gy[i] = conv_yG(IMU_BUF);
            gz[i] = conv_zG(IMU_BUF);
            temp[i] = conv_temp(IMU_BUF);
            
            //Print x acceleration only
            sprintf(m_out, "%f\r\n", ax[i]);
            
            //Print all data:
//            sprintf(m_out, "ax: %.3f ay: %.3f az: %.3f gx: %.3f"
//                    " gy: %.3f gz: %.3f temp: %.3f\r\n", 
//                    ax[i], az[i], az[i], gx[i], gy[i], gz[i], temp[i]);
            NU32DIP_WriteUART1(m_out);
        }
        
        while (_CP0_GET_COUNT() < 24000000/100) {
        }
        
        //arrays contain data, print out to serial
//        for(int i = 0; i < data_pts; i++){
//            sprintf(m_out, "%f\r\n", ax[i]);
//            NU32DIP_WriteUART1(m_out);
//        }
    }
}

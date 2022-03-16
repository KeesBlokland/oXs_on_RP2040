#include "voltage.h"
#include "hardware/adc.h"
#include "tools.h"
#include "stdio.h"
#include <inttypes.h>
#include "param.h"

extern CONFIG config;

VOLTAGE::VOLTAGE() {}

void VOLTAGE::begin(void ) {
  adc_init (); // prepare ADC
  for (int cntInit = 0 ; cntInit < MAX_NBR_VOLTAGES ; cntInit++) {
    adc_gpio_init(pin[cntInit]); // prepare the pin for ADC
    mVolt[cntInit].value = 0;
    mVolt[cntInit].available = false ;
  } // end for
  // 330 because the max volt is 3.3V and we expect 2 decimals - to check if this is correct
  mVoltPerStep[0] = 330 / 4095.0  * config.scaleVolt1;
  mVoltPerStep[1] = 330 / 4095.0  * config.scaleVolt2;
  mVoltPerStep[2] = 330 / 4095.0  * config.scaleVolt3;
  mVoltPerStep[3] = 330 / 4095.0  * config.scaleVolt4;  
  offset[0] = config.offset1; 
  offset[1] = config.offset2;
  offset[2] = config.offset3;
  offset[3] = config.offset4;  
} // end begin

void VOLTAGE::getVoltages(void){
    static uint8_t sumCount = 0;
    static uint32_t lastVoltagemillis = 0 ;
    if ( (millis() - lastVoltagemillis) > VOLTAGEINTERVAL ) {
        lastVoltagemillis = millis() ;
        for (int cntInit = 0 ; cntInit < MAX_NBR_VOLTAGES ; cntInit++) {
            adc_select_input(cntInit); // select the pin
            sumVoltage[cntInit] += adc_read(); // convert and sum
        }
        sumCount++;
        if ( sumCount == SUM_COUNT_MAX_VOLTAGE ) {
            sumCount = 0;
            for (int cntInit = 0 ; cntInit < MAX_NBR_VOLTAGES ; cntInit++) {
                mVolt[cntInit].value = (sumVoltage[cntInit] / SUM_COUNT_MAX_VOLTAGE * mVoltPerStep[cntInit]) - offset[cntInit];
                mVolt[cntInit].available = true ;
                sumVoltage[cntInit] = 0 ;
                //printf("voltage has been measured: %d value= %d \n", cntInit , (int) mVolt[cntInit].value);  
            }    
        }
    }        
}

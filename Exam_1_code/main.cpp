#include "mbed.h"
#include "uLCD_4DGL.h"

DigitalIn D_3(D3);
DigitalIn D_5(D5);
DigitalIn D_6(D6);
AnalogIn Ain(A0);
AnalogOut Aout(D7);
DigitalOut led(LED3);
uLCD_4DGL uLCD(D1, D0, D2);
Thread dac;

// make the button work once a time
int one_pulse(int x, int last_in);
// display the frequency on the LCD 
void LCD(float SR);
// transfer the digital data to analog   
void DAC_f(float *SR);
// transfer the analog data to digital                
void ADC_f(void);                   

int main(void)
{
    int last_D3 = 0;
    int last_D5 = 0;
    int last_D7 = 0;
    int up = 0;
    int down = 0; 
    int sel = 0;
    float dis_SR = 0.125;
    float use_SR = 0.125;
    led = 0;

    dac.start(callback(&DAC_f, &use_SR));

    while(1)
    {
        up = one_pulse(D_3, last_D3);
        last_D3 = D_3;

        down = one_pulse(D_5, last_D5);
        last_D5 = D_5;

        sel = one_pulse(D_6, last_D7);
        last_D7 = D_6;

        if(up)
        {
            led = !led;
            if (dis_SR != 1.0)
                dis_SR *= 2;
            else
                dis_SR = 1.0;
        }
        else if(down)
        {
            led = !led;
            if (dis_SR != 0.125)
                dis_SR /= 2;
            else
                dis_SR = 0.125;                               
        }
        // display the frequency now
        LCD(dis_SR);

        if (sel) 
        {
            // select the frequency
            use_SR = dis_SR;
            // start the ADC

        } 
    }
}

int one_pulse(int x, int last_in)
{
    if (x)
        if (x == last_in)
            return 0;
        else
            return 1;
    else
        return 0;            
}

void LCD(float SR)
{
    uLCD.text_width(4);
    uLCD.text_height(4);
    uLCD.locate(0,1);
    if (SR == 0.125)
        uLCD.printf("1/8");
    else if (SR == 0.25)
        uLCD.printf("1/4");
    else if (SR == 0.5)
        uLCD.printf("1/2");
    else
        uLCD.printf("1.0");
}

void DAC_f(float *SR)
{
    while(1)
    {
        for (int i = 0; i < 12000 / 4 ; i++)
        {
            if (i < ((1.0 / 3.0)  * (12000.0 / 4.0) * *SR))
                // the fucnction of wave
                Aout = ((3.0 / (1.0 / (3.0 * 4.166))) * (i / (12000.0 / 4.0))) / 3.3;
            else if (i > ((12000 / 4) - (1.0 / 3.0)  * (12000.0 / 4.0) * *SR))
                // the fucnction of wave
                Aout = (3.0 - ((3.0 / (1.0 / (3.0 * 4.166))) * ((i - ((12000 / 4) - (1.0 / 3.0)  * (12000.0 / 4.0) * *SR) / (12000.0 / 4.0))))) / 3.3;
            else
                Aout = 3.0 / 3.3;            
        }
    }   
}
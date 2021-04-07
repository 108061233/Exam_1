#include "mbed.h"
#include "uLCD_4DGL.h"

using namespace std::chrono;

DigitalIn D_3(D3);
DigitalIn D_5(D5);
DigitalIn D_6(D6);
AnalogIn Ain(A0);
AnalogOut Aout(D7);
DigitalOut led(LED3);
uLCD_4DGL uLCD(D1, D0, D2);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread dac;
Timer t;

// make the button work once a time
int one_pulse(int x, int last_in);
// display the frequency on the LCD 
void LCD(float SR);
// transfer the digital data to analog
void DAC(float *SR); 
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
            ADC_f();
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
    uLCD.text_width(3);
    uLCD.text_height(3);
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
        t.start();
        for (int i = 0; i < 6000; i++)
        {
            if (i < ((1.0 / 3.0)  * 6000.0 * *SR))
                // the fucnction of wave
                Aout = (3.0 * i / (6000.0 * *SR / 3.0)) / 3.3;
            else if (i > (6000 - (1.0 / 3.0) * (6000.0 * *SR)))
                // the fucnction of wave
                Aout = (3.0 - (3.0 * (i - (6000 - (1.0 / 3.0) * (6000.0 * *SR))) / (6000.0 * *SR / 3.0))) / 3.3;
            else
                Aout = 3.0 / 3.3;            
        }
        t.stop();
        auto ms = chrono::duration_cast<chrono::milliseconds>(t.elapsed_time()).count();
        printf ("Timer time: %llu ms\n", ms);
    }   
}

void ADC_f()
{
    float ADCdata[600];
    // the sample frequency I use here is not 600 / 0.6, need to use timer to test the actual time
    // and the frequency is 600 / actual time 
    for (int i = 0; i < 600; i++)
    {
        ADCdata[i] = Ain;         
        ThisThread::sleep_for(1ms);
    }

    for (int i = 0; i < 600; i++)
    {
        printf("%f\r\n", ADCdata[i]);
    }
}
#include <wiringPi.h>
#include <lcd.h>
#include "pwm.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>

//USE WIRINGPI PIN NUMBERS
#define LCD_RS  2               //Register select pin
#define LCD_E   3               //Enable Pin
#define LCD_D4  6               //Data pin 4
#define LCD_D5  5               //Data pin 5
#define LCD_D6  4               //Data pin 6
#define LCD_D7  1               //Data pin 7
#define MAXTIMINGS 85
#define DHTPIN 27

//PWM PIN
#define PIN             28
#define RANGE           1000
#define VALUE           100


int lcd;
int dht11_dat[5] = {0, 0, 0, 0, 0};

void stopPWM()
{
        printf("Остановка\n");
        PwmStop(PIN);
        exit(0);
}

int read_dht11_dat()
{
        uint8_t laststate = HIGH;
        uint8_t counter = 0;
        uint8_t j = 0, i;
        float f;
        dht11_dat[0] = dht11_dat[1] = dht11_dat[2] = dht11_dat[3] = dht11_dat[4] = 0;

        pinMode(DHTPIN, OUTPUT);
        digitalWrite(DHTPIN, LOW);
        delay(18);

        digitalWrite(DHTPIN, HIGH);
        delayMicroseconds(40);

        pinMode(DHTPIN, INPUT);

        for (i = 0; i < MAXTIMINGS; i++)
        {
                counter = 0;
                while (digitalRead(DHTPIN) == laststate)
                {
                        counter++;
                        delayMicroseconds(1);
                        if (counter == 255)
 {
                                break;
                        }
                }
                laststate = digitalRead(DHTPIN);

                if (counter == 255)
                        break;

                if ((i >= 4) && (i % 2 == 0))
                {
                        dht11_dat[j / 8] <<= 1;
                        if (counter > 50)
                                dht11_dat[j / 8] |= 1;
                        j++;
                }
         }

        if ((j >= 40) && (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)))
        {

                lcdPosition(lcd, 0, 0);
                lcdPrintf(lcd, "Humidity: %d.%d %%\n", dht11_dat[0], dht11_dat[1]);
                lcdPosition(lcd, 0, 1);
                lcdPrintf(lcd, "Temp: %d.0 C", dht11_dat[2]);
                return dht11_dat[2];
        }

        return -1;
}

int main(void)
{
        int lcd;
        wiringPiSetup();
        lcd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0);
        PwmCreate(PIN, VALUE, RANGE);
        float pwmValue = 0;
        int temp = 0;
        signal(SIGSTOP, stopPWM);
        while (1)
        {

                temp = read_dht11_dat();
                if(temp == -1)
                        continue;
                delay(4000);
                pwmValue = 50*temp - 850;

                if(temp < 28){
                        pwmValue = 0;
                }

                PwmWriteValue(PIN, pwmValue);
                printf("Temp = %d\n D = %f % \n", temp, pwmValue/RANGE * 100);
               // delay(2000);
        }

        return(0);
}

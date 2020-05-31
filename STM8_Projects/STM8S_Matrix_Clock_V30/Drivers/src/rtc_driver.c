/*******************************************************************************
** File Name:  rtc_driver.c
** Created on: Mar 14, 2018
** Author: quoclaptk
*******************************************************************************/
#include "rtc_driver.h"
/******************************************************************************/
const uint8_t ALdauthangDL[22][12] = {
    {17,18,16,27,18,19,20,21,23,24,25,26},//2010
    {27,29,27,28,29,30, 1, 2, 4, 5, 6, 7},//2011
    { 8,10, 9,11,11,12,13,14,16,16,18,18},//2012
    {20,21,20,21,22,23,24,25,26,27,28,29},//2013
    { 1, 2, 1, 2, 3, 4, 5, 6, 8, 8, 9,10},//2014
    {11,13,11,13,13,15,16,17,19,19,20,20},//2015
    {22,23,23,24,25,26,27,29, 1, 1, 2, 3},//2016
    { 4, 5, 4, 5, 6, 7, 8,10,11,12,13,14},//2017
    {15,16,14,16,16,18,18,20,22,22,24,25},//2018
    {26,27,25,27,27,28,29, 1, 3, 3, 5, 6},//2019
    { 7, 8, 8, 9, 9,10,11,12,14,15,16,17},//2020
    {19,20,18,20,20,21,22,23,25,25,27,27},//2021
    {29, 1,29, 1, 1, 3, 3, 4, 6, 6, 8, 8},//2022
    {10,11,20,11,12,14,14,15,17,17,18,19},//2023
    {20,22,21,23,23,25,26,27,29,29, 1, 1},//2024
    { 2, 4, 2, 4, 4, 6, 7, 8,10,10,12,12},//2025
    {13,14,13,14,15,16,17,19,20,21,23,23},//2026
    {24,25,24,25,25,27,27,29, 1, 2, 4, 4},//2027   
    { 5, 7, 6, 7, 7, 9, 9,11,13,13,15,16},//2028
    {17,18,17,18,18,20,20,22,23,24,25,26},//2029
    {28,29,28,29,29, 1, 1, 3, 4, 5, 6, 7} //2030
};
/******************************************************************************/
const uint8_t thangALdauthangDL[22][12] = {
    {11,12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10},//2010
    {11,12, 1, 2, 3, 4, 6, 7, 8, 9,10,11},//2011
    {12, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9,10},//2012
    {11,12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10},//2013
    {12, 1, 2, 3, 4, 5, 6, 7, 8, 9, 9,10},//2014
    {11,12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10},//2015
    {11,12, 1, 2, 3, 4, 5, 6, 8, 9,10,11},//2016
    {12, 1, 2, 3, 4, 5, 6, 6, 7, 8, 9,10},//2017
    {11,12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10},//2018
    {11,12, 1, 2, 3, 4, 5, 7, 8, 9,10,11},//2019
    {12, 1, 2, 3, 4, 4, 5, 6, 7, 8, 9,10},//2020
    {11,12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10},//2021
    {11, 1, 1, 3, 4, 5, 6, 7, 8, 9,10,11},//2022
    {12, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9,10},//2023
    {11,12, 1, 2, 3, 4, 5, 6, 7, 8,10,11},//2024
    {12, 1, 2, 3, 4, 5, 6, 6, 7, 8, 9,10},//2025
    {11,12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10},//2026
    {11,12, 1, 2, 3, 4, 5, 6, 8, 9,10,11},//2027
    {12, 1, 2, 3, 4, 5, 5, 6, 7, 8, 9,10},//2028
    {11,12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10},//2029
    {11,12, 1, 2, 3, 5, 6, 7, 8, 9,10,11} //2030
};
/******************************************************************************/
const uint8_t DLdauthangAL[22][12] = {
    {15,14,16, 5,14,12,12,10, 8, 8, 6, 6},//2010
    { 4, 3, 5, 3, 3, 2,31,29,27,27,25,25},//2011
    {23,22,22,21,21,19,19,17,16,15,14,13},//2012
    {12,10,12,10,10, 8, 8, 7, 5, 5, 3, 3},//2013
    {31,31,31,29,29,27,27,25,24,24,22,22},//2014
    {20,19,20,19,18,16,16,14,13,13,12,11},//2015
    {10, 8, 9, 7, 7, 5, 4, 3, 1,31,29,29},//2016
    {28,26,28,26,26,24,23,22,20,20,18,18},//2017
    {17,16,17,16,15,14,13,11,10, 9, 7, 7},//2018
    { 6, 5, 6, 5, 5, 3, 3,30,29,28,26,26},//2019
    {25,23,24,23,23,21,21,19,17,17,15,14},//2020
    {13,12,13,12,12,10,10, 8, 7, 6, 5, 4},//2021
    { 3,29, 3,31,30,29,29,27,26,25,24,23},//2022
    {22,20,22,20,19,18,18,16,15,15,13,13},//2023
    {11,10,10, 9, 8, 6, 6, 4, 3, 3,31,31},//2024
    {29,28,29,28,27,25,25,23,22,21,20,20},//2025
    {19,17,19,17,17,15,14,13,11,10, 9, 9},//2026
    { 8, 6, 8, 7, 6, 5, 4, 1,30,29,28,28},//2027
    {26,25,26,25,24,23,22,20,19,18,16,16},//2028
    {15,23,15,14,13,12,11,10, 8, 8, 6, 5},//2029
    { 4, 2, 4,31, 2,30,30,29,27,27,25,25} //2030
};
/******************************************************************************/
const uint8_t thangALdauthangAL[22][12] = {
    {12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11},//2010
    {12, 1, 2, 3, 4, 5, 7, 8, 9,10,11,12},//2011
    { 1, 2, 3, 4, 4, 5, 6, 7, 8, 9,10,11},//2012
    {12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11},//2013
    { 1, 2, 3, 4, 5, 6, 7, 8, 9, 9,10,11},//2014
    {12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11},//2015
    {12, 1, 2, 3, 4, 5, 6, 7, 8,10,11,12},//2016
    { 1, 2, 3, 4, 5, 6, 6, 7, 8, 9,10,11},//2017
    {12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11},//2018
    {12, 1, 2, 3, 4, 5, 6, 8, 9,10,11,12},//2019
    { 1, 2, 3, 4, 4, 5, 6, 7, 8, 9,10,11},//2020
    {12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11},//2021
    {12, 1, 2, 3, 5, 6, 7, 8, 9,10,11,12},//2022
    { 1, 2, 2, 3, 4, 5, 6, 7, 8, 9,10,11},//2023
    {12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,12},//2024
    { 1, 2, 3, 4, 5, 6, 6, 7, 8, 9,10,11},//2025
    {12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11},//2026
    {12, 1, 2, 3, 4, 5, 6, 7, 9,10,11,12},//2027
    { 1, 2, 3, 4, 5, 5, 6, 7, 8, 9,10,11},//2028
    {12, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11},//2029
    {12, 1, 2, 3, 4, 5, 7, 8, 9,10,11,12} //2030
};
/******************************************************************************/
void RTC_Init(void)
{
    I2C_Start();                            // Start I2C communication
    I2C_WriteByte(DS1307_WRITE_MODE);        // Connect to DS1307 by sending its ID on I2c Bus
    I2C_WriteByte(DS1307_CONTROL_REG_ADDRESS);// Select the Ds1307 ControlRegister to configure Ds1307
    I2C_WriteByte(0x00);                        // Write 0x00 to Control register to disable SQW-Out
    I2C_Stop();                             // Stop I2C communication after initializing DS1307
}
/******************************************************************************/
void RTC_SetDateTime(rtc_t *rtc)
{
    I2C_Start();                          // Start I2C communication
    I2C_WriteByte(DS1307_WRITE_MODE);      // connect to DS1307 by sending its ID on I2c Bus
    I2C_WriteByte(DS1307_SECOND_REG_ADDRESS); // Request sec RAM address at 00H
    I2C_WriteByte(rtc->sec);                    // Write sec from RAM address 00H
    I2C_WriteByte(rtc->min);                    // Write min from RAM address 01H
    I2C_WriteByte(rtc->hour);                    // Write hour from RAM address 02H
    I2C_WriteByte(rtc->weekDay);                // Write weekDay on RAM address 03H
    I2C_WriteByte(rtc->date);                    // Write date on RAM address 04H
    I2C_WriteByte(rtc->month);                    // Write month on RAM address 05H
    I2C_WriteByte(rtc->year);                    // Write year on RAM address 06h
    I2C_Stop();                              // Stop I2C communication after Setting the Date
}
/******************************************************************************/
void RTC_GetDateTime(rtc_t *rtc)
{
    I2C_Start();                            // Start I2C communication

    I2C_WriteByte(DS1307_WRITE_MODE);        // connect to DS1307 by sending its ID on I2c Bus
    I2C_WriteByte(DS1307_SECOND_REG_ADDRESS); // Request Sec RAM address at 00H
    I2C_Stop();                                // Stop I2C communication after selecting Sec Register
    I2C_Start();                            // Start I2C communication
    I2C_WriteByte(DS1307_READ_MODE);            // connect to DS1307(Read mode) by sending its ID
    rtc->sec = I2C_ReadByteACK();                // read second and return Positive ACK
    rtc->min = I2C_ReadByteACK();                 // read minute and return Positive ACK
    rtc->hour= I2C_ReadByteACK();               // read hour and return Negative/No ACK
    rtc->weekDay = I2C_ReadByteACK();           // read weekDay and return Positive ACK
    rtc->date= I2C_ReadByteACK();              // read Date and return Positive ACK
    rtc->month=I2C_ReadByteACK();            // read Month and return Positive ACK
    rtc->year =I2C_ReadByteACK();             // read Year and return Negative/No ACK
    I2C_Stop();                              // Stop I2C communication after reading the Date
}
/******************************************************************************/
uint32_t lunar_calculate(uint8_t x, uint8_t y, uint8_t t) 
{
    unsigned char timeRTC_Mday, timeRTC_Mon, timeRTC_Year, luu, tam_1;
    unsigned char RTCLunarRTC_Mday, RTCLunarRTC_Mon, RTCLunarRTC_Year;
    unsigned char da, db, day_1, mon_1, year_1;
    unsigned char lmon;
    uint32_t luu_amlich=0;
    timeRTC_Mday = x;
    timeRTC_Mon = y;
    timeRTC_Year = t;
    day_1 = timeRTC_Mday;
    mon_1 = timeRTC_Mon;
    year_1 = timeRTC_Year;
    if ((year_1 - 10) > 30) //cho phep den nam 2030
    {
        goto thoat;
    }
    da = ALdauthangDL[year_1 - 10][mon_1 - 1];
    db = DLdauthangAL[year_1 - 10][mon_1 - 1];
    luu = db - day_1;
    if ((luu) <= 0) 
    {
        RTCLunarRTC_Mday = (day_1 - db + 1);
        RTCLunarRTC_Mon = thangALdauthangAL[year_1 - 10][mon_1 - 1];
    } 
    else 
    {
        if ((luu) > 31) 
        {
            RTCLunarRTC_Mday = (day_1 - db + 1);
            RTCLunarRTC_Mon = thangALdauthangDL[year_1 - 10][mon_1];
        } 
        else 
        {
            RTCLunarRTC_Mday = (day_1 + da - 1);
            RTCLunarRTC_Mon = thangALdauthangDL[year_1 - 10][mon_1 - 1];
        }
    }
    lmon = RTCLunarRTC_Mon;
    tam_1 = mon_1 - lmon;
    if ((tam_1) > 100 && tam_1 < 255)RTCLunarRTC_Year = (year_1 - 1);
    else 
    {
        RTCLunarRTC_Year = year_1;
    }
    luu_amlich = luu_amlich | RTCLunarRTC_Year;
    luu_amlich <<= 8;
    luu_amlich = luu_amlich | RTCLunarRTC_Mon;
    luu_amlich <<= 8;
    luu_amlich = luu_amlich | RTCLunarRTC_Mday;
    luu_amlich <<= 8;
    thoat:
        return luu_amlich;
}
/*******************************************************************************
**                            END OF FILE                                     **
*******************************************************************************/
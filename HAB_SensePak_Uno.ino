/*
 *Assignments 
 *A0-TDS
 *A1-Turb
 *A2-pH
 *A3-WP
 *A4-Chl
 *A5-SD Card //not yet implemented here
 *D0-RX
 *D1-TX
 *D2-DO RX
 *D3-DO TX
 *D4-EC RX
 *D5-EC TX
 *D6-
 *D7-
 *D8-TEMP
 *D9-
 *D10-
 *D11-
 *D12-
 *D13-
 */
//Analog functions
#define TdsSensorPin A0
#define VREF 5.0      // analog reference voltage(Volt) of the ADC
#define SCOUNT  30           // sum of sample point
float TDS(){
  int analogBuffer[SCOUNT];    // store the analog value in the array, read from ADC
  int analogBufferTemp[SCOUNT];
  int analogBufferIndex = 0,copyIndex = 0;
  float averageVoltage = 0,tdsValue = 0,temperature = 25;
//  int16_t adc0;
//  float v0=0.0;
  static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT)
         analogBufferIndex = 0;
   }
   static unsigned long printTimepoint = millis();
   if(millis()-printTimepoint > 800U)
   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
      //Serial.print("voltage:");
      //Serial.print(averageVoltage,2);
      //Serial.print("V   ");
//      Serial.print("TDS Value:");
//      Serial.print(tdsValue,0);
//      Serial.println("ppm");
   }

  return(tdsValue);
}
int getMedianNum(int bArray[], int iFilterLen) //for TDS
{
      int bTab[iFilterLen];
      for (byte i = 0; i<iFilterLen; i++)
      bTab[i] = bArray[i];
      int i, j, bTemp;
      for (j = 0; j < iFilterLen - 1; j++)
      {
      for (i = 0; i < iFilterLen - j - 1; i++)
          {
        if (bTab[i] > bTab[i+1])
            {
        bTemp = bTab[i];
            bTab[i] = bTab[i+1];
        bTab[i+1] = bTemp;
         }
      }
      }
      if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
      else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
      return bTemp;
}
float turb(){
  int adc0;
  float v0=0.0;
  adc0 = analogRead(A1);
  v0 = adc0* (5.0 / 1023.0);//(adc0 * 0.1875)/1000;//* 0.1875/1000;
  //float turbs = -1120.4*v0*v0+ 5742.3*v0-4352.9;// y = -1120.4x^2 + 5742.3x-4352.9
  float turbs = (v0- 4.1037) / (-0.0008);//x = (y - 4.1037) / (-0.0008)
  return(turbs);
}
float pH(){
  int Offset=0.00;            //deviation compensate
  int samplingInterval=20;
  int printInterval= 800;
  int ArrayLenth = 40;    //times of collection

  int pHArray[ArrayLenth];   //Store the average value of the sensor feedback
  int pHArrayIndex=0;
  float ph=0.0;
  int adc0;
  float v0=0.0;
  adc0 = analogRead(A2);
  v0 = adc0*5.0/1023.0;//(adc0 * 0.1875)/1000;// *0.1875/1000;
  ph = 3.5*v0+ Offset;
  return(ph);
  
  
}
float wPress(){
  int adc0;
  float v0=0.0;
  float P=0.0;
  float OffSet  =0.464;
  adc0 =analogRead(A3);
  v0 = adc0*5.0/1023.0;//(adc0 * 0.1875)/1000;// *0.1875/1000;
  P = (v0 - OffSet) * 400;
  return(P);
}
//void serialEvent() {                                  //if the hardware serial port_0 receives a char
//  ECinputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
//  ECinput_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
//}
float chlA(){
  int adc0=0;
  float v0=0.0;
  adc0=analogRead(A4);
  v0 = adc0*5.0/1023.0;
  return(v0);
}


//Digital functions
#include <SoftwareSerial.h>                           //we have to include the SoftwareSerial library, or else we can't use it
#define ECrx 4                                         //define what pin rx is going to be
#define ECtx 5                                          //define what pin tx is going to be
#define DOrx 2                                         //define what pin rx is going to be
#define DOtx 3   
SoftwareSerial ECserial(ECrx, ECtx);                      //define how the soft serial port is going to work
SoftwareSerial DOserial(DOrx, DOtx);                      //define how the soft serial port is going to work

#include <OneWire.h> 
#include <DallasTemperature.h>
OneWire oneWire(8); 
DallasTemperature sensors(&oneWire);

float EC(){
  String ECinputstring = "";                              //a string to hold incoming data from the PC
  String ECsensorstring = "";                             //a string to hold the data from the Atlas Scientific product
  boolean ECinput_string_complete = false;                //have we received all the data from the PC
  boolean ECsensor_string_complete = false;               //have we received all the data from the Atlas Scientific product

   ECserial.begin(9600);                               //set baud rate for the software serial port to 9600
   ECinputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
   ECsensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
   while(ECsensor_string_complete == false){
    if (ECinput_string_complete) {                        //if a string from the PC has been received in its entirety
    ECserial.print(ECinputstring);                      //send that string to the Atlas Scientific product
    ECserial.print('\r');                             //add a <CR> to the end of the string
    ECinputstring = "";                                 //clear the string
    ECinput_string_complete = false;                    //reset the flag used to tell if we have received a completed string from the PC
  }

  if (ECserial.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char ECinchar = (char)ECserial.read();              //get the char we just received
    ECsensorstring += ECinchar;                           //add the char to the var called sensorstring
    if (ECinchar == '\r') {                             //if the incoming character is a <CR>
      ECsensor_string_complete = true;                  //set the flag
    }
  }

  if (ECsensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
//    if (isdigit(ECsensorstring[0]) == false) {          //if the first character in the string is a digit
//      Serial.println(ECsensorstring);                   //send that string to the PC's serial monitor
//    }
//    else                                              //if the first character in the string is NOT a digit
//    {
//      print_EC_data();                                //then call this function 
//    }
//    Serial.println(ECsensorstring); 
    ECserial.end();
    return(ECsensorstring.toFloat());
    ECsensorstring = "";                                //clear the string
    ECsensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
    
  }
  }
   
}

float DO(){
  String DOinputstring = "";                              //a string to hold incoming data from the PC
  String DOsensorstring = "";                             //a string to hold the data from the Atlas Scientific product
  boolean DOinput_string_complete = false;                //have we received all the data from the PC
  boolean DOsensor_string_complete = false;               //have we received all the data from the Atlas Scientific product

   DOserial.begin(9600);                               //set baud rate for the software serial port to 9600
   DOinputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
   DOsensorstring.reserve(30);                           //set aside some bytes for receiving data from Atlas Scientific product
   while(DOsensor_string_complete == false){
    if (DOinput_string_complete) {                        //if a string from the PC has been received in its entirety
    DOserial.print(DOinputstring);                      //send that string to the Atlas Scientific product
    DOserial.print('\r');                             //add a <CR> to the end of the string
    DOinputstring = "";                                 //clear the string
    DOinput_string_complete = false;                    //reset the flag used to tell if we have received a completed string from the PC
  }

  if (DOserial.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char DOinchar = (char)DOserial.read();              //get the char we just received
    DOsensorstring += DOinchar;                           //add the char to the var called sensorstring
    if (DOinchar == '\r') {                             //if the incoming character is a <CR>
      DOsensor_string_complete = true;                  //set the flag
    }
  }


  if (DOsensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
//    if (isdigit(DOsensorstring[0]) == false) {          //if the first character in the string is a digit
//      Serial.println(DOsensorstring);                   //send that string to the PC's serial monitor
//    }
//    else                                              //if the first character in the string is NOT a digit
//    {
//      print_EC_data();                                //then call this function 
//    }
//    Serial.println(DOsensorstring); 
    DOserial.end();
    return(DOsensorstring.toFloat());
    DOsensorstring = "";                                //clear the string
    DOsensor_string_complete = false;                   //reset the flag used to tell if we have received a completed string from the Atlas Scientific product
    
  }
  }
   
}
float temp(){
  float tmpr = 0.0;
  sensors.begin(); 
  sensors.requestTemperatures();
  tmpr = sensors.getTempCByIndex(0);
  return(tmpr);
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(TdsSensorPin,INPUT);
}
float ecsend=0.0;
float dosend=0.0;
float tempsend=0.0;
float tdssend=0.0;
float turbsend=0.0;
float pHsend=0.0;
float wpsend=0.0;
void loop() {
  /*
   * Proper 30 second sampling
   * 1 sample / 1ms
   * 
   */
  float ecread = 0.0;
  float doread = 0.0;
  float tempread = 0.0;
  float tdsread = 0.0;
  float turbread = 0.0;
  float pHread = 0.0;
  float wpread= 0.0;
  int N=10;
  for(int t=0;t<N;t+=1){ //sample for 30,000 ms (1sample/ms)
    ecread += EC();
    doread += DO();
    tempread += temp();
    tdsread += TDS();
    turbread += turb();
    pHread += pH();    
    wpread += wPress();
    delay(1);
  }
  //Average N samples
  ecsend=ecread/float(N);
  dosend=doread/float(N);
  tempsend=tempread/float(N);
  tdssend=tdsread/float(N);
  turbsend=turbread/float(N);
  pHsend=pHread/float(N);
  wpsend=wpread/float(N);
  //Send that $h*t
  Serial.println(String(ecsend)+":"+String(dosend)+":"+String(tempsend)+":"+String(tdssend)+":"+String(turbsend)+":"+String(pHsend)+":"+String(wpsend));

/*
 * Single continuous read; Battery eater edition
 */
//ecread = EC();
//doread = DO();
//tempread = temp();
//tdsread = TDS();
//turbread = turb();
//pHread = pH();    
//wpread = wPress();
//Serial.println(String(ecread)+":"+String(doread)+":"+String(tempread)+":"+String(tdsread)+":"+String(turbread)+":"+String(pHread)+":"+String(wpread));

/*
 * Mema readings lang
 */
//    Serial.println("Digital");
//    Serial.print("EC: ");
//    Serial.println(ecread);
//    Serial.print("DO: ");
//    Serial.println(doread);
//    Serial.print("Temp: ");
//    Serial.println(tempread);
//    Serial.println("Analog");
//    Serial.print("TDS: ");
//    Serial.println(tdsread);
//    Serial.print("Turb: ");
//    Serial.println(turbread);
//    Serial.print("pH: ");
//    Serial.println(pHread);
//    Serial.print("wPress: ");
//    Serial.println(wPread);
//    Serial.println("=====");
}

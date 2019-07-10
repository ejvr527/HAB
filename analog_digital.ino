// --> OBSOLETE
//digital
#include <SoftwareSerial.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

SoftwareSerial myserial(16,17);  //EC
SoftwareSerial myserial1(12,13); //DO

String inputstring = "";
String sensorstring = "";
boolean input_string_complete = false;
boolean sensor_string_complete = false;

String inputstring1 = "";
String sensorstring1 = "";
boolean input_string_complete1 = false;
boolean sensor_string_complete1 = false;

//analog
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_ADS1015.h>
#define PIN_SDA 21
#define PIN_SCL 22
#include <AsyncDelay.h>
#include <SoftWire.h>
SoftWire sw(PIN_SDA, PIN_SCL);

Adafruit_ADS1115 ads(0x48);
float v0 = 0.0;
float v1 = 0.0;
float v2 = 0.0;
float v3 = 0.0;

void setup() {                                        //set up the hardware
  Serial.begin(9600);                                 //set baud rate for the hardware serial port_0 to 9600
  myserial.begin(9600);                               //set baud rate for the software serial port to 9600
  myserial1.begin(9600); 
  inputstring.reserve(10);                            //set aside some bytes for receiving data from the PC
  sensorstring.reserve(30);
  inputstring1.reserve(10);                            //set aside some bytes for receiving data from the PC
  sensorstring1.reserve(30);
  ads.begin();
}


void serialEvent() {                                  //if the hardware serial port_0 receives a char
  inputstring = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete = true;                       //set the flag used to tell if we have received a completed string from the PC
}
void serialEvent1() {                                  //if the hardware serial port_0 receives a char
  inputstring1 = Serial.readStringUntil(13);           //read the string until we see a <CR>
  input_string_complete1 = true;                       //set the flag used to tell if we have received a completed string from the PC
}



void loop() {                                         //here we go...
  //analog
  Serial.println("----\nAnalog");
  int16_t adc0,adc1,adc2,adc3;  // we read from the ADC, we have a sixteen bit integer as a result

  adc0 = ads.readADC_SingleEnded(0);
  adc1 = ads.readADC_SingleEnded(1);
  adc2 = ads.readADC_SingleEnded(2);
  adc3 = ads.readADC_SingleEnded(3);
  v0 = (adc0 * 0.1875)/1000;
  v1 = (adc1 * 0.1875)/1000;
  v2 = (adc2 * 0.1875)/1000;
  v3 = (adc3 * 0.1875)/1000;
  
  Serial.print("AIN0: "); 
  Serial.print(adc0);
  Serial.print("\tVoltage: ");
  Serial.println(v0);  
  Serial.println();

  Serial.print("AIN1: "); 
  Serial.print(adc1);
  Serial.print("\tVoltage: ");
  Serial.println(v1);  
  Serial.println();

  Serial.print("AIN2: "); 
  Serial.print(adc2);
  Serial.print("\tVoltage: ");
  Serial.println(v2);  
  Serial.println();

  Serial.print("AIN3: "); 
  Serial.print(adc3);
  Serial.print("\tVoltage: ");
  Serial.println(v3);  
  Serial.println();
  
  delay(1000);

  //digital
  Serial.println("Digital");
  if (input_string_complete) {                        //if a string from the PC has been received in its entirety
    myserial.print(inputstring);                      //send that string to the Atlas Scientific product
    myserial.print('\r');                             //add a <CR> to the end of the string
    inputstring = "";                                 //clear the string
    input_string_complete = false;                    //reset the flag used to tell if we have received a completed string from the PC
  }
  delay(3000);
  if (input_string_complete1) {                        //if a string from the PC has been received in its entirety
    myserial1.print(inputstring1);                      //send that string to the Atlas Scientific product
    myserial1.print('\r');                             //add a <CR> to the end of the string
    inputstring1 = "";                                 //clear the string
    input_string_complete1 = false;                    //reset the flag used to tell if we have received a completed string from the PC
  }
  delay(3000);
  if (myserial.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)myserial.read();              //get the char we just received
    sensorstring += inchar;                           //add the char to the var called sensorstring
    if (inchar == '\r') {                             //if the incoming character is a <CR>
      sensor_string_complete = true;                  //set the flag
    }
  }
  if (myserial1.available() > 0) {                     //if we see that the Atlas Scientific product has sent a character
    char inchar = (char)myserial1.read();              //get the char we just received
    sensorstring1 += inchar;                           //add the char to the var called sensorstring
    if (inchar == '\r') {                             //if the incoming character is a <CR>
      sensor_string_complete1 = true;                  //set the flag
    }
  }

  if (sensor_string_complete == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    Serial.print("EC");
//    Serial.println(sensorstring);
    print_EC_data();
    sensorstring = "";                                //clear the string
    sensor_string_complete = false;  
  }
  delay(3000);
  if (sensor_string_complete1 == true) {               //if a string from the Atlas Scientific product has been received in its entirety
    Serial.print("DO:");
    Serial.println(sensorstring1);
//    print_EC_data();
    sensorstring1 = "";                                //clear the string
    sensor_string_complete1 = false;  
  }
  delay(3000);

//  Serial.print(" Requesting temperatures..."); 
  sensors.requestTemperatures(); // Send the command to get temperature readings 
  Serial.print("Temperature is: ");
  Serial.println(sensors.getTempCByIndex(0)); // Why "byIndex"?  
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
  Serial.println();
}



void print_EC_data(void) {                            //this function will pars the string  

  char sensorstring_array[30];                        //we make a char array
  char *EC;                                           //char pointer used in string parsing
  char *TDS;                                          //char pointer used in string parsing
  char *SAL;                                          //char pointer used in string parsing
  char *GRAV;                                         //char pointer used in string parsing
  float f_ec;                                         //used to hold a floating point number that is the EC
  
  sensorstring.toCharArray(sensorstring_array, 30);   //convert the string to a char array 
  EC = strtok(sensorstring_array, ",");               //let's pars the array at each comma
  TDS = strtok(NULL, ",");                            //let's pars the array at each comma
  SAL = strtok(NULL, ",");                            //let's pars the array at each comma
  GRAV = strtok(NULL, ",");                           //let's pars the array at each comma

  Serial.print("EC:");                                //we now print each value we parsed separately
  Serial.println(EC);                                 //this is the EC value

  Serial.print("TDS:");                               //we now print each value we parsed separately
  Serial.println(TDS);                                //this is the TDS value

  Serial.print("SAL:");                               //we now print each value we parsed separately
  Serial.println(SAL);                                //this is the salinity value

  Serial.print("GRAV:");                              //we now print each value we parsed separately
  Serial.println(GRAV);                               //this is the specific gravity
  Serial.println();                                   //this just makes the output easier to read
  
//f_ec= atof(EC);                                     //uncomment this line to convert the char to a float
}

#include <math.h>
#include <Wire.h>
#include <MPU6050.h>
#include <TinyGPS++.h>
 #include <SoftwareSerial.h>
SoftwareSerial mySerial(10,9);
int i = 0;
int tem=0;
static const uint32_t GPSBaud = 9600;
// The TinyGPS++ object
TinyGPSPlus gps;
// The serial connection to the GPS device
SoftwareSerial ss(5, 6);


MPU6050 mpu;

int sensorPin = A0; // select the input pin for the temperature sensor potentiometer

int led = 13;                // the pin that the LED is atteched to
int sensor = 4;                //the pin PIR sensor is attached to
int analogPin = 3;     // accident sensor (middle terminal) connected to analog pin 3
int state = LOW;             // by default, no motion detected
int val = 0;               // variable to store the sensor status (value)
int a=0;
int b=0;
char str[15];

   const int Aoutpin=A1;  //accel inpit
const int Doutpin=3;      //accel input
const int ledpin=8;
int value;
int value2;
float temp;

 
double Thermistor(int RawADC) {
  double Temp;
  Temp = log(10000.0*((1024.0/RawADC-1))); 
  Temp = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * Temp * Temp ))* Temp );
  Temp = Temp - 273.15;            // Convert Kelvin to Celcius
   //Temp = (Temp * 9.0)/ 5.0 + 32.0; // Convert Celcius to Fahrenheit
   return Temp;
   

   
}
 
void setup() {
  
  Serial.begin(9600);
  //ss.begin(GPSBaud);
  Serial.println(F("A simple demonstration of TinyGPS++ with an attached GPS module"));
   pinMode(led, OUTPUT);      // initalize LED as an output
  pinMode(sensor, INPUT);    // initialize sensor as an input
  mySerial.begin(9600);       // baud rate for gsm module delay(100);

  Serial.println("Initialize MPU6050");

  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
  {
    Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
    delay(500);
  }
   pinMode(Doutpin, INPUT);
 pinMode(Aoutpin, INPUT);
 pinMode(ledpin,OUTPUT);
 
}


 
void loop() {
  
// This sketch displays information every time a new sentence is correctly encoded.
  while (Serial.available() > 0)
    if (gps.encode(Serial.read()))
   {
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  

  Serial.println();
   }

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
  
  
 Serial.println(" ");
 
 
 
 int readVal=analogRead(sensorPin);
 temp =  Thermistor(readVal);
 Serial.print("temperature in celcius== " );
 Serial.println(temp);  // display tempature
 //Serial.println(readVal);  // display tempature
 
 
 
  Vector normAccel = mpu.readNormalizeAccel();
  Serial.print(" Xnorm = ");
  Serial.print(normAccel.XAxis);
  Serial.print(" Ynorm = ");
  Serial.print(normAccel.YAxis);
  Serial.print(" Znorm = ");
  Serial.println(normAccel.ZAxis);
  if ( normAccel.XAxis<7 || normAccel.YAxis>7 )
  {
  Serial.println("Vehicle has tilted");
  }
  else
  {
    Serial.println("Vehicle is normal(not tilted)");
  }


value=analogRead(Aoutpin);
value=value+200;
Serial.print("Carbon Dioxide in air is ");
Serial.print(value);
Serial.println("ppm");
if(700<=value){
  Serial.println("Oxygen percentage is below required");
  digitalWrite(ledpin,HIGH);
  
}
else{Serial.println("Oxygen pecentage is ok");
digitalWrite(ledpin,LOW);
}



  val = digitalRead(sensor);   // read sensor value
  if (val == HIGH) {           // check if the sensor is HIGH
    digitalWrite(led, HIGH);   // turn LED ON
       if (state == LOW) {
               Serial.println("Motion detected!"); 
      a=1;
      state = HIGH;       // update variable state to HIGH
    }
  } 
  else {
      digitalWrite(led, LOW); // turn LED OFF
     
      if (state == HIGH){
        Serial.println("Motion not detected!");
        state = LOW;       // update variable state to LOW
        a=0;
    }
  }
  
  
     val = analogRead(analogPin);     // read the input pin
     Serial.println("ACC sensor value");
      Serial.println(val);        // debug value


  if (val>500) 
  {           // check if the sensor is HIGH
       Serial.println("Accident detected!"); 
        mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
       delay(1000);  // Delay of 1000 milli seconds or 1 second
        mySerial.println("AT+CMGS=\"+9779847473444\"\r"); // Replace x with mobile number
  delay(1000);  
 String dataMessage = ("Accident Occured");
   mySerial.println(dataMessage);
   
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(3000);
  }  
  
  
  if(temp>50 || value>=600 || a==1 || normAccel.XAxis<7 || normAccel.YAxis>7|| val>500 )
  {
    b=1;
  }
  
  
  
  
 switch (b) {
    case 1:
      //do something when var equals 1
      b=0;
                 
              if(value>=600 && a==1 && (normAccel.XAxis<7 || normAccel.YAxis>7))
              {
               SendMessage1();
              }
              
              else if(value>=600 && a==1  && (normAccel.XAxis>7 || normAccel.YAxis<7))
              {
               SendMessage2();
              }
             
              else if(value>=600 && a==0  && (normAccel.XAxis<7 || normAccel.YAxis>7))
              {
              SendMessage3();
              }
              
              else if(value>=600 && a==0 && (normAccel.XAxis>7 || normAccel.YAxis<7))
              {
               SendMessage4();
              }  
              
              else if(value<600 && a==1  && (normAccel.XAxis<7 || normAccel.YAxis>7))
              {
                SendMessage5();           
              }
              
              else if(value<600 && a==1  && (normAccel.XAxis>7 || normAccel.YAxis<7))
              {
               SendMessage6();
              }
       
              else if(value<600 && a==0 && (normAccel.XAxis<7 || normAccel.YAxis>7))
              {
              SendMessage7();
              }
              
              else if(value<600 && a==0 && (normAccel.XAxis>7 || normAccel.YAxis<7))
              {
               SendMessage8();
              }
              
  
      break;
          default:
      // if nothing else matches, do the default
      // default is optional
         break;
                   
  }
      


delay(3000);

}




 void SendMessage1()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+9779847473444\"\r"); // Replace x with mobile number
  delay(1000);
  
 String dataMessage = ("Temperature is: " + String(temp)+"Carbon Dioxide in air is: "  +"ppm "+ String(value)+"Oxygen percentage is below required "+"Vehicle has tilted "+"Motion detected!"+ String(gps.location.lat(), 6 )
 +String (gps.location.lng(), 6));
   
   mySerial.println(dataMessage);
   
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(3000);
}


 void SendMessage2()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+9779847473444\"\r"); // Replace x with mobile number
  delay(1000);
  
 String dataMessage = ("Temperature is: " + String(temp)+"Carbon Dioxide in air is: "  +"ppm "+  String(value)+"Oxygen percentage is below required "+"Vehicle is normal(not tilted) "+"Motion detected!"+ String(gps.location.lat(), 6 )
 +String (gps.location.lng(), 6));
   mySerial.println(dataMessage);
   
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(3000);
}

 void SendMessage3()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+9779847473444\"\r"); // Replace x with mobile number
  delay(1000);
  
 String dataMessage = ("Temperature is: " + String(temp)+"Carbon Dioxide in air is: "  +"ppm "+ String(value)+"Oxygen percentage is below required "+"Vehicle has tilted "+"Motion not detected!"+ String(gps.location.lat(), 6 )
 +String (gps.location.lng(), 6));
   mySerial.println(dataMessage);
   
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(3000);
}

 void SendMessage4()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+9779847473444\"\r"); // Replace x with mobile number
  delay(1000);
  
 String dataMessage = ("Temperature is: " + String(temp)+"Carbon Dioxide in air is: " +"ppm " + String(value)+"Oxygen percentage is below required "+"Vehicle is normal(not tilted) "+"Motion not detected!"+ String(gps.location.lat(), 6 )
 +String (gps.location.lng(), 6));
   mySerial.println(dataMessage);
   
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(3000);
}

 void SendMessage5()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+9779847473444\"\r"); // Replace x with mobile number
  delay(1000);
  
 String dataMessage = ("Temperature is: " + String(temp)+"Carbon Dioxide in air is: "  +"ppm "+ String(value)+"Oxygen percentage is ok "+"Vehicle has tilted "+"Motion detected!"+ String(gps.location.lat(), 6 )
 +String (gps.location.lng(), 6));
   mySerial.println(dataMessage);
   
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(3000);
}

 void SendMessage6()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+9779847473444\"\r"); // Replace x with mobile number
  delay(1000);
  
 String dataMessage = ("Temperature is: " + String(temp)+"Carbon Dioxide in air is: " +"ppm "+ String(value)+"Oxygen percentage is ok "+"Vehicle is normal(not tilted) "+"Motion detected!"+ String(gps.location.lat(), 6 )
 +String (gps.location.lng(), 6));
   mySerial.println(dataMessage);
   
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(3000);
}

 void SendMessage7()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+9779847473444\"\r"); // Replace x with mobile number
  delay(1000);
  
 String dataMessage = ("Temperature is: " + String(temp)+"Carbon Dioxide in air is: " +"ppm " + String(value)+"Oxygen percentage is ok "+"Vehicle has tilted "+"Motion not detected!"+ String(gps.location.lat(), 6 )
 +String (gps.location.lng(), 6));
   mySerial.println(dataMessage);
   
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(3000);
}

 void SendMessage8()
{
  mySerial.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);  // Delay of 1000 milli seconds or 1 second
  mySerial.println("AT+CMGS=\"+9779847473444\"\r"); // Replace x with mobile number
  delay(1000);
  
String dataMessage = ("Temperature is: " + String(temp)+"Carbon Dioxide in air is: "  +"ppm "+ String(value)+"Oxygen percentage is ok "+"Vehicle is normal(not tilted) "+"Motion not detected!"+ String(gps.location.lat(), 6 )
 +String (gps.location.lng(), 6));
   mySerial.println(dataMessage);
   
  delay(100);
   mySerial.println((char)26);// ASCII code of CTRL+Z
  delay(3000);
}

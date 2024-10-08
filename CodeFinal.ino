//**************** Home security Program - offline ******************************

 
#define PIR_MOTION_SENSOR 7   // Use pin 2 to receive the signal from the module

#define INFRARED_SENSOR 3
 
 
 
#define LED1 4                // Blue Led for motion detected
 
#define LED2 6                // Green Led for triggered alarm
 
#define LED3 8                // Red Led if motion and trigger switch are on
 
 
 
#define buzzer 5              // Buzzer  
 
 
#include <Wire.h>
#include <SoftwareSerial.h>

SoftwareSerial sim(10, 11);
 
int tempout_pin = A2;
 

int _timeout;
String _buffer;


String number = "+84367205630"; //-> so dien thoai nguoi nhan (Chu nha)
String val;

 
unsigned long previousMillis = 0;
 
void setup()
 
{
 
 Serial.begin(9600);
 
 while (!Serial);
 
 
 
 
 
 while (!Serial);
 
 Wire.begin();
 
 
 
 
 pinMode(LED1,OUTPUT);
 
 pinMode(LED2,OUTPUT);
 
 pinMode(LED3,OUTPUT);
 
 pinMode(buzzer, OUTPUT);
 
 pinMode(PIR_MOTION_SENSOR, INPUT);

 pinMode(INFRARED_SENSOR, INPUT);

 delay(5000); //delay for 5 seconds to make sure the modules get the signal

 _buffer.reserve(50);
 Serial.println("Sistem Started...");
 sim.begin(9600);
 delay(1000);
 
}
 
// ********************* Start Loop *****************************************
 
void loop()
 
{
   RecieveMessage();  // Doc tin nhan tu so dien thoai gui toi
 
   Serial.println();

 
 
   // ********************** Cảm biến PIR phát hiện chuyển động ***********************
 
   
   // ********************* Phat hien dot nhap bat hop phap *******************
      if(isInfrared() & isPIRDetected())                               //if it detects the moving people?
 
      {
     
          delay(10);           
          digitalWrite(LED3,HIGH);
          int check = 1;
          callNumber();   //Goi dien thoai den nguoi nhan
          delay(500);
          SendMessage();  //Gui tin nhan thong bao den so dien thoai nhan
          delay(500);
          while(check == 1){
            triggerBuzzer(10,500,100);  //Set coi bao dong Bat 0.5s, Tat 0.1s, lap lai 10 lan
            if(sim.available()>0){
                val = sim.readStringUntil('\n');
                Serial.println(val);
                if(val=="Tat\r"){
                  check = 0;
                   digitalWrite(buzzer, LOW);
                    SendMessageFeedback();
                   Serial.println("Da tat bao dong.");
                }
                
           }
     
            Serial.println("Phat hien mot xam nhap bat hop phap.");
          }
      }
 
   else
 
      { if(isPIRDetected())                               //Neu cam bien PIR phat hien
 
      {
 
           digitalWrite(LED1, HIGH);                        // Bat den xanh duong
     
          delay(500);

      }
 
   else
 
      { digitalWrite(LED1, LOW);
 
      //  trigger = 0;
 
       delay(500);
 
      }

      // ********************** Cảm biến hồng ngoại phát hiện chuyển động ***********************
 
   if(isInfrared())                             
 
      {
           digitalWrite(LED2, HIGH);                        // Bat den xanh la
     
          delay(500);
     
      }
 
   else
 
      { digitalWrite(LED2, LOW);
 
     //   trigger = 0;
 
       delay(500);
 
      }
 
      } 

 
      delay(10);
 
}
 
// **************************  End vòng lặp Loop *****************************






// ************************* Hàm gửi tin nhan thong bao phat hien dot nhap************************
 void SendMessage()
{
  sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
  sim.println("AT+CMGS=\"" + number + "\"\r"); //number la so dien thai nguoi nhan da define o tren
  delay(1000);
  String SMS = "Phat hien mot xam nhap bat hop phap !";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  Serial.println ("Da gui tin nhan thong bao den nguoi nhan");
  delay(1000);
  _buffer = _readSerial();
}




// ************************* Hàm gửi tin nhan thong bao da tat bao dong************************
 void SendMessageFeedback()
{
  //Serial.println ("Sending Message");
  sim.println("AT+CMGF=1");    //Sets the GSM Module in Text Mode
  delay(1000);
//  Serial.println ("Set SMS Number");
  sim.println("AT+CMGS=\"" + number + "\"\r"); //number la so dien thai nguoi nhan da define o tren
  delay(1000);
  String SMS = "Da tat bao dong";
  sim.println(SMS);
  delay(100);
  sim.println((char)26);// ASCII code of CTRL+Z
  Serial.println ("Da gui tin nhan thong bao den nguoi nhan");
  delay(1000);
  _buffer = _readSerial();
}




// ************************* Hàm nhận tin nhan ************************
void RecieveMessage()
{
  Serial.println ("SIM800L Read an SMS");
  delay (1000);
  sim.println("AT+CNMI=2,2,0,0,0"); // AT Command to receive a live SMS
  delay(1000);
  Serial.write ("Unread Message done");
}
String _readSerial() {
  _timeout = 0;
  while  (!sim.available() && _timeout < 12000  )
  {
    delay(13);
    _timeout++;
  }
  if (sim.available()) {
    return sim.readString();
  }
}

// ************************* Hàm goi dien thoai ************************
void callNumber() {
  sim.print (F("ATD"));
  sim.print (number);
  sim.print (F(";\r\n"));
  _buffer = _readSerial();
  Serial.println(_buffer);
}
 
 
 
 
//***************************************************************
 
// Ham kiem tra du lieu dau vao cua Hong ngoai
 
boolean isInfrared()
 
{
 
   int sensorValue = digitalRead(INFRARED_SENSOR);
 
   if(sensorValue == LOW)                           //Neu gia tri sensor Hong ngoai = LOW ?
 
      {
 
       Serial.println("Phat hien vat cam tai vi tri hong ngoai !!!");
 
       return true;                                  //yes,return true
 
      }
 
   else
 
      {
 
       Serial.println("Khong co vat can tai vi tri hong ngoai");     
 
       return false;                                 //no,return false
 
      }
 
}
// Ham kiem tra du lieu dau vao cua PIR
 
boolean isPIRDetected()
 
{
 
   int sensorValue = digitalRead(PIR_MOTION_SENSOR);
 
   if(sensorValue == HIGH)                           //Neu gia tri sensor = HIGH ?
 
      {
 
       Serial.println("Phat hien mot chuyen dong nhiet thong qua PIR");
 
       return true;                                  //yes,return true
 
      }
 
   else
 
      {
 
       Serial.println("Khong phat hien chuyen dong.");     
 
       return false;                                 //no,return false
 
      }
 
}
 
 
//******************** Hàm thiet ke chuong bao dong theo chu ki *******************************8
void triggerBuzzer(int iteration,int delay1,int delay2)
 
{
 
     for (int i = 1; i < iteration; i++)
 
     {
 
      digitalWrite(buzzer, HIGH);     // Bat coi voi thoi gian delay 1
 
      delay(delay1);
 
      digitalWrite(buzzer, LOW);      // Tat coi voi thoi gian delay 2
 
      delay(delay2);
 
     }
 
}
 
//*****************************************************************

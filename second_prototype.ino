#include <AFMotor.h>
#define DEBUG true

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);

const int CONST = 255;

String dataIN = "";
String dataOUT = "";
String ip = "";

const int buzzer = 52;

void setup() {
  
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  
  Serial.begin(9600);
  Serial1.begin(115200);
  Serial.println("Start");
  esp8266Serial("AT\r\n", 3000, DEBUG);
  esp8266Serial("AT+CWQAP\r\n", 3000, DEBUG);
  esp8266Serial("AT+RST\r\n", 3000, DEBUG); // Reset the ESP8266
  Serial.println("Resettato");
  esp8266Serial("AT+CWMODE=1\r\n", 3000, DEBUG); //Set station mode Operation
  Serial.println("Station Mode ON");
  esp8266Serial("AT+CWJAP=\"Camionetta NSA\",\"12348765\"\r\n", 3000, DEBUG);//Enter your WiFi network's SSID and Password.
  Serial.println("Waiting for connection..");                                  
  while(Serial1.find("OK"))
  {
  }
  Serial.println("Connection ok");
  esp8266Serial("AT+CIFSR\r\n", 3000, DEBUG);//You will get the IP Address of the ESP8266 from this command
  Serial.println("Get IP");
  esp8266Serial("AT+CIPMUX=1\r\n", 3000, DEBUG);
  esp8266Serial("AT+CIPSERVER=1,80\r\n", 3000, DEBUG);

  motor1.setSpeed(CONST);  // Set motor 1 to maximum speed
  motor2.setSpeed(CONST);  // Set motor 2 to maximum speed
  motor3.setSpeed(CONST);  // Set motor 3 to maximum speed
  motor4.setSpeed(CONST);  // Set motor 4 to maximum speed
  
  tone(buzzer, 250, 75);
}

void loop() {
  char c;

  if (Serial.available()) {
    c = Serial.read();
    dataIN += c;
    if(c=='\n'){
      Serial1.print(dataIN);
      dataIN = "";
    }
  }
  if (Serial1.available()) {
    c = Serial1.read();
    dataOUT += c;
    if(c=='\n'){
      if(dataOUT.substring(9) == "On\r\n" || dataOUT == "On\r\n"){
        Serial.println("ON TROVATO");
        motor1.run(FORWARD);
        motor2.run(FORWARD);
        motor3.run(FORWARD);
        motor4.run(FORWARD);
      }else if(dataOUT.substring(9) == "Left\r\n" || dataOUT == "Left\r\n"){
        Serial.println("LEFT TROVATO");
        motor3.run(FORWARD);
        motor4.run(FORWARD);
      }else if(dataOUT.substring(9) == "Right\r\n" || dataOUT == "Right\r\n"){
        Serial.println("RIGHT TROVATO");
        motor1.run(FORWARD);
        motor2.run(FORWARD);
      }else if(dataOUT.substring(9) == "Behind\r\n" || dataOUT == "Behind\r\n"){
        Serial.println("BEHIND TROVATO");
        motor1.run(BACKWARD);
        motor2.run(BACKWARD);
        motor3.run(BACKWARD);
        motor4.run(BACKWARD);
      }else if(dataOUT.substring(9) == "Shoot\r\n" || dataOUT == "Shoot\r\n"){
        Serial.println("SHOOT TROVATO");
          tone(buzzer, 250, 75);
        /*analogWrite(ledPin5, 255);
        tone(buzzer, 250, 75);
        delay(100);
        analogWrite(ledPin5, 0);*/
      }else if(dataOUT.substring(9) == "STOP\r\n" || dataOUT == "STOP\r\n"){
        Serial.println("STOP TROVATO");
        motor1.run(RELEASE);
        motor2.run(RELEASE);
        motor3.run(RELEASE);
        motor4.run(RELEASE);    
      }    
      Serial.print(dataOUT);
      dataOUT = "";
    }
  }
}

String esp8266Serial(String command, const int timeout, boolean debug)
  {
    String response = "";
    Serial1.print(command);
    long int time = millis();
    while ( (time + timeout) > millis())
      {
        while (Serial1.available())
          {
            char c = Serial1.read();
            response += c;
          }
      }
      if(debug){
          Serial.println(response);
      }
    return response;
}

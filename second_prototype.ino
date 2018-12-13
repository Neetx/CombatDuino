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
String state = "0000";


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
  
  maxSpeed();
  
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
      String s = extractAngle(dataOUT);
      if(s != "False"){
        int angle = s.toInt();
        //Serial.print(angle);
        if(angle>=80 && angle<=100){
          Serial.print("AVANTI\n");
          if(state != "0000"){
            stop();
            maxSpeed();
          }
          state = "0000";
          forward();
        }else if(angle<=10 && angle>=0) {
          Serial.print("DESTRA\n");
          if(state != "0010"){
            stop();
            maxSpeed();
          }
          state = "0010";
          motor1.run(FORWARD);
          motor2.run(FORWARD);
        }else if(angle>=170 && angle<=180){
          Serial.print("SINISTRA\n");
          if(state != "0011"){
            stop();
            maxSpeed();
          }          
          state = "0011";
          motor3.run(FORWARD);
          motor4.run(FORWARD);
        }else if(angle>=-100 && angle<=-80){
          Serial.print("INDIETRO\n");
          if(state != "0100"){
            stop();
            maxSpeed();
          }
          state = "0100";
          backward();
        }else if(angle>10 && angle<=45){
          Serial.print("PRIMO ALTO\n");
          if(state != "0101"){
            stop();
            maxSpeed();
          }
          state = "0101";
          forward();
          delay(60);
          motor1.setSpeed(CONST);
          motor2.setSpeed(CONST);
          motor3.setSpeed(50);
          motor4.setSpeed(50);
          forward();
        }else if(angle>45 && angle<80){
          Serial.print("PRIMO BASSO\n");
          if(state != "0110"){
            stop();
            maxSpeed();
          }
          state = "0110";
          forward();
          delay(60);
          motor1.setSpeed(CONST);
          motor2.setSpeed(CONST);
          motor3.setSpeed(80);
          motor4.setSpeed(80);
          forward();
        }else if(angle<-10 && angle>=-45){
          Serial.print("4o ALTO\n");
          if(state != "0111"){
            stop();
            maxSpeed();
          }
          state = "0111";
          backward();
          delay(60);
          motor1.setSpeed(CONST);
          motor2.setSpeed(CONST);
          motor3.setSpeed(50);
          motor4.setSpeed(50);
          backward();
        }else if(angle<-45 && angle>-80){
          Serial.print("4o BASSO\n");
          if(state != "1000"){
            stop();
            maxSpeed();
          }
          state = "1000";
          backward();
          delay(60);
          motor1.setSpeed(CONST);
          motor2.setSpeed(CONST);
          motor3.setSpeed(80);
          motor4.setSpeed(80);
          backward();
        }else if(angle>100 && angle<=135){
          Serial.print("2o ALTO\n");
          if(state != "1001"){
            stop();
            maxSpeed();
          }
          state = "1001";
          forward();
          delay(60);
          motor1.setSpeed(80);
          motor2.setSpeed(80);
          motor3.setSpeed(CONST);
          motor4.setSpeed(CONST);
          forward();
        }else if(angle>135 && angle<170){
          Serial.print("2o BASSO\n");
          if(state != "1010"){
            stop();
            maxSpeed();
          }
          state = "1010";
          forward();
          delay(60);
          motor1.setSpeed(50);
          motor2.setSpeed(50);
          motor3.setSpeed(CONST);
          motor4.setSpeed(CONST);
          forward();
        }else if(angle<-100 && angle>=-135){
          Serial.print("3o ALTO\n");
          if(state != "1011"){
            stop();
            maxSpeed();
          }
          state = "1011";
          backward();
          delay(60);
          motor1.setSpeed(80);
          motor2.setSpeed(80);
          motor3.setSpeed(CONST);
          motor4.setSpeed(CONST);
          backward();
        }else if(angle<-135 && angle>-170){
          Serial.print("3o BASSO\n");
          if(state != "1100"){
            stop();
            maxSpeed();
          }
          state = "1100";
          backward();
          delay(60);
          motor1.setSpeed(50);
          motor2.setSpeed(50);
          motor3.setSpeed(CONST);
          motor4.setSpeed(CONST);
          backward();
        }else if(angle>=-10 && angle<0){
          Serial.print("DESTRA DIETRO\n");
          if(state != "1101"){
            stop();
            maxSpeed();
          }
          state = "1101";
          motor1.run(BACKWARD);
          motor2.run(BACKWARD);
        }else if(angle>=-180 && angle<=-170){
          Serial.print("SINISTRA DIETRO\n");
          if(state != "1110"){
            stop();
            maxSpeed();
          }
          state = "1110";
          motor3.run(BACKWARD);
          motor4.run(BACKWARD);
        }
        //Serial.print(dataOUT);
        //Serial.print(extractRadius(dataOUT)+"\n");
        //Serial.print(extractAngle(dataOUT).toFloat());
        //dataOUT = "";
      } else {
        if(dataOUT.substring(9) == "Shoot\r\n" || dataOUT == "Shoot\r\n"){
          Serial.println("SHOOT TROVATO");
          tone(buzzer, 250, 75);
          /*analogWrite(ledPin5, 255);
          tone(buzzer, 250, 75);
          delay(100);
          analogWrite(ledPin5, 0);*/
        }else if(dataOUT.substring(9) == "STOP\r\n" || dataOUT == "STOP\r\n"){
          Serial.println("STOP TROVATO");
          stop();
        }else if(dataOUT.substring(9) == "Check\r\n" || dataOUT == "Check\r\n"){
          Serial.println("Check TROVATO");
          esp8266Serial("AT+CIPSEND=0,4\r\n", 2, DEBUG);
          while(Serial1.find(">"))
          {
          }
          esp8266Serial("ACK\n\r\n", 2, DEBUG);
        }
      }
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

void stop(){
  motor1.run(RELEASE);
  motor2.run(RELEASE);
  motor3.run(RELEASE);
  motor4.run(RELEASE);
}

void maxSpeed(){
  motor1.setSpeed(CONST);
  motor2.setSpeed(CONST);
  motor3.setSpeed(CONST);
  motor4.setSpeed(CONST);
}

void forward(){
  motor1.run(FORWARD);
  motor2.run(FORWARD);
  motor3.run(FORWARD);
  motor4.run(FORWARD);
}

void backward(){
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}
String extractRadius(String str){
  unsigned int r = str.indexOf("R");
  if(r==65535){
    return "False";
  }
  return str.substring(str.indexOf("R")+1, str.indexOf("A"));
}

String extractAngle(String str){
  unsigned int a = str.indexOf("A");
  if(a==65535){
    return "False";
  }
  return str.substring(str.indexOf("A")+1, str.indexOf("\r"));
}

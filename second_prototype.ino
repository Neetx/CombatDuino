//#pragma GCC optimize ("-O3")
//#pragma GCC push_options

#include <AFMotor.h>
#include <ArduinoJson.h>
#include <GCM.h>
#include <AES.h>
#include <Crypto.h>

#define DEBUG true

//start crypto section
#define MAX_PLAINTEXT_LEN 128

byte key[32] = {0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08,0xfe, 0xff, 0xe9, 0x92, 0x86, 0x65, 0x73, 0x1c,0x6d, 0x6a, 0x8f, 0x94, 0x67, 0x30, 0x83, 0x08};
byte iv[12];//      = {0xca, 0xfe, 0xba, 0xbe, 0xfa, 0xce, 0xdb, 0xad,0xde, 0xca, 0xf8, 0x88};
byte plaintext[2];
byte ciphertext[MAX_PLAINTEXT_LEN];

GCM<AES256> *cipher = 0;

byte buffer[256];
size_t ivsize = 12;
size_t posn, len;
size_t inc = 16;


//end crypto section

AF_DCMotor motor1(1, MOTOR12_1KHZ);
AF_DCMotor motor2(2, MOTOR12_1KHZ);
AF_DCMotor motor3(3, MOTOR34_1KHZ);
AF_DCMotor motor4(4, MOTOR34_1KHZ);

int x = 0;

const int CONST = 255;

String dataIN = "";
byte dataOUT[256];
byte tmp[256];
String ip = "";
String state = "0000";

int index = 0;
int readed = 0;

const int buzzer = 52;

void setup() {
  
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  
  Serial.begin(9600);
  Serial1.begin(115200);
  //Serial1.setTimeout(5);
  //Serial.setTimeout(5);
  
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

  Serial1.setTimeout(10);
  Serial.setTimeout(5);
  
  maxSpeed();

  cipher = new GCM<AES256>();
  crypto_feed_watchdog();
  cipher->clear();
  if (!cipher->setKey(key, cipher->keySize())) {
      Serial.print("setKey ");
  }
  //removed set iv
  memset(buffer, 0xFF, sizeof(buffer));
  
  tone(buzzer, 250, 75);
}

void loop() {
  char c;
  byte b;
  if (Serial.available()) {
    c = Serial.read();
    dataIN += c;
    if(c=='\n'){
      Serial1.print(dataIN);
      dataIN = "";
    }
  }
  Serial1.flush();
  while(Serial1.available()>0 && readed == 0){
    //delay(3);
    b = Serial1.read();
    dataOUT[index] = b;
    index++;
    if(b=='\n' && dataOUT[index-2]=='\r' && dataOUT[index-3]=='>' && dataOUT[index-4]=='>'){
    //if(index>=86){
    index = 0;
    //dataOUT = Serial1.readString();
      readed = 1;
    }
  }  
   if(readed == 1){
   /*if(Serial1.available()){
    c = Serial1.read();
    dataOUT += c;
    index++;
    if(c=='\n' && dataOUT[index-2]=='\r' && dataOUT[index-3]=='>' && dataOUT[index-4]=='>'){
    Serial1.flush();*/
    readed = 0;
    //index = 0;    
//crypto section
  int i=12;
  int j=0;
  int fl=0;
  while(i<256 && dataOUT[i] !='\r' && dataOUT[i-1]!='>' && dataOUT[i-2]!='>'){
    /*Serial.print("*");
    if(dataOUT[i]==':'){
      Serial.println("Found");
      fl=1;
    }*/
    //if(fl==1){
      //Serial.print("-");
      tmp[j]=dataOUT[i];
      j++;
    //}
    i++;
  }
  int datasize = j-1;
  Serial.println();
  //size_t datasize = dataOUT.length();
  if(datasize==30){
  /*Serial.println("Size");
  Serial.println(datasize);
  Serial.println("DATAOUT BYTE");
  for(int i = 0; i< 50; i++){
    Serial.print(dataOUT[i],HEX);
  }
  Serial.println("\nTEMP BYTE");
  for(int i = 0; i< datasize; i++){
    Serial.print(tmp[i],HEX);
  }
  Serial.println();*/
  int j=0;
  for(int i=ivsize;i<datasize-16;i++){
    ciphertext[j]=tmp[i];
    j++;
  }
  for(int i=0;i<ivsize;i++){
    iv[i]=tmp[i];
  }
  int cryptsize = j;
  Serial.println("\nCIPHERTEXT BYTE");
  for(int i = 0; i< cryptsize; i++){
    Serial.print(ciphertext[i],HEX);
  }
  Serial.println();
  if (!cipher->setIV(iv, ivsize)) {
      Serial.print("setIV ");
  } 
   for (posn = 0; posn < cryptsize ; posn += inc) {
      len = cryptsize - posn;
      if (len > inc)
          len = inc;
      cipher->decrypt(buffer + posn, ciphertext + posn, len);
   }
  for(int i=0;i<sizeof(plaintext);i++){
    plaintext[i] = buffer[i];
  }
  Serial.println("Plaintext");
  for(int i = 0; i< sizeof(plaintext)/sizeof(byte); i++){
    Serial.print(plaintext[i],HEX);
  }
  Serial.println("\nCommand");
  Serial.write(plaintext[0]);
  Serial.println("Params");
  //Serial.print((int8_t)plaintext[1],DEC);
  //Serial.println();

//end crypto
      /*String json = extractJson(dataOUT);
      if(!json.equals("False")){
        Serial.print(json);
        StaticJsonBuffer<200> jsonBuffer;
        JsonObject& root = jsonBuffer.parseObject(json);
        if(root.success()) {
          String cmd = root["CMD"];
          if(cmd != nullptr){
            Serial.print(cmd);
            Serial.print('\n');
            if(cmd.equals("Move")){
              const char* params = root["Params"];
              StaticJsonBuffer<60> jsonBuffer2;
              JsonObject& root2 = jsonBuffer2.parseObject(params);
              if(root2.success()){
                String a = root2["A"];
                if(a != nullptr){
                  //Serial.print(a);
                  int angle = a.toInt();
                  Serial.print(angle);*/
                 if(plaintext[0]==5){
                 int8_t angle1 = (int8_t)plaintext[1];
                                   Serial.println(angle1);

                 int angle = angle1*2;
                  Serial.println(angle);
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
                //}  
              //}else{Serial.print("ERR2");}           
            //} else if (cmd.equals("Shoot")){
              } else if (plaintext[0]==0){
              Serial.println("SHOOT TROVATO");
              tone(buzzer, 250, 75);
            //} else if (cmd.equals("Stop")){
            } else if (plaintext[0]==12){
              Serial.println("STOP TROVATO");
              stop();
            //} else if(cmd.equals("CHECK")){
            } else if (plaintext[0]==10){
              Serial.println("Check TROVATO");
                //String r = String(x) + "\n";
                String r = "{\"resp\":\""+String(x)+"\",\"params\":\"null\"}\n";
                esp8266Serial("AT+CIPSEND=0," + String(r.length()) + "\r\n", 2, DEBUG); //Todo: length parametrization
                while(Serial1.find(">"))
                {
                }
                esp8266Serial( r + "\r\n", 2, DEBUG);
                x++;
            }
            memset(buffer, 0xFF, sizeof(buffer));
            memset(ciphertext, 0x00, sizeof(ciphertext));
            memset(tmp, 0x00, sizeof(tmp));
          }
          memset(dataOUT, 0x00, sizeof(dataOUT));
       }
  }
          /*bool resp = root["Resp"];
          Serial.print(resp);
          if(resp == 1){
            //String r = String(x) + "\n";
            String r = "{\"resp\":\""+String(x)+"\",\"params\":\"null\"}\n";
            esp8266Serial("AT+CIPSEND=0," + String(r.length()) + "\r\n", 2, DEBUG); //Todo: length parametrization
            while(Serial1.find(">"))
            {
            }
            esp8266Serial( r + "\r\n", 2, DEBUG);
            x++;
          }
        }//else{Serial.print("ERR1");}
      }
    //dataOUT = "";
  memset(buffer, 0xFF, sizeof(buffer));
  memset(ciphertext, 0x00, sizeof(ciphertext));
  memset(dataOUT, 0x00, sizeof(dataOUT));
  memset(tmp, 0x00, sizeof(tmp));
    }
  }
}*/

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

void extractCrypto(byte str[], byte tmp[]){
  int i=10;
  while(i<256 && str[i]=='\n' && str[i-1] =='\r' && str[i-2]=='>' && str[i-3]=='>'){
    tmp[i]=str[i];
    i++;
  }
}

String extractJson(String str){
  unsigned int graph = str.indexOf("{");
  if(graph==65535){
    return "False";
  }
  return str.substring(str.indexOf("{"), str.indexOf("\r"));
}

//#pragma GCC pop_options

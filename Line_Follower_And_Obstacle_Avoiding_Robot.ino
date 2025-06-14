#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define enA 10
#define in1 9 
#define in2 8 
#define in3 7 
#define in4 6 
#define enB 5 
#define M_S 2

#define L_S A0 //ir sensor l
#define R_S A1 //ir sensor r
#define pressurePin A4

#define echo A2    
#define trigger A3 

#define servo A5


int Set=15;
int distance_L, distance_F, distance_R; 
int pressureInit = 0;
bool carStarted = false;
bool carPaused = false;

// nRF24L01 설정
RF24 radio(5, 6);
const byte address[6] = "00001";

void setup(){ 

Serial.begin(9600); 

pinMode(R_S, INPUT); 
pinMode(L_S, INPUT); 
pinMode(M_S, INPUT);

pinMode(echo, INPUT );
pinMode(trigger, OUTPUT);

pinMode(enA, OUTPUT); 
pinMode(in1, OUTPUT); 
pinMode(in2, OUTPUT);  
pinMode(in3, OUTPUT);    
pinMode(in4, OUTPUT);  
pinMode(enB, OUTPUT);  

pinMode(servo, OUTPUT);

pressureInit = analogRead(pressurePin);
Serial.print("Initial Pressure: ");
Serial.println(pressureInit);

// nRF24L01 초기화
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening(); // 송신 모드

 for (int angle = 70; angle <= 140; angle += 5)  {
   servoPulse(servo, angle);  }
 for (int angle = 140; angle >= 0; angle -= 5)  {
   servoPulse(servo, angle);  }

 for (int angle = 0; angle <= 70; angle += 5)  {
   servoPulse(servo, angle);  }

distance_F = Ultrasonic_read();

delay(500);
}


void loop(){  

  int currentPressure = analogRead(pressurePin);
  int pressureDiff = abs(currentPressure - pressureInit);
  Serial.print("Pressure: ");
  Serial.println(currentPressure);

  // 차량이 완전히 정지되어 대기 중일 때 (pause 상태)
  if (carPaused) {
    if (pressureDiff < 50) { // 초기값으로 돌아온 경우
      Serial.println("압력이 초기값으로 돌아옴. 차량 재출발!");
      carPaused = false;
      carStarted = true;
    } else {
      Stop();
      delay(100);
      return;
    }
  }

  // 압력 변화 감지 전에는 대기 상태
  if (!carStarted) {
    if (pressureDiff > 100) {
      Serial.println("압력 감지됨. 차량 출발!");
      carStarted = true;
    } else {
      Stop(); 
      delay(100);
      return; 
    }
  }
distance_F = Ultrasonic_read();
Serial.print("D F=");Serial.println(distance_F);


 if((digitalRead(R_S) == 0)&&(digitalRead(L_S) == 0)&&(digitalRead(M_S) == 1)){
  if(distance_F > Set){forword();}
                  else{Check_side();}  
 }  
 
else if((digitalRead(R_S) == 1)&&(digitalRead(L_S) == 0)){
  Stop();
  delay(100);
  turnRight();
  delay(25); }
else if((digitalRead(R_S) == 0)&&(digitalRead(L_S) == 1)){
  Stop();
  delay(100);
  turnLeft();
  delay(25); } 
else if ((digitalRead(R_S) == 1) && (digitalRead(L_S) == 1)) {
    Stop();
    delay(100);
    const char text[] = "STOP";
    bool success = radio.write(&text, sizeof(text));
    if (success) {
      Serial.println("STOP send signal");
    } else {
      Serial.println("Failed send signal");
    }

    carStarted = false;
    carPaused = true;

    return;
  }
delay(50);
}

void servoPulse (int pin, int angle){
int pwm = (angle*22) + 500;
 digitalWrite(pin, HIGH);
 delayMicroseconds(pwm);
 digitalWrite(pin, LOW);
 delay(50);
}


long Ultrasonic_read(){
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  long time = pulseIn (echo, HIGH);
  return time / 29 / 2;
}

void compareDistance(){
    if(distance_L > distance_R){
  turnLeft();
  delay(500);
  forword();
  delay(600);
  turnRight();
  delay(500);
  forword();
  delay(600);
  turnRight();
  delay(400);
  }
  else{
  turnRight();
  delay(500);
  forword();
  delay(600);
  turnLeft();
  delay(500);
  forword();
  delay(600);  
  turnLeft();
  delay(400);
  }
}

void Check_side(){
    Stop();
    delay(100);
 for (int angle = 70; angle <= 140; angle += 5)  {
   servoPulse(servo, angle);  }
    delay(300);
    distance_R = Ultrasonic_read();
    Serial.print("D R=");Serial.println(distance_R);
    delay(100);
  for (int angle = 140; angle >= 0; angle -= 5)  {
   servoPulse(servo, angle);  }
    delay(500);
    distance_L = Ultrasonic_read();
    Serial.print("D L=");Serial.println(distance_L);
    delay(100);
 for (int angle = 0; angle <= 70; angle += 5)  {
   servoPulse(servo, angle);  }
    delay(300);
    compareDistance();
}

void forword(){ 
digitalWrite(in1, LOW); //L M b
digitalWrite(in2, HIGH); //L M f 
digitalWrite(in3, HIGH); //R M f 
digitalWrite(in4, LOW); //R M b
analogWrite(enA, 150);
analogWrite(enB, 150);
}

void backword(){ //backword
digitalWrite(in1, HIGH); //L M B 
digitalWrite(in2, LOW); //L M F 
digitalWrite(in3, LOW); //R M F 
digitalWrite(in4, HIGH); //R M B
analogWrite(enA, 150);
analogWrite(enB, 150);
}

void turnRight(){ //turnRight
digitalWrite(in1, LOW); //L M B 
digitalWrite(in2, HIGH); //L M F 
digitalWrite(in3, LOW); //R M F 
digitalWrite(in4, HIGH); //R M B
analogWrite(enA, 200);
analogWrite(enB, 200);
}

void turnLeft(){ //turnLeft
digitalWrite(in1, HIGH); //L M B 
digitalWrite(in2, LOW); //L M F 
digitalWrite(in3, HIGH); //R M F 
digitalWrite(in4, LOW); //R M B
analogWrite(enA, 200);
analogWrite(enB, 200);
}

void Stop(){ //stop
digitalWrite(in1, LOW); //L M B 
digitalWrite(in2, LOW); //L M F 
digitalWrite(in3, LOW); //R M F 
digitalWrite(in4, LOW); //R M B
analogWrite(enA, 0);
analogWrite(enB, 0);
}

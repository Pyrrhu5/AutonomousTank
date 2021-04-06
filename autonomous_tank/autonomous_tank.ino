#include <IRremote.h>
#include <IRremoteInt.h>

#include <Servo.h>

/* ========================================================================== */
/*                                PINS CONFIG                                 */
/* ========================================================================== */
// Motors
#define IN1  	8   // K1, K2 motor direction
#define IN2  	9   // K1, K2 motor direction
#define IN3  	10  // K3, K4 motor direction
#define IN4  	12  // K3, K4 motor direction
#define ENA  	5   // Needs to be a PWM pin to be able to control motor speed ENA
#define ENB  	6   // Needs to be a PWM pin to be able to control motor speed ENB
// Headlights
#define LED_LEFT 	2   // left led connect to D2
#define LED_RIGHT 	3   // right led connect to D3
// Obstacles detector
#define SERVO 	11  // servo connect to D11
#define ECHO  	A3  // Ultrasonic Echo pin connect to A2
#define TRIG  	A2  // Ultrasonic Trig pin connect to A3
// Remote control
#define IRPIN 	13  // IR receiver Signal pin connect to Arduino pin 13
// Buzzer
#define BUZZER_PIN 7

/* ========================================================================== */
/*                            COMPONANTS SETTINGS                             */
/* ========================================================================== */
// Motor
#define RSPEED   	255  		// right motor speed
#define LSPEED   	255  		// left motor speed

// Obstacles avoidance
#define FRONT_DIST_MAX 25 		// in cm
#define SIDE_DIST_MAX  20     // in cm
Servo avoidanceServo;

enum DN {
  START_AVOIDANCE,
  STOP_AVOIDANCE,
  DEF
} 
Drive_Num=DEF;

// Remote control
#define IR_OK 		0x00FF38C7  // "OK" button
#define IR_0 		  0x00FF9867  // "0" button
#define IR_UP 		0x00FF18E7  // "▲" button
#define IR_DOWN 	0x00FF4AB5  // "▼" button
#define IR_RIGHT 	0x00FF5AA5  // ">" button
#define IR_LEFT 	0x00FF10EF  // "<" button
#define IR_POUND 	0x00FFB04F  // "#" button
#define IR_STAR 	0x00FF6897  // "*" button
IRrecv IR(IRPIN);
decode_results IrResults;

/* ========================================================================== */
/*                              GLOBAL VARIABLES                              */
/* ========================================================================== */
// for avoidance, the distances in all directions (in cm)
int frontDist, leftDist, leftishDist, rightDist, rightishDist;

// autonomous driving on/off
bool isAutonomousOn = false;

// for autonomous driving, a flag to forbid to go forward after reversing
// to prevent it to be stuck in a deadend
bool forwardForbidden = false;

// Overwrite autonomous driving
const int overwriteDuration = 500; // in ms
bool isStarted = false;

// For non-blocking delays
unsigned long currentMs = 0;
unsigned long nextAutonomousTickMs = 0;
unsigned long lastAutonomousTickMs = 0;

/* ========================================================================== */
/*                               TANK MOVEMENTS                               */
/* ========================================================================== */
void tank_speed(int lspeed,int rspeed) {
  analogWrite(ENA,lspeed); 
  analogWrite(ENB,rspeed);   
}

void tank_ahead() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void tank_back() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW); 
}

void tank_stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW); 
}

void tank_left() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void tank_right() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

/* ========================================================================== */
/*                            OBSTACLES AVOIDANCE                             */
/* ========================================================================== */
int get_distance() {
  /*
   * Read the distance with the ultrasonic sensor
   * in cm
   */
  long howFar;
  digitalWrite(TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(15);
  digitalWrite(TRIG, LOW);
  howFar = pulseIn(ECHO, HIGH);
  howFar = howFar * 0.01657; //how far away is the object in cm

  Serial.println((int)howFar);

  return round(howFar);
}

bool is_obs_forward() {

  // do a sweep to check a wider angle
  for ( int i = 60 ; i < 121 ; i += 15 ) {
    avoidanceServo.write(i);
    delay(100);

    if ( get_distance() <= FRONT_DIST_MAX ) {
      return true;
    }

  }

  return false;

}

bool is_obs_left() {
  avoidanceServo.write(120);
  delay(150);
  rightishDist = get_distance();
  avoidanceServo.write(180);
  delay(100);
  rightDist = get_distance();

  return rightishDist <= SIDE_DIST_MAX || rightDist <= SIDE_DIST_MAX;
}

bool is_obs_right() {
  avoidanceServo.write(60);
  delay(150);
  leftishDist = get_distance();
  avoidanceServo.write(0);
  delay(100);
  leftDist = get_distance();

  return leftishDist <= SIDE_DIST_MAX || leftDist <= SIDE_DIST_MAX;
}

void read_directions() {
  /* 
   *  Read the obstacles in all directions
   */

}

/* ========================================================================== */
/*                                   BUZZER                                    */
/* ========================================================================== */
void bip(int howLong=250) {
	/* 
		Make a short sound with the buzzer.
		Blocking delay
	*/
	digitalWrite(BUZZER_PIN, LOW);
	delay(howLong);
	digitalWrite(BUZZER_PIN, HIGH);
}

/* ========================================================================== */
/*                                 HEADLIGHTS                                 */
/* ========================================================================== */
void headlights_on() {
  digitalWrite(LED_LEFT, LOW);
  digitalWrite(LED_RIGHT, LOW);
}

void headlights_off() {
  digitalWrite(LED_LEFT, HIGH);
  digitalWrite(LED_RIGHT, HIGH);
}

void headlights_switch() {
  digitalWrite(LED_LEFT, !digitalRead(LED_LEFT));
  digitalWrite(LED_RIGHT, !digitalRead(LED_RIGHT));
}

/* ========================================================================== */
/*                               REMOTE CONTROL                               */
/* ========================================================================== */
void remote_tick() {
  if (IR.decode(&IrResults)) {
    switch (IrResults.value) {
    case IR_OK:
	  bip();
      isAutonomousOn = !isAutonomousOn;
      tank_stop();
      break;
    case IR_0:
      if (!isStarted) {
		  bip();
        tank_speed(RSPEED, LSPEED);
        isStarted = true;
      } 
      else {
		  bip();
        tank_speed(0, 0);
        tank_stop();
        isStarted = false;
      }
      break;

      // manual overwrite
    case IR_UP:
	  bip();
      tank_ahead();
      //delay(overwriteDuration);
      //tank_stop();
      break;
    case IR_DOWN:
	  bip();
      tank_back();
      //delay(overwriteDuration);
      //tank_stop();
      break;
    case IR_LEFT:
	  bip();
      tank_left();
      delay(overwriteDuration);
      //tank_stop();
      tank_ahead();
      break;
    case IR_RIGHT:
	  bip();
      tank_right();
      delay(overwriteDuration);
      //tank_stop();
      tank_ahead();
      break;

    case IR_STAR:
	  bip();
      headlights_switch();
      break;
    case IR_POUND:
	  bip();
      break;
    default:
      ;
    }
    // Reset
    //IrResults.value = 0;
    IR.resume();
  }
}

/* ========================================================================== */
/*                              AUTONOMOUS DRIVE                              */
/* ========================================================================== */
void auto_drive_tick() {
  tank_speed(RSPEED, LSPEED);

  if ( !is_obs_forward() && !forwardForbidden ) {
    //nextDir = directions[random(0,2)];
    tank_ahead();
    delay(500);
    return;

  } 
  else if ( !is_obs_right() ) {

    forwardForbidden = false;
    tank_right();
    delay(500);
    auto_drive_tick();
    return;

  } 
  else if ( !is_obs_left() ) {

    forwardForbidden = false;
    tank_left();
    delay(500);
    auto_drive_tick();
    return;

  }
  else {

    // All direction are blocked
    forwardForbidden = true;
    tank_back();
    delay(500);
    auto_drive_tick();
    return;

  }

}

/* ========================================================================== */
/*                                    MAIN                                    */
/* ========================================================================== */
void setup(){
  // Motors
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT); 
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT); 
  pinMode(ENA, OUTPUT);  
  pinMode(ENB, OUTPUT);
  // Headlights
  pinMode(LED_LEFT, OUTPUT);
  pinMode(LED_RIGHT, OUTPUT);
  headlights_off();

  // Obstacle avoidance
  avoidanceServo.attach(SERVO);
  avoidanceServo.write(90);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(TRIG, LOW);

  // Remote control
  pinMode(IRPIN, INPUT); 
  digitalWrite(IRPIN, HIGH);  
  IR.enableIRIn();
  // buzzer
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  // Console output
  Serial.begin(9600);
  // Tank	
  tank_stop();

}

void loop(){
  currentMs = millis();

  // Non blocking delay for autonomous driving
  // to let the IR read happening
  if (isAutonomousOn && (currentMs - lastAutonomousTickMs >= 250) ) {
    lastAutonomousTickMs = millis();
    auto_drive_tick();
  }
  remote_tick();
    //digitalWrite(BUZZER_PIN, HIGH);
    //delay(1000);
	/* bip(); */
	/* delay(500); */
}


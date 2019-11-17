/* Author: Pyae Soan Aung (Ghostman @ greenhackers)
 *  Credits:
 *    Sung Jik Cha      http://forum.arduino.cc/index.php?topic=8652.0
 *    Dallaby           http://letsmakerobots.com/node/19558#comment-49685
 *    Bill Porter       http://www.billporter.info/?p=286
 *    bobbyorr(nice connection diagram) http://forum.pololu.com/viewtopic.php?f=15&t=1923
 */
#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
#endif


#define right_encoderC1      2     // (interrupt numbers 0)
#define right_encoderC2      22

#define slave_right_encoderC1      3     // (interrupt numbers 1)
#define slave_right_encoderC2      24

#define left_encoderC1      18     // (interrupt numbers 5)
#define left_encoderC2      26

#define slave_left_encoderC1      19     // (interrupt numbers 4)
#define slave_left_encoderC2      28

#define SMOOTH      10
#define sign(x) (x > 0) - (x < 0) // if x>0 --> 1 , if x<0 --> -1, if x=0 --> 0
#define LOOPTIME        100


//Motor Shield headers
#include <Wire.h>
#include <Adafruit_MotorShield.h>
//#include "utility/Adafruit_PWMServoDriver.h"   //re uncomment this one
#include "robot_specs.h"
#include <CmdMessenger.h>

/*------------------IMU-------------------- */
long accelX,accelY,accelZ;
volatile float gForceX,gForceY,gForceZ;

long gyroX,gyroY,gyroZ;
volatile float rotX,rotY,rotZ;

//--------------------new-------------------/
int end_bit = 0;
char field_separator   = ',';
char command_separator = ' ';
char escape_separator  = '/n';
float frequency=0;
enum
{
  Start,          // 0
  rpm_req,        // 1
  right_rpm_req,  // 2
  left_rpm_req,   // 3
  Rate,           // 4
  actual_rpm,     // 5
  imu,            // 6
};

//------------------------------------------

CmdMessenger cmdMessenger = CmdMessenger(Serial,field_separator,command_separator,escape_separator);

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motor_right = AFMS.getMotor(1); // right
Adafruit_DCMotor *slave_right = AFMS.getMotor(2);

Adafruit_DCMotor *motor_left = AFMS.getMotor(3);  // left
Adafruit_DCMotor *slave_left = AFMS.getMotor(4);

unsigned long lastMilli = 0;       // loop timing 
unsigned long lastMilliPub = 0;

double rpm_req1 = 0;
double rpm_req2 = 0;
double rpm_act1 = 0; double rpm_act1_slave = 0; //right
double rpm_act2 = 0; double rpm_act2_slave = 0; // left

int direction1 = FORWARD;  // right
int direction2 = FORWARD;  // left
int prev_direction1 = RELEASE; // right
int prev_direction2 = RELEASE; // left
int PWM_val1 = 0; int PWM_val1_slave = 0; // right
int PWM_val2 = 0; int PWM_val2_slave = 0; // left

volatile long right_count = 0;    volatile long slave_right_count = 0;      
volatile long left_count = 0;     volatile long slave_left_count = 0;
//volatile bool slave_right_encoderC1Set; // helper variable for interrupt
//volatile bool slave_left_encoderC1Set;  // helper variable for interrupt

long countAnt1 = 0; long countAnt1_slave = 0; // right
long countAnt2 = 0; long countAnt2_slave = 0; // left

float Kp =   0.7;
float Ki =   0.01;
float Kd =   0.6;

////////////////////////////// callback ////////////////////////////////////////

void attachCommandCallbacks()
{
  // Attach callback methods
  cmdMessenger.attach(rpm_req,setRPM);
 // cmdMessenger.attach(leftRPM,setLeftRPM);
 // cmdMessenger.attach(Rate,setRate);
}

void setRPM()
{
  rpm_req1 = cmdMessenger.readInt16Arg();
  rpm_req2 = cmdMessenger.readInt16Arg();
  
}


////////////////////////////////////////////////////////////////////////////

void setup() { 
  //Serial.begin(9600);
 setupMessenger();
 setupMotors();
 //setupIMU();
 setupEncoders(); 
}
//////////////////////////////////// LOOOPP //////////////////////////////
void loop() {
  unsigned long time = millis();  
  if(time-lastMilli>= LOOPTIME)   {      // enter timed loop ( 100 ms or greater)

    cmdMessenger.feedinSerialData();
    int x = time-lastMilli;
    getMotorData(x);
    PWM_val1 = MasterUpdatePid(1, PWM_val1, rpm_req1, rpm_act1);
    PWM_val2 = MasterUpdatePid(2, PWM_val2, rpm_req2, rpm_act2);

    PWM_val1_slave = SlaveUpdatePid(1,PWM_val1_slave,rpm_req1,rpm_act1_slave);
    PWM_val2_slave = SlaveUpdatePid(2,PWM_val2_slave,rpm_req2,rpm_act2_slave);

    if(PWM_val1 > 0) direction1 = FORWARD;
    else if(PWM_val1 < 0) direction1 = BACKWARD;
    if (rpm_req1 == 0) direction1 = RELEASE;
    if(PWM_val2 > 0) direction2 = FORWARD;
    else if(PWM_val2 < 0) direction2 = BACKWARD;
    if (rpm_req2 == 0) direction2 = RELEASE;
    motor_right->run(direction1);
    slave_right->run(direction1);
    
    motor_left->run(direction2);
    slave_left->run(direction2);
    
    motor_right->setSpeed(abs(PWM_val1));
    slave_right->setSpeed(abs(PWM_val1_slave));
    
    motor_left->setSpeed(abs(PWM_val2));
    slave_left->setSpeed(abs(PWM_val2_slave));

    //updateIMU();

    float y = x/1000.0; // millis to seconds
    sendMessage(y);

    lastMilli = time;
  }
  //Serial.print(" ");Serial.print(rpm_req1);
  //Serial.print(" ");Serial.print(rpm_act1);  
  //Serial.print(" ");Serial.print(rpm_act1_slave);
  //Serial.print(" ");Serial.println(slave_right_count);
  //Serial.print(" ");Serial.println(right_count);
 
  //Serial.print(" ");Serial.print(rpm_req2);
  //Serial.print(" ");Serial.print(rpm_act2);  
  //Serial.print(" ");Serial.print(rpm_act2_slave);
  //Serial.print(" ");Serial.println(slave_left_count);
  //Serial.print(" ");Serial.println(left_count);
  //Serial.println(" ");
  
}

//

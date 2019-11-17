#define pi 3.1415926
#define twoPi 6.28318
#include <MPU6050_tockn.h>

MPU6050 mpu(Wire);
int degree;
float radian=0.0,rad= 0.0;
void setup() {
  Serial.begin(9600);
  mpu.begin();
  //mpu.calcGyroOffsets(true);
  //mpu.setGyroOffsets(0.38,0.02,0.61); // mpu with red male connector
  mpu.setGyroOffsets(0.02,0.24,-1.01);  // old one
}

void loop() {
 mpu.update();
 degree = mpu.getAngleZ();
 if(degree > 360) degree = degree%360;
 else if(degree < -360) degree = degree%360;
 
 radian    = degree * 0.0174532;    // radian = degree * (2*pi / 360.0); 
 //Serial.print(" ");Serial.print(degree);
 Serial.print(" ");Serial.print(radian); 
 Serial.println();

}

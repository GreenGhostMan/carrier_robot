void setupMotors(){
  AFMS.begin();  // create with the default frequency 1.6KHz

  rpm_req1 = 0;
  rpm_req2 = 0;
  rpm_act1 = 0;
  rpm_act2 = 0;
  PWM_val1 = 0;
  PWM_val2 = 0;

  motor_right->setSpeed(0); slave_right->setSpeed(0);
  motor_left->setSpeed(0);  slave_left->setSpeed(0);
  motor_right->run(FORWARD);slave_right->run(FORWARD);
  motor_right->run(RELEASE);slave_right->run(RELEASE);
  motor_left->run(FORWARD); slave_left->run(FORWARD);
  motor_left->run(RELEASE); slave_left->run(RELEASE);
}

void getMotorData(unsigned long time)  {
 // rpm_act1 = double(right_count-countAnt1)/double(time) * 181.8181;
 // rpm_act2 = double(left_count-countAnt2)/double(time) * 181.8181;

 rpm_act1 = double((right_count-countAnt1)*60*1000)/double(time*new_enc_ticks);
 rpm_act1_slave = double(( slave_right_count - countAnt1_slave )*60*1000)/double(time*new_enc_ticks);
 
 rpm_act2 = double((left_count-countAnt2)*60*1000)/double(time*new_enc_ticks);
 rpm_act2_slave = double(( slave_left_count - countAnt2_slave )*60*1000)/double(time*new_enc_ticks);

 countAnt1 = right_count;
 countAnt1_slave = slave_right_count;
 
 countAnt2 = left_count;
 countAnt2_slave = slave_left_count;
}


int MasterUpdatePid(int id, int old_pwm, double targetValue, double currentValue) {
  double pidTerm = 0;                            // PID correction
  double error = 0;
  double new_rpm = 0;
  double new_pwm = 0;
  static double last_error1 = 0;
  static double last_error2 = 0;
  static double int_error1 = 0;
  static double int_error2 = 0;
  float p =   0.6;
  float i =   0.00005;
  float d =   0.6;
  error = targetValue-currentValue;
  if (id == 1) { // Right
    int_error1 += error;
    pidTerm = p*error + d*(error-last_error1) + i*int_error1;
    last_error1 = error;
  }
  else { // Left
    int_error2 += error;
    pidTerm = Kp*error + Kd*(error-last_error2) + Ki*int_error2;
    last_error2 = error;
  }
  new_rpm = constrain(double(old_pwm)*MAX_RPM/255.0 + pidTerm, -MAX_RPM, MAX_RPM);
  new_pwm = 255.0*new_rpm/MAX_RPM;
  return int(new_pwm);
}

int SlaveUpdatePid(int id, int old_pwm, double targetValue, double currentValue) {
  double pidTerm = 0;                            // PID correction
  double error = 0;
  double new_rpm = 0;
  double new_pwm = 0;
  static double last_error1 = 0;
  static double last_error2 = 0;
  static double int_error1 = 0;
  static double int_error2 = 0;
  float rp =   0.6;
  float ri =   0.00005;
  float rd =   0.6;

  float lp =   0.65;
  float li =   0.00005;
  float ld =   0.6;
  
  error = targetValue-currentValue;
  if (id == 1) { // Right
    int_error1 += error;
    pidTerm = rp*error + rd*(error-last_error1) + ri*int_error1;
    last_error1 = error;
  }
  else { // Left
    int_error2 += error;
    pidTerm = lp*error + ld*(error-last_error2) + li*int_error2;
    last_error2 = error;
  }
  new_rpm = constrain(double(old_pwm)*MAX_RPM/255.0 + pidTerm, -MAX_RPM, MAX_RPM);
  new_pwm = 255.0*new_rpm/MAX_RPM;
  return int(new_pwm);
}

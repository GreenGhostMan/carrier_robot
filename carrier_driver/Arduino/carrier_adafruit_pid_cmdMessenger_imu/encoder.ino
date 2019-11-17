void setupEncoders(){

  right_count = 0;
  left_count = 0;
  countAnt1 = 0;
  countAnt2 = 0;
  
  pinMode(right_encoderC1, INPUT); 
  pinMode(slave_right_encoderC1, INPUT); 
  pinMode(right_encoderC2, INPUT);
  pinMode(slave_right_encoderC2, INPUT);
  
  digitalWrite(right_encoderC1, HIGH);                // turn on pullup resistor
  digitalWrite(slave_right_encoderC1, HIGH);
  attachInterrupt(digitalPinToInterrupt(right_encoderC1), rightFront, CHANGE);
  attachInterrupt(digitalPinToInterrupt(slave_right_encoderC1), rightBack, CHANGE);
  
  pinMode(left_encoderC1, INPUT); 
  pinMode(slave_left_encoderC1, INPUT); 
  pinMode(left_encoderC2, INPUT);
  pinMode(slave_left_encoderC2, INPUT);
  
  digitalWrite(left_encoderC1, HIGH);                // turn on pullup resistor
  digitalWrite(slave_left_encoderC1, HIGH);
  attachInterrupt(digitalPinToInterrupt(left_encoderC1), leftFront, CHANGE);
  attachInterrupt(digitalPinToInterrupt(slave_left_encoderC1), leftBack, CHANGE);
}

void rightFront() {
  if(digitalRead(right_encoderC1) == digitalRead(right_encoderC2)){
    right_count = right_count - 1;
  }
  else{
    right_count = right_count + 1; 
  }
}
void rightBack() {
  if(digitalRead(slave_right_encoderC1) == digitalRead(slave_right_encoderC2)){
    slave_right_count = slave_right_count - 1;
  }
  else{
    slave_right_count = slave_right_count + 1; 
  }
}

void leftFront() {
  if(digitalRead(left_encoderC1) == digitalRead(left_encoderC2)){
    left_count = left_count + 1;
  }
  else{
    left_count = left_count - 1; 
  }
}
void leftBack() {
  if(digitalRead(slave_left_encoderC1) == digitalRead(slave_left_encoderC2)){
    slave_left_count = slave_left_count + 1;
  }
  else{
    slave_left_count = slave_left_count - 1; 
  }
}

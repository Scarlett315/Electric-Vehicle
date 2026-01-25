void IRAM_ATTR read_encoder_M1(){
  int b = digitalRead(ENC1B);
  if(b>0){ // when the interrupt triggers, signal b is high  (clockwise)
    pos_1++;
  } else{ // when the interrupt triggers, signal b is low (counterclockwise)
    pos_1--;
  }
}

void IRAM_ATTR read_encoder_M2(){
  int b = digitalRead(ENC2B);
  if(b>0){ // when the interrupt triggers, signal b is high  (clockwise)
    pos_2--;
  } else{ // when the interrupt triggers, signal b is low (counterclockwise)
    pos_2++;
  }
}

// reads new data in and updates heading -- do this as often as possible (prev. setting 50ms)
void update_heading(){
  movement.update();        // Read new movement data from the sensor
  omega = movement.getYaw() + 0.06; // angular velocity; this had an experimental error of ~0.06-0.12 for some reason

  //Serial.print("angular velocity: ");
  //Serial.println(omega);
  
  dT = millis() - prev_time; // calculate the change in time
  prev_time = millis();

  // integrate angular velocity with respect to time to get heading (approxmiation)
  dTheta = dT/1000 * omega;      // change in heading = change in time (seconds) x angular velocity
  heading += dTheta;               // calculate new heading
  
  //make the value wrap around (so that -180 < theta < 180)
  heading = normalize_angle(heading);
}



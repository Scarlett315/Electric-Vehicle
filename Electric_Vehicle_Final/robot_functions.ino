// CONVERSIONS
// output shaft rotation = input shaft rotations / gear ratio

// encoder counts --> wheel angle --> distance traveled
float counts_to_angle(int encoder_counts){ return ((float) encoder_counts / (float) (ENCODER_CPR)) / (float) GEAR_RATIO * 360.0; }
float angle_to_dist(float wheel_angle){ return (wheel_angle / 360.0) * (2 * PI * WHEEL_RADIUS) ;}

// target distance --> target wheel angle --> target encoder counts
float angle_to_counts(float angle){ return angle / 360.0 * (float) GEAR_RATIO * (float) (ENCODER_CPR); }
float dist_to_angle(float target_dist){ return target_dist / (2 * PI * WHEEL_RADIUS) * 360; }


// --------------------------- SIMPLE DRIVE -----------------------------------
// spin motor for a certain time
void rotate_time_simple(int motor, int speed, float target_time){
  float target_time_ms = target_time * 1000;
  int start_time = millis();
  mc.setSpeed(motor, speed);

  if (target_time_ms >= start_time){
    mc.setSpeed(motor, 0);
  }
  return;
}

// spin a single motor to get to a target distance (m) -- besides initial conversion everything is in encoder counts
void rotate_dist(int motor, float speed, float target_dist, bool debug){
  float target_counts = angle_to_counts(dist_to_angle(target_dist)); // how many encoder counts to get to that position
  if (debug){
    Serial.print("Target Counts: " );
    Serial.print(target_counts);
  }
  
  mc.setSpeed(motor, speed); // set motor speed!
  float pos_initial; 

  if (motor == 1){
    pos_initial = pos_1;

    // keep running motor (yield) while current position < initial + target change in position
    while (abs(pos_1) < abs(target_counts + pos_initial)){

      static unsigned long lastPrint = 0;
      if (debug && millis() - lastPrint > 100) {
        Serial.print("Encoder Position: ");
        Serial.println(pos_1);
        lastPrint = millis();
      }
      
      yield();
    } 

  } 
  
  if (motor == 2){
    pos_initial = pos_2;

    while (abs(pos_2) < abs(target_counts + pos_initial)){

      static unsigned long lastPrint = 0;
      if (debug && millis() - lastPrint > 100) {
        Serial.print("Encoder Position: ");
        Serial.println(pos_2);
        lastPrint = millis();
      
      yield();
      }
    }
  }

  mc.setSpeed(motor, 0); //stop motor

}

// drive -- spin both motors in the same direction
void drive_simple(float speed, float target_dist, bool debug){
  float target_counts = angle_to_counts(dist_to_angle(target_dist)); // how many encoder counts to get to that position

  float pos_initial_1 = pos_1;
  float pos_initial_2 = pos_2;

  if (debug){
    Serial.print("Target counts: ");
    Serial.print(target_counts);
    Serial.print(" | pos_initial_1: ");
    Serial.print(pos_initial_1);
    Serial.print(" | pos_initial_2: ");
    Serial.println(pos_initial_2);
  }

  mc.setSpeed(1, speed);
  mc.setSpeed(2, speed);

  // loop         TODO: should it stop if both of them reach the final position or take the average?
  while(abs(pos_1) < abs(target_counts + pos_initial_1) || abs(pos_2) < abs(target_counts + pos_initial_2)){

      // print encoder positions every 1s
      if (debug){
        static unsigned long last_print = 0;
        if (millis() - last_print > 1000) {
          Serial.print("Time: ");
          Serial.print(millis());
          Serial.print("ms | encoder pos_1: ");
          Serial.print(pos_1);
          Serial.print(" | encoder pos_2: ");
          Serial.println(pos_2);

          last_print = millis();
        }
      }

      yield();
  }

  mc.setSpeed(1, 0);
  mc.setSpeed(2, 0);
}

// tank turn a certain number of degrees
void tank_turn_simple(float speed, float target_angle, bool debug){
  float initial_angle = heading;

  mc.setSpeed(1, speed);
  mc.setSpeed(2, -speed);
  

}


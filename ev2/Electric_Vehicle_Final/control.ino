/*bool drive_towards(float speed, float target_dist, bool debug){
  static bool initialized = false;
  static long target_counts; 
  static long pos_initial_1, pos_initial_2;
  static float target_angle;
  static float prev_heading_error;
  static unsigned long last_print;
  static unsigned long last_time_correction;

  //initialization
  if (!initialized){
    target_counts = angle_to_counts(dist_to_angle(target_dist)); // how many encoder counts to get to that position
    pos_initial_1 = pos_1;
    pos_initial_2 = pos_2;
    target_angle = heading;
    prev_heading_error = 0;
    last_time_correction = millis();
    last_print = millis();

    if (debug){
      Serial.print("Target counts: ");
      Serial.print(target_counts);
      Serial.print(" | pos_initial_1: ");
      Serial.print(pos_initial_1);
      Serial.print(" | pos_initial_2: ");
      Serial.println(pos_initial_2);
    }

    //mc.setSpeed(1, speed);
    //mc.setSpeed(2, speed);

    initialized = true; 

  }

  // distance error calculation
  long delta_left  = pos_1 - pos_initial_1; 
  long delta_right = pos_2 - pos_initial_2;
  long avg_delta = (delta_left + delta_right) / 2;
  long dist_error = target_counts - avg_delta;

  // Stop condition
  if(abs(dist_error) < 10){
    mc.setSpeed(1, 0);
    mc.setSpeed(2, 0);

    //reset
    initialized = false;
    //current_speed = 0;
    prev_heading_error = 0;
    return true;
  }

  
  // heading error calculation & implementation of PD
  unsigned long now = millis();
  float dt = (now - last_time_correction) / 1000.0; // change in time
  if (dt <= 0) dt = 1e-3; // prevent mistakes/huge corrections
  last_time_correction = now;

  float heading_error = shortest_angle(target_angle, heading); //proportional
  float d_error = (heading_error - prev_heading_error) / dt; //derivative (dw/dt)
  prev_heading_error = heading_error;

  float correction = Kp_heading * heading_error + Kd_heading * d_error; // PD correction calculation

  
  float left_cmd  = constrain(speed - correction,  -400, 400);
  float right_cmd = constrain(speed + correction, -400, 400);
  mc.setSpeed(1, left_cmd);
  mc.setSpeed(2, right_cmd);

  // print encoder positions every 1s
  if (debug && millis() - last_print > 1000){
    Serial.print("Time: ");
    Serial.print(millis());
    Serial.print("ms | ENCODERS pos_1: ");
    Serial.print(pos_1);
    Serial.print(" & pos_2: ");
    Serial.println(pos_2);

    last_print = millis();
  }

}*/

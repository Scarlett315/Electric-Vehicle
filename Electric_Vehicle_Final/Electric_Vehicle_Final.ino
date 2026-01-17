#include <Motoron.h>
#include "Modulino.h"

MotoronI2C mc; // initialize motor controller
ModulinoMovement movement;


// ----------- PIN NUMBERS  ------------------
int BUTTON_PIN = 21;  // the number of the pushbutton pin
//encoders; motor #s 1/2 each with internal trigger A/B
#define ENC1A 5
#define ENC1B 6
#define ENC2A 13
#define ENC2B 14

// ----------- CONSTANTS & VARIABLES --------------------
bool run_complete = false;

// hardware constants -- everything length-related in meters
const float WHEEL_RADIUS = 0.0254;
const int ENCODER_CPR = 12;
const int GEAR_RATIO = 51;
const float AXIAL_DIST = 0.1; //distance beteen the wheels (m)

// position/encoders -- in encoder counts
int start_time;
int last_heading_update;
volatile long pos_1 = 0;
volatile long pos_2 = 0; 

// angle/heading
float x, y, z;
float omega; 

volatile float heading = 0; //z-axis heading; -180 < heading < 180
float prev_time = 0; // time since last measurement
float dT = 0;
float dTheta = 0;

// start button
bool started = false; 
int button_state = 0; 

// ---------- DECLARATIONS --------------
void rotate_dist(int motor, float speed, float target_dist, bool debug=false);
bool drive_simple(float speed, float target_dist, bool debug=false);
bool tank_turn_simple(float speed, float target_angle, bool debug=false);

void IRAM_ATTR read_encoder_M1();
void IRAM_ATTR read_encoder_M2();
void update_heading();

// -------------------------------------------------------------------------------------

// Motion States -- the "steps" that the robot takes
enum MotionState {
  WAITING,
  TURN_1,
  DRIVE_1,
  TURN_2,
  DONE
};

MotionState motion_state = WAITING;


// Setup & Run!
void setup() {
  setup_hardware();
  delay(100);
  Serial.println("Everything has been set up!");
}

void loop() {
  //update frequency = 1 / 50ms = 20Hz
  if (millis() - last_heading_update >= 50){
    update_heading();
    last_heading_update = millis();
  }
  
  // ---- motion state machine ----
  switch (motion_state) {
    case WAITING:
      if (await_button()){
        motion_state = TURN_1;
      }
      break;

    case TURN_1:
      if (tank_turn_simple(200, 90, true)) {
        motion_state = DRIVE_1;
      }
      break;

    case DRIVE_1:
      if (drive_simple(200, 1.0, true)) {  // 1 meter
        motion_state = TURN_2;
      }
      break;

    case TURN_2:
      if (tank_turn_simple(200, 90, true)) {
        motion_state = DONE;
      }
      break;

    case DONE:
      break;
  }
}

// -------------------------------------------------------------------------------------

void setup_hardware(){
  Serial.begin(9600);
  while (!Serial) { delay(10); }
  start_time = millis();
  last_heading_update = millis();


  Wire.begin();
  
  // MOTOR CONTROLLER (re)INITIALIZATION
  mc.reinitialize();   
  mc.disableCrc();  
  mc.clearResetFlag();  
  mc.disableCommandTimeout();  // disable timeout

  // Configure motor 1
  mc.setMaxAcceleration(1, 200);
  mc.setMaxDeceleration(1, 200);

  // Configure motor 2
  mc.setMaxAcceleration(2, 200);
  mc.setMaxDeceleration(2, 200);

  // ENCODER PIN INITIALIZATION
  pinMode(ENC1A, INPUT);
  pinMode(ENC1B, INPUT);
  pinMode(ENC2A, INPUT);
  pinMode(ENC2B, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENC1A), read_encoder_M1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC2A), read_encoder_M2, RISING);

  // IMU INITIALIZATION
  Modulino.begin();
  movement.begin();
  
  // PUSHBUTTON INITIALIZATION
  pinMode(BUTTON_PIN, INPUT); 
}

// wait until button is pressed to start the run
bool await_button(){
  delay(100);

  button_state = digitalRead(BUTTON_PIN);

  // check if the pushbutton is pressed
  if (button_state == HIGH) {
    Serial.println("Run started!!!");
    delay(500); // wait 0.5s
    return true;
  } 
  return false;

}


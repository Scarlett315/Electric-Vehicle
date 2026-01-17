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
// hardware constants -- everything length-related in meters
const float WHEEL_RADIUS = 0.0254;
const int ENCODER_CPR = 12;
const int GEAR_RATIO = 51;
const float AXIAL_DIST = 0.1 //distance beteen the wheels (m)

// position/encoders -- in encoder counts
int start_time;
int last_heading_update;
volatile long pos_1 = 0;
volatile long pos_2 = 0; 
volatile long heading = 0; // -180 < heading < 180

// angle/heading
float x, y, z;
float omega; 
float theta = 0; // z-axis heading; the z-axis (left/right) is the important one here
float prev_time = 0; // time since last measurement
float dT = 0;
float dTheta = 0;

// start button
bool started = false; 
int buttonState = 0; 

// ---------- DECLARATIONS --------------
void rotate_dist(int motor, float speed, float target_dist, bool debug=false);
void drive_simple(float speed, float target_dist, bool debug=false);

void IRAM_ATTR read_encoder_M1();
void IRAM_ATTR read_encoder_M2();

// -------------------------------------------------------------------------------------
void setup() {
  setup_hardware();
  delay(100);
  Serial.println("Everything has been set up!");
}

void loop() {
  
  //update frequency = 1 / 50ms = 20kHz
  if (last_heading_update % 50 == 0){
    update_heading();
    last_heading_update = millis();
  }
  

  delay(500);
  drive_simple(200.0, 1.0, true);
  Serial.println("motor finished running! :D");
  while(true);
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
  mc.setMaxAcceleration(1, 140);
  mc.setMaxDeceleration(1, 300);

  // Configure motor 2
  mc.setMaxAcceleration(2, 200);
  mc.setMaxDeceleration(2, 300);

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


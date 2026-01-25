// This example shows a simple way to control the
// Motoron Motor Controller using I2C.
//
// The motors will stop but automatically recover if:
// - Motor power (VIN) is interrupted, or
// - A temporary motor fault occurs, or
// - A command timeout occurs.
//
// The motors will stop until you power cycle or reset your
// Arduino if:
// - The Motoron experiences a reset.
//
// If a latched motor fault occurs, the motors
// experiencing the fault will stop until you power cycle motor
// power (VIN) or cause the motors to coast.

#include <Motoron.h>

MotoronI2C mc;
int button_pin = 21;

void setup()
{
  pinMode(button_pin, INPUT); 

  Serial.begin(9600);
  Wire.begin();

  // Reset the controller to its default settings, then disable
  // CRC.  The bytes for each of these commands are shown here
  // in case you want to implement them on your own without
  // using the library.
  mc.reinitialize();    // Bytes: 0x96 0x74
  mc.disableCrc();      // Bytes: 0x8B 0x04 0x7B 0x43

  // Clear the reset flag, which is set after the controller
  // reinitializes and counts as an error.
  mc.clearResetFlag();  // Bytes: 0xA9 0x00 0x04

  // By default, the Motoron is configured to stop the motors if
  // it does not get a motor control command for 1500 ms.  You
  // can uncomment a line below to adjust this time or disable
  // the timeout feature.
  // mc.setCommandTimeoutMilliseconds(1000);
  // mc.disableCommandTimeout();

  // Configure motor 1
  mc.setMaxAcceleration(1, 140);
  mc.setMaxDeceleration(1, 300);

  // Configure motor 2
  mc.setMaxAcceleration(2, 200);
  mc.setMaxDeceleration(2, 300);

}

bool started = false;
int button_state = 0;
int start_time = 0;

void loop()
{
   
  //delay(100);
  button_state = digitalRead(button_pin);
 //Serial.print(button_state);
  if (button_state == HIGH){
    started = true;
    Serial.println("Started!!!!!!");
    start_time = millis();
  }

  if (started){
    mc.setSpeed(1, 200);
    mc.setSpeed(2, -200);
  }


  if (millis() - start_time >= 5000){
    started = false;
    mc.setSpeed(1, 0);
    mc.setSpeed(2, 0);
  }

  //printMotoronStatusFlags();
  
}

void printMotoronStatusFlags()
{
  Serial.println(F("=== Motoron Status Flags ==="));



  Serial.print(F("Motor fault (latched): "));
  Serial.println(mc.getMotorFaultLatchedFlag());

  Serial.print(F("No power (latched): "));
  Serial.println(mc.getNoPowerLatchedFlag());

  Serial.print(F("Motor faulting: "));
  Serial.println(mc.getMotorFaultingFlag());

  Serial.print(F("No power: "));
  Serial.println(mc.getNoPowerFlag());

  Serial.print(F("Error active: "));
  Serial.println(mc.getErrorActiveFlag());

  Serial.print(F("Motor output enabled: "));
  Serial.println(mc.getMotorOutputEnabledFlag());

  Serial.print(F("Motor driving: "));
  Serial.println(mc.getMotorDrivingFlag());

  Serial.println(F("============================"));
}

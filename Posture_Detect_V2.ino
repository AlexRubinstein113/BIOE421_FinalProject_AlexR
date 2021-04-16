///////////////////////////////////////////////////////////////////////////////
// Posture Detector Test
// Alex Rubinstein


#include <Adafruit_CircuitPlayground.h>

#define POSTURE_ANGLE        10.0      // allowable posture angle (degrees)
#define SLOUCH_TIME          3000      // allowable slouch time (seconds)
#define GRAV                 9.80665   // gravity (m/s^2)
#define RAD2DEG              52.29578  // convert radians to degrees

float CurrentAngle;
float TargetAngle;
unsigned long SlouchStart;
bool slouch;

  // Initialize Serial Print Times

const unsigned long EventInterval = 1000;
unsigned long PreviousTime = 0;


///////////////////////////////////////////////////////////////////////////////
void setup() {
  // Initialize Circuit Playground
  CircuitPlayground.begin();

  // Set initial target angle to zero.
  TargetAngle = 0;



}

///////////////////////////////////////////////////////////////////////////////
void loop() {
  // Compute current angle
  CurrentAngle = RAD2DEG * asin(-CircuitPlayground.motionZ() / GRAV);

  // Set target angle on button press
  if ((CircuitPlayground.leftButton()) || (CircuitPlayground.rightButton())) {
    TargetAngle = CurrentAngle;
    
    Serial.print("Target Angle is ");
    Serial.print(TargetAngle);
    Serial.println();
    
    CircuitPlayground.playTone(500,30);
    delay(30);
    CircuitPlayground.playTone(500,30);
    delay(30);
  }
//   Print Current Angle to Serial Monitor

 unsigned long CurrentTime = millis();

  if (CurrentTime - PreviousTime >= EventInterval) {
    Serial.println(CurrentAngle);

    PreviousTime = CurrentTime;
  }


  
  // Check if posture is poor
  if (CurrentAngle - TargetAngle > POSTURE_ANGLE) {
    if (!slouch) SlouchStart = millis();  
    slouch = true;
  } else {
    slouch = false;
  }

  // If there is bad posture
  if (slouch) {
    // Check how long bad posture has been occuring
    if (millis() - SlouchStart > SLOUCH_TIME) {
    // Activate Alarm
    CircuitPlayground.playTone(800, 500);    
    }
  }
}

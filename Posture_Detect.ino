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

// Initialize Button Functionality
bool ButtonState;
bool lastButtonState = false;
unsigned long PrevPressTime = 0;
const unsigned long PressTime = 1000;
bool LeftInput = false;

// Initialize Serial Print Times

const unsigned long EventInterval = 1000;
unsigned long PreviousTime = 0;
unsigned long  test = millis();

// Initialize Volume Type Modulation


///////////////////////////////////////////////////////////////////////////////
void setup() {
  // Initialize Circuit Playground
  CircuitPlayground.begin();

  // Set initial target angle to zero.
  TargetAngle = 0;


}

///////////////////////////////////////////////////////////////////////////////

void AlarmSwitch() {
  // Alarm Type Modulation

  bool slideSwitch = CircuitPlayground.slideSwitch();

  if (slideSwitch) {
    CircuitPlayground.playTone(800, 500);
  } else {
    CircuitPlayground.playTone(600, 2000);
  }
}



///////////////////////////////////////////////////////////////////////////////
void loop() {
  // Compute current angle
  CurrentAngle = RAD2DEG * asin(-CircuitPlayground.motionZ() / GRAV);


  /////////////////////////////////////

  // Hold Button Down Functionality
  bool Press = CircuitPlayground.leftButton();


  if (Press != lastButtonState) {
    if (Press == true) {
      PrevPressTime = millis();
      ButtonState = true;
    }
    else {
      ButtonState = false;
    }

    lastButtonState = Press;

  }
  if (ButtonState) {
    if ((millis() - PrevPressTime) >= PressTime ) {
      LeftInput = true;
    }
  }
  else {
    LeftInput = false;
  }

  if (LeftInput == true) {
    if ((millis() - PrevPressTime) >= PressTime ) {
      ButtonState = false;
    }
  }





  unsigned long CurrentTime = millis();

  if (CurrentTime - PreviousTime >= 500) {
    Serial.println(Press);
    Serial.println(lastButtonState);
    Serial.println(LeftInput);
    Serial.println(PrevPressTime);
    PreviousTime = CurrentTime;
  }






  // Set target angle on button press
  if ((LeftInput) || (CircuitPlayground.rightButton())) {
    TargetAngle = CurrentAngle;

    Serial.print("Target Angle is ");
    Serial.print(TargetAngle);
    Serial.println();

    CircuitPlayground.playTone(500, 30);

  }
  //   Print Current Angle to Serial Monitor

  // unsigned long CurrentTime = millis();

  //  if (CurrentTime - PreviousTime >= EventInterval) {
  //    Serial.println(CurrentAngle);

  //   PreviousTime = CurrentTime;
  //  }



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
      AlarmSwitch();
    }
  }
}

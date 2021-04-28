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

// Initialize Left Button Functionality
bool ButtonLState;
bool lastButtonLState = false;
unsigned long PrevPressLeftTime = 0;
const unsigned long PressLeftTime = 1000;
bool LeftInput = false;

// Initialize Right Button Funcionality
bool ButtonRState;
bool lastButtonRState = false;
unsigned long PrevPressRightTime = 0;
const unsigned long PressRightTime = 5000;
bool RightInput = false;

// Initialize Serial Print Times

const unsigned long EventInterval = 1000;
unsigned long PreviousTime = 0;

// Initialize Volume Type Modulation

// Initalize Slouch Counter

int Count = 0;

// Initialize Prone Detection
bool prone;
unsigned long proneTime;
const unsigned long proneinterval = 3000;
bool ProneDetState = false;

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

void TargetAngleColor() {
  // Device flashes blue when target angle is set

  for (int p = 0; p < 10; p++) {
    CircuitPlayground.setPixelColor(p, 53, 198, 242);
  }
  delay(500);
  CircuitPlayground.clearPixels();
}
///////////////////////////////////////////////////////////////////////////////

void SlouchAngleColor() {
  //  Device flashes red when slouch occurs

  for (int p = 0; p < 10; p++) {
    CircuitPlayground.setPixelColor(p, 247, 16, 0);
  }
  delay(300);
  CircuitPlayground.clearPixels();

}

///////////////////////////////////////////////////////////////////////////////

void SlouchCounter() {

  // Device counts number of slouch alarms and prints to serial monitor

  Count++;
  Serial.print("Slouch count is currently ");
  Serial.print(Count);
  Serial.println();
}

///////////////////////////////////////////////////////////////////////////////

void loop() {
  // Compute current angle
  CurrentAngle = RAD2DEG * asin(-CircuitPlayground.motionZ() / GRAV);



  /////////////////////////////////////

  // Hold Left Button Down Functionality
  bool PressLeft = CircuitPlayground.leftButton();


  if (PressLeft != lastButtonLState) {
    if (PressLeft == true) {
      PrevPressLeftTime = millis();
      ButtonLState = true;
    }
    else {
      ButtonLState = false;
    }

    lastButtonLState = PressLeft;

  }
  if (ButtonLState) {
    if ((millis() - PrevPressLeftTime) >= PressLeftTime ) {
      LeftInput = true;
    }
  }
  else {
    LeftInput = false;
  }

  if (LeftInput == true) {
    if ((millis() - PrevPressLeftTime) >= PressLeftTime ) {
      ButtonLState = false;
    }
  }

  //  unsigned long CurrentTime = millis();

  //  if (CurrentTime - PreviousTime >= 500) {
  //    Serial.println(PressLeft);
  //    Serial.println(lastButtonState);
  //    Serial.println(LeftInput);
  //    Serial.println(PrevPressLeftTime);
  //   PreviousTime = CurrentTime;
  //  }


  /////////////////////////////////////

  // Hold Right Button Down Functionality

  bool PressRight = CircuitPlayground.rightButton();


  if (PressRight != lastButtonRState) {
    if (PressRight == true) {
      PrevPressRightTime = millis();
      ButtonRState = true;
    }
    else {
      ButtonRState = false;
    }

    lastButtonRState = PressRight;

  }
  if (ButtonRState) {
    if ((millis() - PrevPressRightTime) >= PressRightTime ) {
      RightInput = true;
    }
  }
  else {
    RightInput = false;
  }



  if (RightInput == true) {
    ProneDetState = !ProneDetState;
    if (ProneDetState) {
      CircuitPlayground.playTone(600, 300);
    }
    else {
      CircuitPlayground.playTone(300, 300);
    }
    if ((millis() - PrevPressRightTime) >= PressRightTime ) {
      ButtonRState = false;
    }
  }




  /////////////////////////////////////




  // Set target angle on button press
  if (LeftInput) {
    TargetAngle = CurrentAngle;

    Serial.print("Target Angle is ");
    Serial.print(TargetAngle);
    Serial.println();

    CircuitPlayground.playTone(500, 50);
    TargetAngleColor();

  }
  //     Print Current Angle to Serial Monitor

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
      // Activate Alarm and Counter

      AlarmSwitch();
      SlouchAngleColor();
      SlouchCounter();
    }
  }

  /////////////////////////////////////

  // Prone Detection

  if (ProneDetState) {
    if (CircuitPlayground.motionX() >= -5 && CircuitPlayground.motionX() <= 5) {
      if (!prone) proneTime = millis();
      prone = true;
    } else {
      prone = false;
    }

    if (prone) {
      if (millis() - proneTime > proneinterval) {
        CircuitPlayground.playTone(900, 1000);
      }
    }
  }
}

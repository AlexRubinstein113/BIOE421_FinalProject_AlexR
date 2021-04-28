///////////////////////////////////////////////////////////////////////////////
// Posture Detector Test
// Alex Rubinstein


#include <Adafruit_CircuitPlayground.h>

#define POSTURE_ANGLE        10.0      // allowable posture angle (degrees)
#define SLOUCH_TIME          3000      // allowable slouch time (seconds)
#define GRAV                 9.80665   // gravity (m/s^2)
#define RAD2DEG              52.29578  // convert radians to degrees


// Initialize Slouch Functionality
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
    CircuitPlayground.playTone(800, 500);  // if switch is set to right, play shorter and higher tone
  } else {
    CircuitPlayground.playTone(600, 2000); // if switch is set to left, play longer and lower tone
  }

}



///////////////////////////////////////////////////////////////////////////////

void TargetAngleColor() {
  // Device flashes blue when target angle is set

  for (int p = 0; p < 10; p++) {
    CircuitPlayground.setPixelColor(p, 53, 198, 242); // set all pixels to blue color
  }
  delay(500);
  CircuitPlayground.clearPixels();
}
///////////////////////////////////////////////////////////////////////////////

void SlouchAngleColor() {
  //  Device flashes red when slouch occurs

  for (int p = 0; p < 10; p++) {
    CircuitPlayground.setPixelColor(p, 247, 16, 0); // set all pixels to red color
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


  if (PressLeft != lastButtonLState) {   // if the button state is changed
    if (PressLeft == true) {             // if the button state is pressed
      PrevPressLeftTime = millis();      // start button press counter
      ButtonLState = true;
    }
    else {
      ButtonLState = false;
    }

    lastButtonLState = PressLeft;      // Set button state for next cycle

  }
  if (ButtonLState) {             // if button is still pressed down
    if ((millis() - PrevPressLeftTime) >= PressLeftTime ) {   // if button is pressed down for specified period of time
      LeftInput = true;  // Send input from button press
    }
  }
  else {
    LeftInput = false;
  }

  if (LeftInput == true) {  // immediately set button state to false after output goes through
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
    ProneDetState = !ProneDetState;  // switch on or off prone detection state with successive outputs
    if (ProneDetState) {
      CircuitPlayground.playTone(600, 300);  // play higher tone if prone detection is turned on
    }
    else {
      CircuitPlayground.playTone(300, 300);  // play lower tone if prone detection is turned off
    }
    if ((millis() - PrevPressRightTime) >= PressRightTime ) {
      ButtonRState = false; // reset right button state
    }
  }




  /////////////////////////////////////




  // Set target angle on button press
  if (LeftInput) {  
    TargetAngle = CurrentAngle;  // set current angle as target angle

    Serial.print("Target Angle is ");  // print current angle to serial monitor
    Serial.print(TargetAngle);
    Serial.println();

    CircuitPlayground.playTone(500, 50); // play tone when sngle is set
    TargetAngleColor(); // Activate neopixels when target angle is set

  }
  //     Print Current Angle to Serial Monitor

  unsigned long CurrentTime = millis();

  if (CurrentTime - PreviousTime >= EventInterval) {
    Serial.println(CurrentAngle);

    PreviousTime = CurrentTime;
  }


  // Check if posture is poor
  if (CurrentAngle - TargetAngle > POSTURE_ANGLE) { // if posture angle is more than set number of degrees from target angle
    if (!slouch) SlouchStart = millis(); // activate slouch alarm timer 
    slouch = true; // set slouch state to true
  } else {
    slouch = false;
  }

  // If there is bad posture
  if (slouch) { // if there is slouching
    // Check how long bad posture has been occuring
    if (millis() - SlouchStart > SLOUCH_TIME) { // if alarm timer has surpassed set slouch grace period duration
      // Activate Alarm and Counter

      AlarmSwitch(); // activate auditory alarm 
      SlouchAngleColor(); // activate visual alarm
      SlouchCounter(); // add slouch instance to slouch counter
    }
  }

  /////////////////////////////////////

  // Prone Detection

  if (ProneDetState) { // prone detection is activated 
    if (CircuitPlayground.motionX() >= -5 && CircuitPlayground.motionX() <= 5) { // if device is oriented in position that indicates user is prone
      if (!prone) proneTime = millis(); // start prone alarm timer
      prone = true; // set prone state as true
    } else {
      prone = false;
    }

    if (prone) { // if user is prone
      if (millis() - proneTime > proneinterval) { // if prone alarm timer has surpassed set grace period 
        CircuitPlayground.playTone(900, 1000); // activate auditory alarm
      }
    }
  }
}

// Circuit Playground Lab 8- Simon Says
// Alex Rubinstein
// BIOE421
// Simon Says Game
// Rules: Press buttons in order shown. 
// If wrong button is pressed, game is lost.


#include <Adafruit_CircuitPlayground.h>

// define values to be used 

#define MAX_SEQUENCE    10
#define NO_BUTTON       -1
#define FAILURE_TONE    100
#define SEQUENCE_DELAY  800
#define TONE_TIME       250
#define CAP_THRESHOLD   10

// declare variables to be used

uint8_t simonSequence[MAX_SEQUENCE];
uint8_t skillLevel;
uint8_t sequenceLength;
uint8_t currentStep;
uint8_t losscounter = 0;
int8_t guess;

// create code structure for "buttons" of Simon Says

struct button {
  uint8_t capPad[2];
  uint8_t pixel[3];
  uint32_t color;
  uint16_t freq;
  
// Create variable that corresponds to four different colors of buttons

} simonButton[] = {
  { {3,2},    {0,1,2},  0x00FF00,   415 },  // GREEN
  { {0,1},    {2,3,4},  0xFFFF00,   252 },  // YELLOW
  { {12, 6},  {5,6,7},  0x0000FF,   209 },  // BLUE
  { {9, 10},  {7,8,9},  0xFF0000,   310 },  // RED 
};


///////////////////////////////////////////////////////////////////////////////
void newGame() {
  
  // setup game sequence
  
  sequenceLength = 8;
  
  for (int i=0; i<sequenceLength; i++) {
    simonSequence[i] = random(4);
  }

  // Start with first step
  currentStep = 1;
}

///////////////////////////////////////////////////////////////////////////////

// returns value "b" for corresponding button and plays corresponding tone for returned value "duration" 

void indicateButton(uint8_t b, uint16_t duration) {
  CircuitPlayground.clearPixels();
  for (int p=0; p<3; p++) {
    CircuitPlayground.setPixelColor(simonButton[b].pixel[p], simonButton[b].color);
  }
  CircuitPlayground.playTone(simonButton[b].freq, duration);
  CircuitPlayground.clearPixels();
}

///////////////////////////////////////////////////////////////////////////////
void ShowSequence() {
  // If the current step is less than 5, the tone plays longer (is slower), if step is more than 5 tone is faster
   
  uint16_t toneDuration;
  if (currentStep<=5) {
    toneDuration = 420;
  } else if (currentStep>5) {
    toneDuration = 320;
  }
  // Sequence will play back up to current step
  
  for (int i=0; i<currentStep; i++) {
    delay(50);
    indicateButton(simonSequence[i], toneDuration);
  }
}

///////////////////////////////////////////////////////////////////////////////

// Each button consists of two touch pads, they are scanned to see if touched
// If pads touched, return corresponding b value of pad  and set time value TONE_TIME, if not, return NO_BUTTON defined value

uint8_t getButtonPress() {
  for (int b=0; b<4; b++) {
    for (int p=0; p<2; p++) {
      if (CircuitPlayground.readCap(simonButton[b].capPad[p]) > CAP_THRESHOLD) {
        indicateButton(b, TONE_TIME);
        return b;
      }
    }
  }
  return NO_BUTTON;
}

///////////////////////////////////////////////////////////////////////////////
void LoseGame(int b) {
  // Show button that should have been pressed when button is pressed incorrectly 
  
  for (int p=0; p<3; p++) {
    CircuitPlayground.setPixelColor(simonButton[b].pixel[p], simonButton[b].color);
  }

  // Sound played when button is pressed incorrectly 
  
  CircuitPlayground.playTone(FAILURE_TONE, 1500);
 
  // Game state maintained until reset 
  
  while (true) {}
}

///////////////////////////////////////////////////////////////////////////////
void WinGame() {
  // Play sequence when game is won 
  
  for (int i=0; i<3; i++) {
    indicateButton(3, 100);  // RED
    indicateButton(1, 100);  // YELLOW
    indicateButton(2, 100);  // BLUE
    indicateButton(0, 100);  // GREEN
  }
  indicateButton(3, 100);  // RED
  indicateButton(1, 100);  // YELLOW

  // Change tones to failure tone
  
  for (int b=0; b<4; b++) simonButton[b].freq = FAILURE_TONE;

  // Continue sequence for time period
  
  for (int i=0; i<2; i++) {
    indicateButton(2, 100);  // BLUE
    indicateButton(0, 100);  // GREEN
    indicateButton(3, 100);  // RED
    indicateButton(1, 100);  // YELLOW
  }

  // Silence tone 
  
  for (int b=0; b<4; b++) simonButton[b].freq = 0;
  

}

///////////////////////////////////////////////////////////////////////////////
void setup() {
  // Initialize the Circuit Playground
  
  CircuitPlayground.begin();

  // Set random seed
  
  randomSeed(millis());
  
  // Create game
  
  newGame();
}

///////////////////////////////////////////////////////////////////////////////
void loop() {
  // Show sequence up to current step
  
  ShowSequence();

  // Read button presses
  
  for (int s=0; s<currentStep; s++) {

    guess = NO_BUTTON;
    while (guess==NO_BUTTON) {
      guess = getButtonPress();           
    }
    if (guess != simonSequence[s]) {
      LoseGame(simonSequence[s]);

    }
  }
    currentStep++; 
  if (currentStep > sequenceLength) {
    delay(SEQUENCE_DELAY);
    WinGame();
  }
  delay(SEQUENCE_DELAY);
}

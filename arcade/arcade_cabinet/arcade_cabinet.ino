// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library
#include <Adafruit_NeoPixel.h>

#define NEOPIXEL_PIN 6
#define NEOPIXEL_NUMPIXELS 30

#define RED_BUTTON_PIN 7
#define ORANGE_BUTTON_PIN 4


// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NEOPIXEL_NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#define STARTUP_COLOUR pixels.Color(25,25,25)
#define STARTUP_DELAY_MS 50
#define MIN_LED_FRAME_TIME_MS 50

#define LERP(x, x0, x1, y0, y1) (static_cast<float>(y0) + static_cast<float>(y1-y0) * static_cast<float>(x-x0) / static_cast<float>(x1-x0))

class Animator {
public:
  Animator() : frameDelayInMs(MIN_LED_FRAME_TIME_MS), numFrames(0), currFrame(0) {}
  virtual ~Animator() {}
  
  void setFrameDelay(uint32_t delayInMs) { this->frameDelayInMs = delayInMs; }
  uint32_t getFrameDelay() const { return this->frameDelayInMs; }
  
  boolean isFinished() const { return this->currFrame > this->numFrames; }
  
  virtual uint32_t updateFrame() = 0;
  virtual void setEndColour(uint8_t r, uint8_t g, uint8_t b) {}
  
protected:
  uint32_t frameDelayInMs; // Delay between frames of the animation
  uint32_t numFrames;      // Number of frames of the animation
  uint32_t currFrame;      // Current frame of the animation
};

class ButtonAnimator : public Animator {
public:  
  ButtonAnimator(uint8_t buttonPin): Animator(), pinModeState(LOW), buttonPin(buttonPin), numFramesForFlash(10) {}
  ~ButtonAnimator() {}
  
  void setIsActive(boolean isActive) {
    if (isActive) {
      this->currFrame = 0;
      this->numFrames = INFINITE_FRAMES;
    }
    else {
      this->numFrames = 0;
    }
  }
  
  void setNumFramesForFlash(uint32_t numFramesForFlash) {
    this->numFramesForFlash = numFramesForFlash;
  }
  
  uint32_t updateFrame() {
    if (this->numFrames == 0) {
      digitalWrite(this->buttonPin, LOW);
      return 0;
    }
    
    digitalWrite(this->buttonPin, this->pinModeState);
    this->currFrame++;
    
    if (this->numFramesForFlash > 0 && this->currFrame % this->numFramesForFlash == 0) {
      this->pinModeState = (this->pinModeState == HIGH) ? LOW : HIGH;
      this->currFrame = 0;
    } 
    
    return this->frameDelayInMs;
  }
  
private:
  static const uint32_t INFINITE_FRAMES = 99999;
  const uint8_t buttonPin;
  uint32_t numFramesForFlash;
  int pinModeState;
};

class LerpLEDStripAnimator : public Animator {
public:
  LerpLEDStripAnimator() : Animator(), r0(0), g0(0), b0(0), 
    r1(0), g1(0), b1(0), r(0), g(0), b(0) {}
  ~LerpLEDStripAnimator() {}

  void setLerp(uint32_t numFrames, uint8_t r0, uint8_t g0, uint8_t b0,
               uint8_t r1, uint8_t g1, uint8_t b1) {
    this->r0 = r0; this->g0 = g0; this->b0 = b0;
    this->r1 = r1; this->g1 = g1; this->b1 = b1;
    this->r = r0; this->g = g0; this->b = b0;
    this->numFrames = numFrames;
    this->currFrame = 0;
    //Serial.print("Setting LERP: ");
    //Serial.print("RGB0: ("); Serial.print(r0); Serial.print(","); Serial.print(g0); Serial.print(","); Serial.print(b0); Serial.print("), ");
    //Serial.print("RGB1: ("); Serial.print(r1); Serial.print(","); Serial.print(g1); Serial.print(","); Serial.print(b1); Serial.println(")");
  }
  
  uint32_t updateFrame() {
    if (this->isFinished()) {
      this->r = this->r1; this->g = this->g1; this->b = this->b1;
      this->frameDelayInMs = 0;
    }
    else {
      this->r = static_cast<uint8_t>(LERP(this->currFrame, 0, this->numFrames, this->r0, this->r1));
      this->g = static_cast<uint8_t>(LERP(this->currFrame, 0, this->numFrames, this->g0, this->g1));
      this->b = static_cast<uint8_t>(LERP(this->currFrame, 0, this->numFrames, this->b0, this->b1));
      this->currFrame++;
    }
    
    setAllPixels(this->r, this->g, this->b);
    pixels.show();
    
    return this->frameDelayInMs;
  }
  
  void setEndColour(uint8_t r, uint8_t g, uint8_t b) {
    this->r1 = r; this->g1 = g; this->b1 = b;
  }
  
private:
  uint8_t r0, g0, b0;  // r,g,b values at the start of the animation
  uint8_t r1, g1, b1;  // r,g,b values at the end of the animation
  uint8_t r, g, b;     // Current r,g,b values
};

class ConstLEDStripAnimator : public Animator {
public:
  ConstLEDStripAnimator() : r(0), g(0), b(0) {}
  ~ConstLEDStripAnimator() {}
  
  void setConst(uint32_t numFrames, uint8_t r, uint8_t g, uint8_t b) {
    this->r = r; this->g = g; this->b = b;
    this->numFrames = numFrames;
    this->currFrame = 0;
  }
  
  uint32_t updateFrame() {
    if (!this->isFinished()) {
      this->currFrame++;
    }
    else {
      this->frameDelayInMs = 0;
    }
    setAllPixels(this->r, this->g, this->b);
    pixels.show();
    
    return this->frameDelayInMs;
  }
  
  void setEndColour(uint8_t r, uint8_t g, uint8_t b) {
    this->r = r; this->g = g; this->b = b;
  }
  
private:
  uint8_t r, g, b;     // Current r,g,b values
};


#define NUM_LED_STRIP_ANIMATORS 7

Animator* currLedStripAnimators[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };
LerpLEDStripAnimator lerpLEDStripAnimator0;
LerpLEDStripAnimator lerpLEDStripAnimator1;
LerpLEDStripAnimator lerpLEDStripAnimator2;
ConstLEDStripAnimator constAnimator0;
ConstLEDStripAnimator constAnimator1;
ConstLEDStripAnimator constAnimator2;
ConstLEDStripAnimator constAnimator3;
ConstLEDStripAnimator constAnimator4;

ButtonAnimator redButtonAnimator(RED_BUTTON_PIN);
ButtonAnimator orangeButtonAnimator(ORANGE_BUTTON_PIN);

void clearLedStripAnimators() {
  for (int j = 0; j < NUM_LED_STRIP_ANIMATORS; j++) {
    currLedStripAnimators[j] = NULL;  
  }
}

void setup() {
  Serial.begin(38400);
  pixels.begin(); // This initializes the NeoPixel library.
  
  // Button LED outputs
  pinMode(RED_BUTTON_PIN, OUTPUT);
  pinMode(ORANGE_BUTTON_PIN, OUTPUT);
  
    // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
  for(int i = 0; i < NEOPIXEL_NUMPIXELS; i++){
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, STARTUP_COLOUR); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.
    delay(STARTUP_DELAY_MS); // Delay for a period of time (in milliseconds).
  }
  
}

void loop() {
  readSerialCommands();

  uint32_t maxFrameDelay = 0;

  for (int i = 0; i < NUM_LED_STRIP_ANIMATORS; i++) {
    Animator* currAnimator = currLedStripAnimators[i];
    if (currAnimator != NULL) {
      if (currAnimator->isFinished()) {
        currLedStripAnimators[i] = NULL;
      }
      else {
        maxFrameDelay = max(maxFrameDelay, currAnimator->updateFrame());
        break;
      }
    }
  }
  
  maxFrameDelay = max(maxFrameDelay, redButtonAnimator.updateFrame());
  maxFrameDelay = max(maxFrameDelay, orangeButtonAnimator.updateFrame());
   
  delay(maxFrameDelay);
  
  /*
  if (maxFrameDelay > 0) {
    Serial.print("Delay: "); Serial.println(maxFrameDelay);
  }
  */
  Serial.flush();
}

#define PKG_BEGIN_CHAR       '|'
#define QUERY_CHAR           'Q'
#define ALL_LED_RGB_CHAR     'A'
#define FLASH_RGB_CHAR       'F'
#define TWO_FLASH_RGB_CHAR   '2'
#define THREE_FLASH_RGB_CHAR '3'
#define LERP_RGB_CHAR        'L'
#define RED_BUTTON_CHAR      'R'
#define ORANGE_BUTTON_CHAR   'O'
#define TURN_OFF_BUTTON_CHAR 'X'

#define QUERY_RESPONSE "ARCADEARDUINO"

#define CONVERT_RGB_BYTE(b) (uint8_t)(((int)b+1)*2 - 1);

// Checks for available serial data -- this can guide certain operations for the LEDs:
//
// Basic/Communications Commands/Queries ----------------------------------------
// |QQ -- Respond back to inform the sender that this is the correct serial line
//
// Marque Commands --------------------------------------------------------------
// |Argb -- Sets all of the rgb LEDs to (r,g,b), where r,g, and b are each byte values in [0,127] which are mapped to [0,255]
// |Frgbt -- Causes a flash of a particular colour from the current colour to r,g,b and then back, 
//           the flash will hold for t centi-seconds (r,g,b,t are all bytes)
// |Lrgbt -- Causes the current colour to transition to the given colour over t 10th-seconds (r,g,b,t are all bytes)
// |2rgbt -- Causes the current colour to flash 2 times to the given colour and back,
//           the flash will hold for t centi-seconds (r,g,b,t) are all bytes
// |3rgbt -- Causes the current colour to flash 2 times to the given colour and back,
//           the flash will hold for t centi-seconds (r,g,b,t) are all bytes
//
// Button Commands --------------------------------------------------------------
// |RFt -- Tells the red button to flash, with delay between flashes equal to t 100th-seconds (t in bytes), if t is 0 then the LED stays on
// |OFt -- Tells the orange button to flash, with delay between flashes equal to t 100th-seconds (t in bytes), if t is 0 then the LED stays on
// |RX -- Tells the red button to turn its LED off
// |OX -- Tells the orange button to turn its LED off
void readSerialCommands() {

  while (Serial.available() >= 3) {

    char serialReadByte = Serial.read();
    if (serialReadByte == PKG_BEGIN_CHAR) {
      
      // Read the command
      serialReadByte = Serial.read();
      switch (serialReadByte) {
        case QUERY_CHAR:
          Serial.read();
          Serial.println(QUERY_RESPONSE);
          break;
        
        case ALL_LED_RGB_CHAR: {
          while (Serial.available() < 3);
          uint8_t r = CONVERT_RGB_BYTE(Serial.read());
          uint8_t g = CONVERT_RGB_BYTE(Serial.read());
          uint8_t b = CONVERT_RGB_BYTE(Serial.read());
          
          //Serial.print("Setting all LED colours to (");
          //Serial.print(r); Serial.print(","); Serial.print(g); Serial.print(","); Serial.print(b); 
          //Serial.println(")...");
          
          bool changed = false;
          for (int j = NUM_LED_STRIP_ANIMATORS-1; j >= 0 ; j--) {
            Animator* currAnimator = currLedStripAnimators[j];
            if (currAnimator != NULL) {
                currAnimator->setEndColour(r,g,b);
                changed = true;
                break;
            }
          }
          
          if (!changed) {
            setAllPixels(r,g,b);
            pixels.show();
          }
          
          break;
        }
        
        case FLASH_RGB_CHAR: {
          while (Serial.available() < 4);
          uint8_t r = CONVERT_RGB_BYTE(Serial.read());
          uint8_t g = CONVERT_RGB_BYTE(Serial.read());
          uint8_t b = CONVERT_RGB_BYTE(Serial.read());
          uint8_t t = Serial.read();
          
          uint8_t currR, currG, currB;
          getCurrentPixelColour(currR, currG, currB);
          
          clearLedStripAnimators();
          
          lerpLEDStripAnimator0.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          lerpLEDStripAnimator0.setLerp(2, currR, currG, currB, r, g, b);
          currLedStripAnimators[0] = &lerpLEDStripAnimator0;
          
          constAnimator0.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          constAnimator0.setConst(static_cast<uint32_t>(t * 10.0 / static_cast<float>(MIN_LED_FRAME_TIME_MS)), r, g, b);
          currLedStripAnimators[1] = &constAnimator0;
          
          lerpLEDStripAnimator1.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          lerpLEDStripAnimator1.setLerp(3, r, g, b, currR, currG, currB);
          currLedStripAnimators[2] = &lerpLEDStripAnimator1;
          
          //Serial.println("Animating flash...");
          //Serial.print("Flash to colour ("); Serial.print(r); Serial.print(","); 
          //Serial.print(g); Serial.print(","); Serial.print(b); Serial.print(") flash for ");
          //Serial.print(static_cast<float>(t)*10.0/1000.0); Serial.println(" seconds");
          
          break;
        }
        
        case TWO_FLASH_RGB_CHAR: {
          while (Serial.available() < 4);
          uint8_t r = CONVERT_RGB_BYTE(Serial.read());
          uint8_t g = CONVERT_RGB_BYTE(Serial.read());
          uint8_t b = CONVERT_RGB_BYTE(Serial.read());
          uint8_t t = Serial.read();
          
          uint8_t currR, currG, currB;
          getCurrentPixelColour(currR, currG, currB);
          
          clearLedStripAnimators();

          uint32_t holdTime = static_cast<uint32_t>(t * 10.0 / static_cast<float>(MIN_LED_FRAME_TIME_MS));
          holdTime = max(2, holdTime);
          
          lerpLEDStripAnimator0.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          lerpLEDStripAnimator0.setLerp(2, currR, currG, currB, r, g, b);
          currLedStripAnimators[0] = &lerpLEDStripAnimator0;
          
          constAnimator0.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          constAnimator0.setConst(holdTime, r, g, b);
          currLedStripAnimators[1] = &constAnimator0;
          
          constAnimator1.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          constAnimator1.setConst(holdTime, currR, currG, currB);
          currLedStripAnimators[2] = &constAnimator1;
          
          constAnimator2.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          constAnimator2.setConst(holdTime, r, g, b);
          currLedStripAnimators[3] = &constAnimator2;
          
          lerpLEDStripAnimator2.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          lerpLEDStripAnimator2.setLerp(3, r, g, b, currR, currG, currB);
          currLedStripAnimators[4] = &lerpLEDStripAnimator2;
          
          //Serial.println("Animating 2x flash...");
          //Serial.print("Flash to colour ("); Serial.print(r); Serial.print(","); 
          //Serial.print(g); Serial.print(","); Serial.print(b); Serial.print(") flash for ");
          //Serial.print(static_cast<float>(t)*10.0/1000.0); Serial.println(" seconds");
          
          break;
        }
        
        case THREE_FLASH_RGB_CHAR: {
          while (Serial.available() < 4);
          uint8_t r = CONVERT_RGB_BYTE(Serial.read());
          uint8_t g = CONVERT_RGB_BYTE(Serial.read());
          uint8_t b = CONVERT_RGB_BYTE(Serial.read());
          uint8_t t = Serial.read();
          
          uint8_t currR, currG, currB;
          getCurrentPixelColour(currR, currG, currB);
          
          clearLedStripAnimators();
          
          uint32_t holdTime = static_cast<uint32_t>(t * 10.0 / static_cast<float>(MIN_LED_FRAME_TIME_MS));
          holdTime = max(2, holdTime);
          
          lerpLEDStripAnimator0.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          lerpLEDStripAnimator0.setLerp(2, currR, currG, currB, r, g, b);
          currLedStripAnimators[0] = &lerpLEDStripAnimator0;
          
          constAnimator0.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          constAnimator0.setConst(holdTime, r, g, b);
          currLedStripAnimators[1] = &constAnimator0;
          
          constAnimator1.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          constAnimator1.setConst(holdTime, currR, currG, currB);
          currLedStripAnimators[2] = &constAnimator1;
          
          constAnimator2.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          constAnimator2.setConst(holdTime, r, g, b);
          currLedStripAnimators[3] = &constAnimator2;
          
          constAnimator3.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          constAnimator3.setConst(holdTime, currR, currG, currB);
          currLedStripAnimators[4] = &constAnimator3;
          
          constAnimator4.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          constAnimator4.setConst(holdTime, r, g, b);
          currLedStripAnimators[5] = &constAnimator4;
          
          lerpLEDStripAnimator2.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          lerpLEDStripAnimator2.setLerp(3, r, g, b, currR, currG, currB);
          currLedStripAnimators[6] = &lerpLEDStripAnimator2;
          
          //Serial.println("Animating 3x flash...");
          //Serial.print("Flash to colour ("); Serial.print(r); Serial.print(","); 
          //Serial.print(g); Serial.print(","); Serial.print(b); Serial.print(") flash for ");
          //Serial.print(static_cast<float>(t)*10.0/1000.0); Serial.println(" seconds");
          
          break;
        }
        
        case LERP_RGB_CHAR: {
          while (Serial.available() < 4);
          uint8_t r = CONVERT_RGB_BYTE(Serial.read());
          uint8_t g = CONVERT_RGB_BYTE(Serial.read());
          uint8_t b = CONVERT_RGB_BYTE(Serial.read());
          uint8_t t = Serial.read();
          
          uint8_t currR, currG, currB;
          getCurrentPixelColour(currR, currG, currB);
          
          lerpLEDStripAnimator0.setFrameDelay(MIN_LED_FRAME_TIME_MS);
          lerpLEDStripAnimator0.setLerp(static_cast<uint32_t>(t * 100.0 / static_cast<float>(MIN_LED_FRAME_TIME_MS)), currR, currG, currB, r, g, b);
          currLedStripAnimators[0] = &lerpLEDStripAnimator0;
        
          //Serial.println("Animating linear interpolation...");
          //Serial.print("Transition to colour ("); Serial.print(r); Serial.print(","); 
          //Serial.print(g); Serial.print(","); Serial.print(b); Serial.print(") over ");
          //Serial.print(static_cast<float>(t)/10.0); Serial.println(" seconds");

          break; 
        }
        
        case RED_BUTTON_CHAR:
        case ORANGE_BUTTON_CHAR: {
          
          // Handle different commands for the buttons
          ButtonAnimator* buttonAnim = &redButtonAnimator;
          if (serialReadByte == ORANGE_BUTTON_CHAR) {
            buttonAnim = &orangeButtonAnimator;
          }

          // What command is it?
          while (Serial.available() < 1);
          serialReadByte = Serial.read();
          
          if (serialReadByte == FLASH_RGB_CHAR) {
            while(Serial.available() < 1);
            uint8_t t = Serial.read();

            buttonAnim->setFrameDelay(MIN_LED_FRAME_TIME_MS);
            buttonAnim->setIsActive(true);
            buttonAnim->setNumFramesForFlash(static_cast<uint32_t>(t * 10.0 / static_cast<float>(MIN_LED_FRAME_TIME_MS)));
            
            //Serial.println("Animating button flashing...");
            //Serial.print("Flashing button every "); Serial.print(static_cast<float>(t)/100.0); Serial.println(" seconds");
          }
          else if (serialReadByte == TURN_OFF_BUTTON_CHAR) {
            buttonAnim->setIsActive(false);
            //Serial.println("Turning off button flashing...");
          }
          else {
            //Serial.println("No such button command was found.");
          }
          
          break;
        }

        default:
          //Serial.println("No such command was found.");
          break;    
      }
    }

    while (Serial.available() > 0 && Serial.peek() != '|') { Serial.read(); }
  }
}

void setAllPixels(uint8_t r, uint8_t g, uint8_t b) {
  for(int i = 0; i < NEOPIXEL_NUMPIXELS; i++){
    pixels.setPixelColor(i, pixels.Color(r,g,b)); // Moderately bright green color.
  }
  /*
  Serial.print("Setting pixel colour to (");
  Serial.print(r);
  Serial.print(", ");
  Serial.print(g);
  Serial.print(", ");
  Serial.print(b);
  Serial.println(")");
  */
}

void getCurrentPixelColour(uint8_t& r, uint8_t& g, uint8_t& b) {
  convertToRGB(pixels.getPixelColor(0), r, g, b);
}

void convertToRGB(uint32_t rgb, uint8_t& r, uint8_t& g, uint8_t& b) {
  r = (uint8_t)(rgb >> 16),
  g = (uint8_t)(rgb >>  8),
  b = (uint8_t)(0xFF & rgb);
}




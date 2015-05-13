#ifndef __ARCADESERIALCOMM_H__
#define __ARCADESERIALCOMM_H__

#include "serial/serial.h"
#include "../BlammoEngine/Colour.h"

class ArcadeSerialComm {
public:
    ArcadeSerialComm();
    ~ArcadeSerialComm();

    void OpenSerial(const std::string& serialPort);
    void CloseSerial();

    enum ButtonType { FireButton, BoostButton };
    enum ButtonGlowCadenceType { Off, Sustained, SlowButtonFlash, MediumButtonFlash, FastButtonFlash, VeryFastButtonFlash };

    void SetButtonCadence(ButtonType buttonType, ButtonGlowCadenceType cadence);
    
    enum TransitionTimeType { InstantTransition, VerySlowTransition, SlowTransition, MediumTransition, FastTransition, VeryFastTransition };
    void SetMarqueeColour(const Colour& c, TransitionTimeType timeType);

    enum MarqueeFlashType { VerySlowMarqueeFlash, SlowMarqueeFlash, MediumMarqueeFlash, FastMarqueeFlash, VeryFastMarqueeFlash };
    enum NumMarqueeFlashes { OneFlash, TwoFlashes, ThreeFlashes };
    void SetMarqueeFlash(const Colour& c, MarqueeFlashType flashType, NumMarqueeFlashes numFlashes = OneFlash, bool overridePrevFlashes = false);

private:
    serial::Serial* serialObj;

    unsigned long timeOfLastFlashInMs;
    unsigned long lastFlashLengthInMs;


    void SendSerial(const std::string& serialStr);
    void FindAndOpenSerial();

    DISALLOW_COPY_AND_ASSIGN(ArcadeSerialComm);
};


#endif // __ARCADESERIALCOMM_H__
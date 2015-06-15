#ifndef __ARCADESERIALCOMM_H__
#define __ARCADESERIALCOMM_H__

#include "serial/serial.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/Algebra.h"

class ArcadeSerialComm {
public:
    ArcadeSerialComm();
    ~ArcadeSerialComm();

    void OpenSerial(const std::string& serialPort);
    void CloseSerial();

    enum ButtonType { AllButtons, FireButton, BoostButton };
    enum ButtonGlowCadenceType { Off, Sustained, VerySlowButtonFlash, SlowButtonFlash, MediumButtonFlash, FastButtonFlash, VeryFastButtonFlash };

    void SetButtonCadence(ButtonType buttonType, ButtonGlowCadenceType cadence);
    ButtonGlowCadenceType GetCurrentFireButtonCadence() const { return this->currFireButtonCadence; }
    ButtonGlowCadenceType GetCurrentBoostButtonCadence() const { return this->currBoostButtonCadence; }
    
    enum TransitionTimeType { InstantTransition, VerySlowTransition, SlowTransition, MediumTransition, FastTransition, VeryFastTransition };
    void SetMarqueeColour(const Colour& c, TransitionTimeType timeType);

    enum MarqueeFlashType { VerySlowMarqueeFlash = 0, SlowMarqueeFlash, MediumMarqueeFlash, FastMarqueeFlash, VeryFastMarqueeFlash };
    static MarqueeFlashType RandomMarqueeFlashType() { return static_cast<MarqueeFlashType>(Randomizer::GetInstance()->RandomUnsignedInt() % 5); };
    enum NumMarqueeFlashes { OneFlash = 1, TwoFlashes, ThreeFlashes };
    static NumMarqueeFlashes RandomNumMarqueeFlashes() { return static_cast<NumMarqueeFlashes>(1 + (Randomizer::GetInstance()->RandomUnsignedInt() % 3)); };
    void SetMarqueeFlash(const Colour& c, MarqueeFlashType flashType, NumMarqueeFlashes numFlashes = OneFlash, bool overridePrevFlashes = false);

private:
    serial::Serial* serialObj;

    static const uint32_t BAUD_RATE;

    unsigned long timeOfLastFlashInMs;
    unsigned long lastFlashLengthInMs;

    ButtonGlowCadenceType currFireButtonCadence;
    ButtonGlowCadenceType currBoostButtonCadence;

    std::string GetSingleButtonCadenceSerialStr(ButtonType buttonType, ButtonGlowCadenceType cadence) const;
    void SendSerial(const std::string& serialStr);
    void FindAndOpenSerial();

    DISALLOW_COPY_AND_ASSIGN(ArcadeSerialComm);
};


#endif // __ARCADESERIALCOMM_H__
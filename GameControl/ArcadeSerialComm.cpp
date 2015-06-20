#include "ArcadeSerialComm.h"

#define CHECK_OPEN_TRY_AND_RETURN_ON_FAIL() if (this->serialObj == NULL) { return; } if (!this->serialObj->isOpen()) { this->serialObj->open(); if (!this->serialObj->isOpen()) { return; }} 

const uint32_t ArcadeSerialComm::BAUD_RATE = 38400;

ArcadeSerialComm::ArcadeSerialComm() : serialObj(NULL), timeOfLastFlashInMs(0), lastFlashLengthInMs(0),
currFireButtonCadence(Off), currBoostButtonCadence(Off) {
}

ArcadeSerialComm::~ArcadeSerialComm() {
    this->CloseSerial();
}

void ArcadeSerialComm::OpenSerial(const std::string& serialPort) { 
    if (serialPort == "") {
        this->FindAndOpenSerial();
    }
    else {
        try {
            this->CloseSerial();
            this->serialObj = new serial::Serial(serialPort, BAUD_RATE, serial::Timeout::simpleTimeout(0));
        }
        catch (const serial::SerialException& e) {
            debug_output(e.what());
        }
        catch (const serial::IOException& e) {
            debug_output(e.what());
        }
    }
}

void ArcadeSerialComm::CloseSerial() {
    if (this->serialObj != NULL) {
        this->serialObj->close();
        delete this->serialObj;
        this->serialObj = NULL;
    }
}

void ArcadeSerialComm::SetButtonCadence(ButtonType buttonType, ButtonGlowCadenceType cadence) {
    CHECK_OPEN_TRY_AND_RETURN_ON_FAIL();

    std::string serialStr = "";
    switch (buttonType) {
        case FireButton:
            this->currFireButtonCadence = cadence;
            serialStr = GetSingleButtonCadenceSerialStr(buttonType, cadence);
            break;
        case BoostButton:
            this->currBoostButtonCadence = cadence;
            serialStr = GetSingleButtonCadenceSerialStr(buttonType, cadence);
            break;
        case AllButtons:
            this->currBoostButtonCadence = this->currFireButtonCadence = cadence;
            serialStr += GetSingleButtonCadenceSerialStr(FireButton, cadence);
            serialStr += GetSingleButtonCadenceSerialStr(BoostButton, cadence);
            break;
        default:
            assert(false);
            return;
    }

    this->SendSerial(serialStr);
}

std::string ArcadeSerialComm::GetSingleButtonCadenceSerialStr(ButtonType buttonType, ButtonGlowCadenceType cadence) const {
    std::stringstream serialPkt;
    serialPkt << "|";

    switch (buttonType) {
        case FireButton:
            serialPkt << "R";
            break;
        case BoostButton:
            serialPkt << "O";
            break;
        default:
            assert(false);
            return "";
    }

    switch (cadence) {
        case Off:
            serialPkt << "XX";
            break;

        case Sustained:
            serialPkt << "F" << static_cast<char>(0);
            break;

        case VerySlowButtonFlash:
            serialPkt << "F" << static_cast<char>(127);
            break;

        case SlowButtonFlash:
            serialPkt << "F" << static_cast<char>(100);
            break;

        case MediumButtonFlash:
            serialPkt << "F" << static_cast<char>(85);
            break;

        case FastButtonFlash:
            serialPkt << "F" << static_cast<char>(48);
            break;

        case VeryFastButtonFlash:
            serialPkt << "F" << static_cast<char>(32);
            break;

        default:
            assert(false);
            return "";
    }

    // Filler for stable packet size
    serialPkt << "XX";

    return serialPkt.str();
}

void ArcadeSerialComm::SetMarqueeColour(const Colour& c, TransitionTimeType timeType) {
    CHECK_OPEN_TRY_AND_RETURN_ON_FAIL();

    char r = static_cast<char>(c.R() * 127);
    char g = static_cast<char>(c.G() * 127);
    char b = static_cast<char>(c.B() * 127);

    std::stringstream serialPkt;
    serialPkt << "|";

    switch (timeType) {
        case InstantTransition:
            serialPkt << "A" << r << g << b << "X";
            break;

        case VerySlowTransition:
            serialPkt << "L" << r << g << b << static_cast<char>(120);
            break;
        case SlowTransition:
            serialPkt << "L" << r << g << b << static_cast<char>(80);
            break;
        case MediumTransition:
            serialPkt << "L" << r << g << b << static_cast<char>(40);
            break;
        case FastTransition:
            serialPkt << "L" << r << g << b << static_cast<char>(15);
            break;
        case VeryFastTransition:
            serialPkt << "L" << r << g << b << static_cast<char>(9);
            break;
        default:
            assert(false);
            return;
    }

    this->SendSerial(serialPkt.str());
}

void ArcadeSerialComm::SetMarqueeFlash(const Colour& c, MarqueeFlashType flashType, 
                                       NumMarqueeFlashes numFlashes, bool overridePrevFlashes) {
    CHECK_OPEN_TRY_AND_RETURN_ON_FAIL();

    // If we're already flashing then don't flash until we're finished
    unsigned long currTime = BlammoTime::GetSystemTimeInMillisecs();
    if (!overridePrevFlashes && this->lastFlashLengthInMs > currTime - this->timeOfLastFlashInMs) {
        return;
    }

    char r = static_cast<char>(c.R() * 127);
    char g = static_cast<char>(c.G() * 127);
    char b = static_cast<char>(c.B() * 127);

    std::stringstream serialPkt;
    serialPkt << "|";

    switch (numFlashes) {
        case OneFlash:
            serialPkt << "F";
            break;
        case TwoFlashes:
            serialPkt << "2";
            break;
        case ThreeFlashes:
            serialPkt << "3";
            break;
        default:
            break;
    }

    switch (flashType) {
        case VerySlowMarqueeFlash: {
            static const uint8_t VERY_SLOW_TIME = 80;
            serialPkt << r << g << b << static_cast<char>(VERY_SLOW_TIME);
            this->lastFlashLengthInMs = VERY_SLOW_TIME*10;
            break;
        }
        case SlowMarqueeFlash: {
            static const uint8_t SLOW_TIME = 50;
            serialPkt << r << g << b << static_cast<char>(SLOW_TIME);
            this->lastFlashLengthInMs = SLOW_TIME*10;
            break;
        }
        case MediumMarqueeFlash: {
            static const uint8_t MED_TIME = 30;
            serialPkt << r << g << b << static_cast<char>(MED_TIME);
            this->lastFlashLengthInMs = MED_TIME*10;
            break;
        }
        case FastMarqueeFlash: {
            static const uint8_t FAST_TIME = 20;
            serialPkt << r << g << b << static_cast<char>(FAST_TIME);
            this->lastFlashLengthInMs = FAST_TIME*10;
            break;
        }
        case VeryFastMarqueeFlash: {
            static const uint8_t VERY_FAST_TIME = 1;
            serialPkt << r << g << b << static_cast<char>(VERY_FAST_TIME);
            this->lastFlashLengthInMs = VERY_FAST_TIME*10;
            break;
        }

        default:
            assert(false);
            return;
    }

    this->SendSerial(serialPkt.str());
    this->timeOfLastFlashInMs = BlammoTime::GetSystemTimeInMillisecs();
}

void ArcadeSerialComm::SendSerial(const std::string& serialStr) {
    try {
        debug_output("Sending arcade serial: " << serialStr);
        this->serialObj->write(serialStr);
    }
    catch (const serial::IOException& e) {
        debug_output(e.what());
    }
    catch (const serial::PortNotOpenedException& e) {
        debug_output(e.what());
    }
    catch (const serial::SerialException& e) {
        debug_output(e.what());
    }
}

void ArcadeSerialComm::FindAndOpenSerial() {
    std::string expectedStr = "ARCADEARDUINO";
    std::string readStr;

    std::vector<serial::PortInfo> portList = serial::list_ports();
    for (std::vector<serial::PortInfo>::const_iterator iter = portList.begin(); iter != portList.end(); ++iter) {
        const serial::PortInfo& currPort = *iter;
        this->CloseSerial();
        try {
            this->serialObj = new serial::Serial(currPort.port, BAUD_RATE, serial::Timeout::simpleTimeout(3000));
        }
        catch (const serial::SerialException& e) {
            debug_output(e.what());
            this->serialObj = NULL;
            continue;
        }
        catch (const serial::IOException& e) {
            debug_output(e.what());
            this->serialObj = NULL;
            continue;
        }

        // Check to see if the serial port is the correct one...
        if (this->serialObj->isOpen()) {
            this->serialObj->write("|QQQQQ");
            this->serialObj->read(readStr, expectedStr.size());
            if (readStr == expectedStr) {
                this->serialObj->setTimeout(serial::Timeout::max(), 0, 0, 0, 0);
                return;
            }
        }
    }

}
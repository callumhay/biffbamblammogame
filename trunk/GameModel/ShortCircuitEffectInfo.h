/**
 * ShortCircuitEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __SHORTCIRCUITEFFECTINFO_H__
#define __SHORTCIRCUITEFFECTINFO_H__

#include "GeneralEffectEventInfo.h"

class ShortCircuitEffectInfo : public GeneralEffectEventInfo {
public:
    ShortCircuitEffectInfo( const Point2D& position, const Colour& brightColour,
        const Colour& medColour, const Colour& darkColour, double size, double timeInSecs):
        position(position), brightColour(brightColour), medColour(medColour), darkColour(darkColour),
        size(size), timeInSecs(timeInSecs) {};

    ~ShortCircuitEffectInfo() {};

    const Point2D& GetPosition() const { return this->position; }
    const Colour& GetBrightColour() const { return this->brightColour; }
    const Colour& GetMediumColour() const { return this->medColour; }
    const Colour& GetDarkColour() const { return this->darkColour; }
    double GetSize() const { return this->size; }
    double GetTimeInSecs() const { return this->timeInSecs; }

    GeneralEffectEventInfo::Type GetType() const { return GeneralEffectEventInfo::ShortCircuit; }

private:
    const Point2D position;     // Position in level space where the effect should occur
    const Colour brightColour;
    const Colour medColour;
    const Colour darkColour;
    const double size;
    const double timeInSecs;


    DISALLOW_COPY_AND_ASSIGN(ShortCircuitEffectInfo);
};

#endif // __SHORTCIRCUITEFFECTINFO_H__
/**
 * StarSmashEffectInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __STARSMASHEFFECTINFO_H__
#define __STARSMASHEFFECTINFO_H__

#include "GeneralEffectEventInfo.h"
#include "Onomatoplex.h"

class StarSmashEffectInfo : public GeneralEffectEventInfo {
public:
    StarSmashEffectInfo(const Point2D& position, const Vector2D& dir, double size, double timeInSecs,
        Onomatoplex::Extremeness extremeness): position(position), direction(dir), size(size),
        timeInSecs(timeInSecs), extremeness(extremeness) {};

    ~StarSmashEffectInfo() {};

    const Point2D& GetPosition() const { return this->position; }
    const Vector2D& GetDirection() const { return this->direction; }
    double GetSize() const { return this->size; }
    double GetTimeInSecs() const { return this->timeInSecs; }
    Onomatoplex::Extremeness GetExtremeness() const { return this->extremeness; }

    GeneralEffectEventInfo::Type GetType() const { return GeneralEffectEventInfo::StarSmash; }

private:
    const Point2D position;     // Position in level space where the effect should occur
    const Vector2D direction;
    const double size;
    const double timeInSecs;
    const Onomatoplex::Extremeness extremeness;

    DISALLOW_COPY_AND_ASSIGN(StarSmashEffectInfo);
};

#endif // __STARSMASHEFFECTINFO_H__
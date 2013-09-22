/**
 * GeneralEffectEventInfo.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the
 * resulting work only under the same or similar license to this one.
 */

#ifndef __GENERALEFFECTEVENTINFO_H__
#define __GENERALEFFECTEVENTINFO_H__

class GeneralEffectEventInfo {
public:
    GeneralEffectEventInfo() {};
    virtual ~GeneralEffectEventInfo() {};

    enum Type { ShortCircuit, LevelShake };

    virtual GeneralEffectEventInfo::Type GetType() const = 0;

private:
    DISALLOW_COPY_AND_ASSIGN(GeneralEffectEventInfo);
};

#endif // __GENERALEFFECTEVENTINFO_H__
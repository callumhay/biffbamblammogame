/**
 * BBBTitleDisplay.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __BBBTITLEDISPLAY_H__
#define __BBBTITLEDISPLAY_H__

#include "../BlammoEngine/TextLabel.h"
#include "../ESPEngine/ESPPointEmitter.h"

/**
 * Class for displaying the emitters required to show the full
 * "Biff! Bam!! Blammo!?!" title.
 */
class BBBTitleDisplay {
public:
	static const char* TITLE_BIFF_TEXT;
	static const char* TITLE_BAM_TEXT;
	static const char* TITLE_BLAMMO_TEXT;

    BBBTitleDisplay(float scale);
    ~BBBTitleDisplay();

    void Draw(float x, float y, const Camera& camera);
    float GetBlammoWidth() const { return this->blammoWidth; }

    float GetTotalWidth() const;
    float GetTotalHeight() const;

private:
    float scale;
    std::vector<Texture*> bangTextures;
    
	ESPPointEmitter biffEmitter, bamEmitter, blammoEmitter;
	ESPPointEmitter biffTextEmitter, bamTextEmitter, blammoTextEmitter;
    
    float blammoWidth;

    DISALLOW_COPY_AND_ASSIGN(BBBTitleDisplay);
};

inline float BBBTitleDisplay::GetTotalWidth() const {
    return this->scale*11.0f + this->blammoTextEmitter.GetParticleSizeX().maxValue;
}

inline float BBBTitleDisplay::GetTotalHeight() const {
    return this->scale*6.5f + this->blammoTextEmitter.GetParticleSizeY().maxValue;
}

#endif // __BBBTITLEDISPLAY_H__
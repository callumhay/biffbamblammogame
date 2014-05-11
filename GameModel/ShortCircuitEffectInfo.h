/**
 * ShortCircuitEffectInfo.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
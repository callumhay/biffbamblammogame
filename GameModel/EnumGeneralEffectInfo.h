/**
 * EnumGeneralEffectInfo.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __ENUMGENERALEFFECTINFO_H__
#define __ENUMGENERALEFFECTINFO_H__

class EnumGeneralEffectInfo : public GeneralEffectEventInfo {
public:
    enum SpecificEffectType {
        FuturismBarrierBlockDisintegrationEffect  // Added effect that occurs when the futurism boss disintegrates a block, at the site of the block
    };

    EnumGeneralEffectInfo(EnumGeneralEffectInfo::SpecificEffectType specificType) : GeneralEffectEventInfo(), specificType(specificType), position(0,0,0),
    size(0,0), direction(0,0), colour(0,0,0), timeInSecs(0) {}
    virtual ~EnumGeneralEffectInfo() {}

    GeneralEffectEventInfo::Type GetType() const { return GeneralEffectEventInfo::EnumEffect; }
    EnumGeneralEffectInfo::SpecificEffectType GetSpecificType() const { return this->specificType; }

    void SetPosition(const Point3D& pos) { this->position = pos; }
    void SetPosition(const Point2D& pos) { this->position[0] = pos[0]; this->position[1] = pos[1]; }
    const Point3D& GetPosition() const { return this->position; }

    void SetSize2D(const Vector2D& size) { this->size = size; }
    void SetSize2D(float xSize, float ySize) { this->size[0] = xSize; this->size[1] = ySize; }
    void SetSize1D(float size) { this->size[0] = this->size[1] = size; }
    const Vector2D& GetSize2D() const { return this->size; }
    float GetSize1D() const { return this->size[0]; }

    void SetDirection(const Vector2D& dir) { this->direction = Vector2D::Normalize(dir); }
    const Vector2D& GetDirection() const { return this->direction; }

    void SetColour(const Colour& colour) { this->colour = colour; }
    const Colour& GetColour() const { return this->colour; }

    void SetTimeInSecs(double t) { this->timeInSecs = t; }
    double GetTimeInSecs() const { return this->timeInSecs; }

private:
    SpecificEffectType specificType;

    Point3D position;
    Vector2D size;
    Vector2D direction;
    Colour colour;
    double timeInSecs;

    DISALLOW_COPY_AND_ASSIGN(EnumGeneralEffectInfo);
};

#endif // __ENUMGENERALEFFECTINFO_H__

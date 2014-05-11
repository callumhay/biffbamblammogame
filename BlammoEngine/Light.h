/**
 * Light.h
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

#ifndef _LIGHT_H__
#define _LIGHT_H__

#include "BasicIncludes.h"
#include "Point.h"
#include "Vector.h"
#include "Colour.h"
#include "Animation.h"

#include "GeometryMaker.h"

// A simpler less stack/heap intensive version of the PointLight
class BasicPointLight {
public:
	BasicPointLight() {}
	BasicPointLight(const Point3D& pos, const Colour& diffuse, float linAtten) : 
			position(pos), diffuseColour(diffuse), linearAttenuation(linAtten) {}

    BasicPointLight(const BasicPointLight& copy);

	inline const Colour& GetDiffuseColour() const { return this->diffuseColour; }
	inline const Point3D& GetPosition() const { return this->position; }
	inline float GetLinearAttenuation() const { return this->linearAttenuation; }

	inline void SetDiffuseColour(const Colour& c) { this->diffuseColour = c; }
	inline void SetPosition(const Point3D& p) { this->position = p; }
	inline void SetLinearAttenuation(float a) { this->linearAttenuation = a; }

    BasicPointLight& operator=(const BasicPointLight& copy);

private:
	Colour diffuseColour;
	Point3D position;
	float linearAttenuation;
};

inline BasicPointLight::BasicPointLight(const BasicPointLight& copy) :
diffuseColour(copy.diffuseColour), position(copy.position), linearAttenuation(copy.linearAttenuation) {
}

inline BasicPointLight& BasicPointLight::operator=(const BasicPointLight& copy) {
    if (this == &copy) {
        return *this;
    }

    this->diffuseColour = copy.diffuseColour;
    this->position = copy.position;
    this->linearAttenuation = copy.linearAttenuation;

    return *this;
}


// Full animating, state-tracking point light
class PointLight {
public:
	PointLight() : position(0,0,0), currDiffuseColour(0,0,0), linearAttenuation(0), isOn(true), lightIntensity(1.0f) {}
	
	PointLight(const Point3D& pos, const Colour& diffuse, float linAtten) : 
		position(pos), onDiffuseColour(diffuse), currDiffuseColour(diffuse), linearAttenuation(linAtten), isOn(true), lightIntensity(1.0f) {}
	
	~PointLight() {}

	void CopyBasicAttributes(PointLight& copyToMe) const;
	void ConvertToBasicPointLight(BasicPointLight& basicPtLight) const;

	inline const Colour& GetDiffuseColour() const { return this->currDiffuseColour; }
	inline const Point3D& GetPosition() const { return this->position; }
	inline float GetLinearAttenuation() const { return this->linearAttenuation; }
	inline bool GetIsTurnedOn() const { return this->isOn; }
	
	inline void SetDiffuseColour(const Colour& c) { this->onDiffuseColour = c; }
	inline void SetPosition(const Point3D& p) { this->position = p; }
	inline void SetLinearAttenuation(float a) { this->linearAttenuation = a; }

    void SetFromBasicPointLight(const BasicPointLight& light);

	void SetLightOn(bool turnOn, double animationTime);
	void SetLightStrobeOn(const Colour& strobeEndColour, float strobeTime);
	void SetLightStrobeOff();
	void SetLightColourChange(const Colour& newColour, float changeTime);
	void SetLightPositionChange(const Point3D& newPosition, float changeTime);
    void SetLinearAttenuationChange(float newAtten, float changeTime);
	void Tick(double dT);

	inline void DebugDraw() const {
		glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		glColor4f(this->onDiffuseColour.R(), this->onDiffuseColour.G(), this->onDiffuseColour.B(), 0.4f);
		
		// Draw quad at this light's location...
		glPushMatrix();
		glTranslatef(this->position[0], this->position[1], this->position[2]);
		GeometryMaker::GetInstance()->DrawCube();
		glPopMatrix();

		glPopAttrib();
	}

private:
	bool isOn;
	float lightIntensity;			// The current intensity of the light
	Colour onDiffuseColour;		// The colour of the light's diffuse when it's turned on completely
	Colour currDiffuseColour;	// The current diffuse colour of the light

	Point3D position;
	float linearAttenuation;

	std::list<AnimationMultiLerp<float> > lightIntensityAnim;
	std::list<AnimationMultiLerp<Colour> > lightColourChangeAnim;
	std::list<AnimationMultiLerp<Colour> > lightColourStrobeAnim;
	std::list<AnimationMultiLerp<Point3D> > lightPositionAnim;
    std::list<AnimationMultiLerp<float> > lightAttenAnim;

	// Disallow copy and assignment
	PointLight(const PointLight& p);
	PointLight& operator=(const PointLight& p);

};

inline void PointLight::CopyBasicAttributes(PointLight& copyToMe) const {
	copyToMe.SetPosition(this->GetPosition());
	copyToMe.SetDiffuseColour(this->GetDiffuseColour());
	copyToMe.SetLinearAttenuation(this->GetLinearAttenuation());
}

inline void PointLight::ConvertToBasicPointLight(BasicPointLight& basicPtLight) const {
	basicPtLight.SetDiffuseColour(this->GetDiffuseColour());
	basicPtLight.SetPosition(this->GetPosition());
	basicPtLight.SetLinearAttenuation(this->GetLinearAttenuation());
}

inline void PointLight::SetFromBasicPointLight(const BasicPointLight& light) {
    this->SetDiffuseColour(light.GetDiffuseColour());
    this->SetPosition(light.GetPosition());
    this->SetLinearAttenuation(light.GetLinearAttenuation());
}

#endif
/**
 * Light.cpp
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

#include "Light.h"
#include <vector>

/**
 * Toggle the light on or off and fade it on or off over the given animation time frame.
 */ 
void PointLight::SetLightOn(bool turnOn, double animationTime) {
	// Clear any previous light intensity animation
	this->lightIntensityAnim.clear();

	float finalIntensity;

	// Figure out the final colour based on whether we are turning the light on or off
	if (turnOn) {
		finalIntensity = 1.0f;
	}
	else {
		finalIntensity = 0.0f;
	}

	// Create the animation for the light intensity
	AnimationMultiLerp<float> intensityAnim(&this->lightIntensity);
	intensityAnim.SetLerp(animationTime, finalIntensity);
	intensityAnim.SetRepeat(false);

	// Add the animation to this object's animation set
	this->lightIntensityAnim.push_back(intensityAnim);

	// Set whether the light is now on/off
	this->isOn = turnOn;
}

void PointLight::SetLightStrobeOn(const Colour& strobeEndColour, float strobeTime){
	
	// Time values for the strobe animation
	std::vector<double> timeVals;
	timeVals.reserve(3);
	timeVals.push_back(0.0);
	timeVals.push_back(strobeTime);
	timeVals.push_back(2.0 * strobeTime);	

	// Colour values for the strobe animation
	std::vector<Colour> colourVals;
	colourVals.reserve(3);
	colourVals.push_back(this->onDiffuseColour);
	colourVals.push_back(strobeEndColour);
	colourVals.push_back(this->onDiffuseColour);

	// Clear any previous light strobe animation
	this->lightColourStrobeAnim.clear();
	
	// Create the strobe animation
	AnimationMultiLerp<Colour> strobeAnim(this->onDiffuseColour);
	strobeAnim.SetLerp(timeVals, colourVals);
	strobeAnim.SetRepeat(true);

	// Add the strobe animation the light strobe animations
	this->lightColourStrobeAnim.push_back(strobeAnim);
}

/**
 * Turn off the strobe effect - this will just turn off the repeat of the animation
 * and the light will naturally return to its original colour.
 */
void PointLight::SetLightStrobeOff() {
	for (std::list<AnimationMultiLerp<Colour> >::iterator animIter = this->lightColourStrobeAnim.begin(); animIter != this->lightColourStrobeAnim.end(); ++animIter) {
		animIter->SetRepeat(false);
	}
}

/**
 * Change the colour of this light over the given time to the new colour.
 * This change will not stop the strobing of the light but it will affect the colour
 * in the strobe. This will also not do anything noticible until the lights are turned on.
 */
void PointLight::SetLightColourChange(const Colour& newColour, float changeTime) {
	// Clear any previous light colour animation
	this->lightColourChangeAnim.clear();

	// Create the animation for the light colour
	// NOTE: The reason we don't pass a reference here is because we need to change
	// the diffuse colour in the case of a strobe as well - see Tick function.
	AnimationMultiLerp<Colour> diffuseAnim(this->onDiffuseColour);
	diffuseAnim.SetLerp(changeTime, newColour);
	diffuseAnim.SetRepeat(false);

	// Add the animation to this object's animation set
	this->lightColourChangeAnim.push_back(diffuseAnim);
}

/**
 * Change the position of this light over the given time to the new position.
 */
void PointLight::SetLightPositionChange(const Point3D& newPosition, float changeTime) {
	this->lightPositionAnim.clear();

	AnimationMultiLerp<Point3D> positionAnim(&this->position);
	positionAnim.SetLerp(changeTime, newPosition);
	positionAnim.SetRepeat(false);

	this->lightPositionAnim.push_back(positionAnim);
}

void PointLight::SetLinearAttenuationChange(float newAtten, float changeTime) {
    this->lightAttenAnim.clear();

    AnimationMultiLerp<float> attenAnim(&this->linearAttenuation);
    attenAnim.SetLerp(changeTime, newAtten);
    attenAnim.SetRepeat(false);

    this->lightAttenAnim.push_back(attenAnim);
}

/**
 * Ticks away at any active aniamtions.
 */
void PointLight::Tick(double dT) {
	bool isFinished = false;

	// Tick position animaton
	for (std::list<AnimationMultiLerp<Point3D> >::iterator animIter = this->lightPositionAnim.begin();
         animIter != this->lightPositionAnim.end();) {

			isFinished = animIter->Tick(dT);
			if (isFinished) {
				animIter = this->lightPositionAnim.erase(animIter);
			}
			else {
				++animIter;
			}
	}

	// Tick the light animations that turn the light on or off (if any)
	for (std::list<AnimationMultiLerp<float> >::iterator animIter = this->lightIntensityAnim.begin();
         animIter != this->lightIntensityAnim.end();) {

			isFinished = animIter->Tick(dT);
			if (isFinished) {
				animIter = this->lightIntensityAnim.erase(animIter);
			}
			else {
				++animIter;
			}
	}

	// Tick the light-on/light colour change animations (if any)
	Colour newDiffuseColourChange = this->onDiffuseColour;
	for (std::list<AnimationMultiLerp<Colour> >::iterator animIter = this->lightColourChangeAnim.begin();
         animIter != this->lightColourChangeAnim.end();) {

			isFinished = animIter->Tick(dT);
			newDiffuseColourChange = animIter->GetInterpolantValue();
			if (isFinished) {
				this->onDiffuseColour = newDiffuseColourChange;
				animIter = this->lightColourChangeAnim.erase(animIter);
			}
			else {
				++animIter;
			}
	}

    // Tick the attenuation change animations (if any)
    for (std::list<AnimationMultiLerp<float> >::iterator animIter = this->lightAttenAnim.begin(); 
         animIter != this->lightAttenAnim.end();) {

        isFinished = animIter->Tick(dT);
        if (isFinished) {
            animIter = this->lightAttenAnim.erase(animIter);
        }
        else {
            ++animIter;
        }
    }

	// Special case: Light is strobing AND changing colour...
	if (this->lightColourStrobeAnim.size() > 0) {
		
		// We first of all change the strobe animation to accommodate the change in light colour, then
		// we tick the strobe animation and use its colour
		this->lightColourStrobeAnim.front().SetInitialInterpolationValue(newDiffuseColourChange);
		this->lightColourStrobeAnim.front().SetFinalInterpolationValue(newDiffuseColourChange);
	

	    for (std::list<AnimationMultiLerp<Colour> >::iterator animIter = this->lightColourStrobeAnim.begin(); 
             animIter != this->lightColourStrobeAnim.end();) {

		    isFinished = animIter->Tick(dT);
		    newDiffuseColourChange = animIter->GetInterpolantValue();
		    if (isFinished) {
			    animIter = this->lightColourStrobeAnim.erase(animIter);
		    }
		    else {
			    ++animIter;
		    }
	    }
    }

	this->currDiffuseColour = this->lightIntensity * newDiffuseColourChange;
}
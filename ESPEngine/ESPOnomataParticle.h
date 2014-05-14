/**
 * ESPOnomataParticle.h
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

#ifndef __ESPONOMATAPARTICLE_H__
#define __ESPONOMATAPARTICLE_H__

#include "../BlammoEngine/Texture2D.h"
#include "../BlammoEngine/Matrix.h"
#include "../BlammoEngine/Vector.h"
#include "../BlammoEngine/Point.h"
#include "../BlammoEngine/Colour.h"
#include "../BlammoEngine/TextLabel.h"

#include "../GameModel/Onomatoplex.h"

#include "ESPUtil.h"
#include "ESPParticle.h"

class TextureFontSet;

class ESPOnomataParticle : public ESPParticle {
public: 
	ESPOnomataParticle(const TextureFontSet* font, const std::string& text);
	ESPOnomataParticle(const TextureFontSet* font);
	~ESPOnomataParticle();

	void Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan);
	void Tick(const double dT);
	void Draw(const Matrix4x4& modelMat, const Matrix4x4& modelMatInv, const Matrix4x4& modelInvTMat, 
        const Camera& camera, const ESP::ESPAlignment& alignment);

	void SetDropShadow(const DropShadow& ds) {
		this->dropShadow = ds;
		this->dropShadow.isSet = true;
	}
	void SetOnomatoplexSound(Onomatoplex::SoundType st, Onomatoplex::Extremeness e) {
		this->soundType = st;
		this->extremeness = e;
		this->GenerateNewString();
	}

	Onomatoplex::SoundType GetSoundType() const { return this->soundType; }
	Onomatoplex::Extremeness GetSoundExtremeness() const { return this->extremeness; }

protected:
	bool useGivenString;

	const TextureFontSet*		 font;
	Onomatoplex::SoundType	 soundType;
	Onomatoplex::Extremeness extremeness;
	DropShadow dropShadow;

	std::string currStr;
	float currHalfStrWidth;
	float currHalfStrHeight;

	void SetRandomSoundType();
	void SetRandomExtremeness();
	void GenerateNewString();

private:
	DISALLOW_COPY_AND_ASSIGN(ESPOnomataParticle);
};

inline void ESPOnomataParticle::GenerateNewString() {
	assert(!this->useGivenString);
	this->currStr = Onomatoplex::Generator::GetInstance()->Generate(this->soundType, this->extremeness);
	this->currHalfStrWidth = TextureFontSet::TEXT_3D_SCALE * this->font->GetWidth(this->currStr) * 0.5f;
}
inline void ESPOnomataParticle::SetRandomSoundType() {
	assert(!this->useGivenString);
	unsigned int soundTypeRnd		= Randomizer::GetInstance()->RandomUnsignedInt() % Onomatoplex::NumSoundTypes;
	this->soundType		= static_cast<Onomatoplex::SoundType>(soundTypeRnd);
}
inline void ESPOnomataParticle::SetRandomExtremeness() {
	assert(!this->useGivenString);
	unsigned int extremenessRnd	= Randomizer::GetInstance()->RandomUnsignedInt() % Onomatoplex::NumExtremenessTypes;
	this->extremeness	= static_cast<Onomatoplex::Extremeness>(extremenessRnd);
}

/**
 * Revive this particle with the given lifespan length in seconds.
 */
inline void ESPOnomataParticle::Revive(const Point3D& pos, const Vector3D& vel, const Vector2D& size, float rot, float totalLifespan) {
	// Set the members to reflect a 'new life'
	ESPParticle::Revive(pos, vel, size, rot, totalLifespan);
	
	if (!this->useGivenString) {
		this->GenerateNewString();
	}
}

/**
 * Called each frame with the delta time for that frame, this will
 * provide a slice of the lifetime of the particle.
 */
inline void ESPOnomataParticle::Tick(const double dT) {
	ESPParticle::Tick(dT);
}


#endif
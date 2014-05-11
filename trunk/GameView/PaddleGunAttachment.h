/**
 * PaddleGunAttachment.h
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

#ifndef __PADDLEGUNATTACHMENT_H__
#define __PADDLEGUNATTACHMENT_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Mesh.h"

#include "../GameModel/PlayerPaddle.h"

/**
 * Helper class (similar to StickyPaddleGoo) for holding the mesh,
 * materials and effects used for the various paddle gun-types -
 * visible when the user has a paddle gun-type item active.
 */
class PaddleGunAttachment {
public:
	PaddleGunAttachment();
	~PaddleGunAttachment();

	/**
	 * Draws the paddle laser gun attachment - also is responsible for
	 * animating the attachment and setting the appropriate transformations.
	 */
	inline void Draw(double dT, const PlayerPaddle& p, const Camera& camera, CgFxEffectBase* replacementMat,
                     const BasicPointLight& keyLight, const BasicPointLight& fillLight, const BasicPointLight& ballLight) {
		float paddleScaleFactor = p.GetPaddleScaleFactor();
      
		this->gunRecoilAnim.Tick(dT);

		glPushMatrix();
		glTranslatef(0, 0, this->gunRecoilAnim.GetInterpolantValue());	// Animate recoil on fire
		glScalef(paddleScaleFactor, paddleScaleFactor, paddleScaleFactor);
        glRotatef(p.GetZRotation(), 0, 0, 1);
		this->gunMesh->Draw(camera, replacementMat, keyLight, fillLight, ballLight);
		glPopMatrix();
	}

	void FirePaddleGun(const PlayerPaddle& paddle);

private:
	Mesh* gunMesh;
	AnimationMultiLerp<float> gunRecoilAnim;

    DISALLOW_COPY_AND_ASSIGN(PaddleGunAttachment);
};
#endif // __PADDLEGUNATTACHMENT_H__
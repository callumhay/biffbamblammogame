/**
 * PaddleMineLauncher.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __PADDLEMINELAUNCHER_H__
#define __PADDLEMINELAUNCHER_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Mesh.h"

class PlayerPaddle;

class PaddleMineLauncher {
public:
    PaddleMineLauncher();
    ~PaddleMineLauncher();

	/**
	 * Draws the paddle mine launcher gun attachment - also is responsible for
	 * animating the attachment and setting the appropriate transformations.
	 */
	inline void Draw(double dT, const PlayerPaddle& p, const Camera& camera, CgFxEffectBase* replacementMat,
                     const BasicPointLight& paddleKeyLight, const BasicPointLight& paddleFillLight, 
                     const BasicPointLight& ballLight) {

        this->paddleMineAttachment->Draw(camera, replacementMat, paddleKeyLight, paddleFillLight, ballLight);
        this->DrawEffects(dT, p);
	}

    void Activate();
    void Deactivate();
    
	void FireMine();

private:
    bool isActive;
	Mesh* paddleMineAttachment; // Mine launcher attachment mesh for paddle

    AnimationMultiLerp<float> flickerAnim;
    AnimationMultiLerp<float> pulseAlphaAnim;
    
    void DrawEffects(double dT, const PlayerPaddle& paddle);

    DISALLOW_COPY_AND_ASSIGN(PaddleMineLauncher);
};

#endif // __PADDLEMINELAUNCHER_H__
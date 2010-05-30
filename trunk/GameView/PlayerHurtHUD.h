#ifndef __PLAYERHURTHUD_H__
#define __PLAYERHURTHUD_H__

#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Texture.h"

class PlayerHurtHUD {
public:
	PlayerHurtHUD();
	~PlayerHurtHUD();

	enum PainIntensity { MinorPain, ModeratePain, MajorPain };

	void Activate(PlayerHurtHUD::PainIntensity intensity);
	void Deactivate();

	void Draw(double dT, int displayWidth, int displayHeight);

private:
	bool isActive;	// Whether the hurt HUD is active or not
	//Texture* hurtOverlayTex;

	AnimationMultiLerp<float> fadeAnimation;	// Fades the hurting HUD in and out when activated

};

#endif // __PLAYERHURTHUD_H__
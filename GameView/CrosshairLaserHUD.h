/**
 * CrosshairLaserHUD.h
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

#ifndef __CROSSHAIRLASERHUD_H__
#define __CROSSHAIRLASERHUD_H__

#include "../BlammoEngine/BasicIncludes.h"
#include "../BlammoEngine/Animation.h"
#include "../BlammoEngine/Vector.h"

class Texture;
class PlayerPaddle;

class CrosshairLaserHUD {
public:
	CrosshairLaserHUD();
	~CrosshairLaserHUD();

	void Draw(const PlayerPaddle* paddle, int screenWidth, int screenHeight, float alpha) const;
	void Tick(double dT);

private:
	static const int LASER_BULLET_CROSSHAIR_WIDTH					= 128;
	static const int LASER_BULLET_CROSSHAIR_HEIGHT				= LASER_BULLET_CROSSHAIR_WIDTH;
	static const int LASER_BULLET_CROSSHAIR_HALF_WIDTH		= LASER_BULLET_CROSSHAIR_WIDTH / 2;
	static const int LASER_BULLET_CROSSHAIR_HALF_HEIGHT		= LASER_BULLET_CROSSHAIR_HEIGHT / 2;

	static const int LASER_BEAM_CROSSHAIR_WIDTH					= 256;
	static const int LASER_BEAM_CROSSHAIR_HEIGHT				= LASER_BEAM_CROSSHAIR_WIDTH;
	static const int LASER_BEAM_CROSSHAIR_HALF_WIDTH		= LASER_BEAM_CROSSHAIR_WIDTH / 2;
	static const int LASER_BEAM_CROSSHAIR_HALF_HEIGHT		= LASER_BEAM_CROSSHAIR_HEIGHT / 2;
	static const int LASER_BEAM_GLOW_HALF_WIDTH					= LASER_BEAM_CROSSHAIR_HALF_WIDTH / 1.5;
	static const int LASER_BEAM_GLOW_HALF_HEIGHT				= LASER_BEAM_CROSSHAIR_HALF_HEIGHT / 1.5;

	Texture* bulletCrosshairTex; // Crosshair texture for laser bullet
	Texture* beamCrosshairTex;	 // Crosshair texture for laser beam

	Texture* beamCenterGlowTex;	// Center of the beam texture
	//Texture* beamHaloGlowTex;		// Halo around center of the beam texture

	AnimationMultiLerp<float> beamRadiusPulse;

	AnimationMultiLerp<Vector3D> arrowFadeLerp;

	void DrawBulletCrosshair(float alpha) const;
	void DrawBeamCrosshair(float alpha) const;
	void DrawRocketHUD(float alpha) const;
	void DrawBeamBlast(float alpha) const;
};

#endif // __CROSSHAIRLASERHUD_H__
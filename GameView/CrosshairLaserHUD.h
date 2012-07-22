/**
 * CrosshairLaserHUD.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
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
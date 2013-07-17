/**
 * BBBTitleDisplay.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "BBBTitleDisplay.h"
#include "GameViewConstants.h"
#include "GameFontAssetsManager.h"
#include "../BlammoEngine/Vector.h"
#include "../ResourceManager.h"

const char* BBBTitleDisplay::TITLE_BIFF_TEXT    = "Biff!";
const char* BBBTitleDisplay::TITLE_BAM_TEXT     = "Bam!!";
const char* BBBTitleDisplay::TITLE_BLAMMO_TEXT  = "Blammo!?!";

//const float BBBTitleDisplay::OUTLINE_SIZE_MULTIPLIER = 1.04f;

BBBTitleDisplay::BBBTitleDisplay(float scale) : blammoWidth(0), scale(scale) {

	// Setup any textures for rendering the title
	this->bangTextures.reserve(3);
	this->bangTextures.push_back(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG1, Texture2D::Trilinear, GL_TEXTURE_2D));
	this->bangTextures.push_back(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG2, Texture2D::Trilinear, GL_TEXTURE_2D));
	this->bangTextures.push_back(ResourceManager::GetInstance()->GetImgTextureResource(GameViewConstants::GetInstance()->TEXTURE_BANG3, Texture2D::Trilinear, GL_TEXTURE_2D));

    // Setup the full title display...
    static const float TEXT_DROP_SHADOW_SCALE_COEFF = 0.06f;

    // "Biff!" Bang Title Text
	this->biffEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->biffEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->biffEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->biffEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->biffEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->biffEmitter.SetParticleRotation(ESPInterval(-10));
	this->biffEmitter.SetParticleSize(ESPInterval(scale*10), ESPInterval(scale*5));
	this->biffEmitter.SetParticles(1, static_cast<Texture2D*>(this->bangTextures[0]));

	this->biffTextEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->biffTextEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->biffTextEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->biffTextEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->biffTextEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->biffTextEmitter.SetParticleRotation(ESPInterval(-10));
	this->biffTextEmitter.SetParticleSize(ESPInterval(scale*1), ESPInterval(scale*1));
	
    //this->biffTextOutlineEmitter.SetSpawnDelta(ESPInterval(-1, -1));
    //this->biffTextOutlineEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
    //this->biffTextOutlineEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
    //this->biffTextOutlineEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
    //this->biffTextOutlineEmitter.SetParticleAlignment(ESP::ScreenAligned);
    //this->biffTextOutlineEmitter.SetParticleRotation(ESPInterval(-10));
    //this->biffTextOutlineEmitter.SetParticleSize(ESPInterval(scale*OUTLINE_SIZE_MULTIPLIER), ESPInterval(scale*OUTLINE_SIZE_MULTIPLIER));
    //this->biffTextOutlineEmitter.SetParticleColour(ESPInterval(0), ESPInterval(0), ESPInterval(0), ESPInterval(1));

	TextLabel2D biffTitleText(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge),
        TITLE_BIFF_TEXT);
	biffTitleText.SetDropShadow(Colour(0,0,0), TEXT_DROP_SHADOW_SCALE_COEFF*scale);
    biffTitleText.SetScale(scale);
	this->biffTextEmitter.SetParticles(1, biffTitleText);
    //biffTitleText.SetDropShadowAmount(0.0f);
    //this->biffTextOutlineEmitter.SetParticles(1, biffTitleText);

	// "Bam!!" Bang title text
	this->bamEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->bamEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->bamEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->bamEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->bamEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->bamEmitter.SetParticleRotation(ESPInterval(-15));
	this->bamEmitter.SetParticleSize(ESPInterval(scale*11.0f), ESPInterval(scale*5.0f));
	this->bamEmitter.SetParticles(1, static_cast<Texture2D*>(this->bangTextures[1]));

	this->bamTextEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->bamTextEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->bamTextEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->bamTextEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->bamTextEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->bamTextEmitter.SetParticleRotation(ESPInterval(-15));
	this->bamTextEmitter.SetParticleSize(ESPInterval(scale*1), ESPInterval(scale*1));

	TextLabel2D bamTitleText(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge),
        TITLE_BAM_TEXT);
	bamTitleText.SetDropShadow(Colour(0,0,0), TEXT_DROP_SHADOW_SCALE_COEFF*scale);
    bamTitleText.SetScale(scale);
	this->bamTextEmitter.SetParticles(1, bamTitleText);

	// "Blammo!?!" Bang title text
	this->blammoEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->blammoEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->blammoEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->blammoEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->blammoEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->blammoEmitter.SetParticleRotation(ESPInterval(-8));
	this->blammoEmitter.SetParticleSize(ESPInterval(scale*12.0f), ESPInterval(scale*6.5f));
	this->blammoEmitter.SetParticles(1, static_cast<Texture2D*>(this->bangTextures[2]));

	this->blammoTextEmitter.SetSpawnDelta(ESPInterval(-1, -1));
	this->blammoTextEmitter.SetInitialSpd(ESPInterval(0.0f, 0.0f));
	this->blammoTextEmitter.SetParticleLife(ESPParticle::INFINITE_PARTICLE_LIFETIME);
	this->blammoTextEmitter.SetRadiusDeviationFromCenter(ESPInterval(0, 0));
	this->blammoTextEmitter.SetParticleAlignment(ESP::ScreenAligned);
	this->blammoTextEmitter.SetParticleRotation(ESPInterval(-8));
	this->blammoTextEmitter.SetParticleSize(ESPInterval(scale*1), ESPInterval(scale*1));

	TextLabel2D blammoTitleText(GameFontAssetsManager::GetInstance()->GetFont(
        GameFontAssetsManager::ExplosionBoom, GameFontAssetsManager::Huge), 
		TITLE_BLAMMO_TEXT);
	blammoTitleText.SetDropShadow(Colour(0,0,0), TEXT_DROP_SHADOW_SCALE_COEFF*scale);
    blammoTitleText.SetScale(scale);
	this->blammoWidth = blammoTitleText.GetLastRasterWidth();
	this->blammoTextEmitter.SetParticles(1, blammoTitleText);
}

BBBTitleDisplay::~BBBTitleDisplay() {
	// Release texture assets that we no longer need
	for (size_t i = 0; i < this->bangTextures.size(); i++) {
		bool releaseTexSuccess = ResourceManager::GetInstance()->ReleaseTextureResource(this->bangTextures[i]);
        UNUSED_VARIABLE(releaseTexSuccess);
		assert(releaseTexSuccess);
	}
}

void BBBTitleDisplay::Draw(float x, float y, const Camera& camera) {

	// Draw the "Biff!" Text
	Point3D BIFF_EMIT_COORD = Point3D(x, y - this->scale*3.0f, 0);

	this->biffEmitter.OverwriteEmittedPosition(BIFF_EMIT_COORD);
	this->biffEmitter.Tick(0.1);
	this->biffEmitter.Draw(camera);

    //this->biffTextOutlineEmitter.OverwriteEmittedPosition(BIFF_EMIT_COORD);
    //this->biffTextOutlineEmitter.Tick(0.1);
    //this->biffTextOutlineEmitter.Draw(camera);

	this->biffTextEmitter.OverwriteEmittedPosition(BIFF_EMIT_COORD);
	this->biffTextEmitter.Tick(0.1);
	this->biffTextEmitter.Draw(camera);

	// Draw the "Bam!!" Text
	Point3D BAM_EMIT_COORD = Point3D(x + this->scale*5.5f, y - this->scale*4.5f, 0);

	this->bamEmitter.OverwriteEmittedPosition(BAM_EMIT_COORD);
	this->bamEmitter.Tick(0.1);
	this->bamEmitter.Draw(camera);
	
	this->bamTextEmitter.OverwriteEmittedPosition(BAM_EMIT_COORD);
	this->bamTextEmitter.Tick(0.1);
	this->bamTextEmitter.Draw(camera);

	// Draw the "Blammo!?!" Text
	Point3D BLAMMO_EMIT_COORD = Point3D(x + this->scale*11.5f, y - this->scale*6.75f, 0);

	this->blammoEmitter.OverwriteEmittedPosition(BLAMMO_EMIT_COORD);
	this->blammoEmitter.Tick(0.1);
	this->blammoEmitter.Draw(camera);

	this->blammoTextEmitter.OverwriteEmittedPosition(BLAMMO_EMIT_COORD);
	this->blammoTextEmitter.Tick(0.1);
	this->blammoTextEmitter.Draw(camera);
}

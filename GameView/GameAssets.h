#ifndef __GAMEASSETS_H__
#define __GAMEASSETS_H__

#include "../GameModel/GameWorld.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/GameItemTimer.h"

class Texture3D;
class GameWorldAssets;
class LevelMesh;
class CgFxPostRefract;
class ESPEmitter;

// Includes all the models, textures, etc. for the game.
class GameAssets {
public:
	enum FontSize  { Small = 16, Medium = 24, Big = 32, Huge = 60 };
	enum FontStyle { GunBlam, ExplosionBoom, ElectricZap, AllPurpose }; //Decoish, Cyberpunkish };

private:
	// Set of fonts for use in the game, indexed by their style and height
	std::map<FontStyle, std::map<FontSize, TextureFontSet*>> fonts;

	// World-related meshes
	GameWorldAssets* worldAssets;

	// Level-related meshes
	LevelMesh* levelMesh;

	// Regular meshes - these persist throughout the entire game
	Mesh* ball;				// Ball used to break blocks
	Mesh* spikeyBall;	// What happens to the ball when it becomes uber
	Mesh* item;				// Item, picked up by the player paddle

	// Special effects - persistant special effects in the game
	CgFxPostRefract* invisiBallEffect;

	// Item related textures and drawlists
	std::map<std::string, Texture2D*> itemTextures;
	std::map<std::string, Texture2D*> itemTimerTextures;
	std::map<std::string, Texture2D*> itemTimerFillerTextures;

	// Currently active particle systems
	std::list<ESPEmitter*> activeParticleEmitters;

	void DeleteWorldAssets();
	void DeleteLevelAssets();
	void DeleteRegularMeshAssets();
	void DeleteRegularEffectAssets();

	// Asset loading helper functions
	void LoadRegularMeshAssets();
	void LoadRegularFontAssets();
	void LoadRegularEffectAssets();

	void LoadItemTextures();
	void UnloadItemTextures();

public:
	GameAssets();
	~GameAssets();

	void LoadWorldAssets(GameWorld::WorldStyle style);
	void LoadLevelAssets(const GameLevel* level);

	// LoadMinimalAssets(...)			// e.g., Fonts, Loadscreens
	// LoadBasicGameAssets(...)		// e.g., Ball-related, Item-related, Timer-related 

	// Draw functions ******************************************************************************
	void DrawPaddle(const PlayerPaddle& p, const Camera& camera) const;
	void DrawBackground(double dT, const Camera& camera);
	void DrawLevelPieces(const Camera& camera) const;
	void DrawGameBall(const GameBall& b, const Camera& camera, Texture2D* sceneTex) const;
	void DrawItem(const GameItem& gameItem, const Camera& camera) const;
	void DrawTimers(const std::list<GameItemTimer*>& timers, int displayWidth, int displayHeight);
	void DrawParticleEffects(double dT, const Camera& camera);

	// Public Getter Functions **********************************************************************
	LevelMesh* GetLevelMesh() const {
		return this->levelMesh;
	}

	// Obtain a particular font
	const TextureFontSet* GetFont(FontStyle style, FontSize height) const {
		std::map<FontStyle, std::map<FontSize, TextureFontSet*>>::const_iterator fontSetIter = this->fonts.find(style);
		if (fontSetIter != this->fonts.end()) {
			std::map<FontSize, TextureFontSet*>::const_iterator fontIter = fontSetIter->second.find(height);
			if (fontIter != fontSetIter->second.end()) {
				return fontIter->second;
			}
		}
		return NULL;
	}

private:

	// TODO: MOVE ALL THIS STUFF INTO GAMEVIEWCONSTANTS...

	static const std::string ITEM_MESH;
	static const std::string ITEM_LABEL_MATGRP;
	static const std::string ITEM_END_MATGRP;

	// Item timer colours
	static const Colour ITEM_GOOD_COLOUR;
	static const Colour ITEM_BAD_COLOUR;
	static const Colour ITEM_NEUTRAL_COLOUR;

	// Item-related texture assets
	// TODO: make these into arrays???
	static const std::string ITEM_SLOWBALL_TEXTURE;
	static const std::string ITEM_FASTBALL_TEXTURE;
	static const std::string ITEM_UBERBALL_TEXTURE;
	static const std::string ITEM_INVISIBALL_TEXTURE;

	static const std::string ITEM_TIMER_SLOWBALL_TEXTURE;
	static const std::string ITEM_TIMER_FASTBALL_TEXTURE;
	static const std::string ITEM_TIMER_UBERBALL_TEXTURE;
	static const std::string ITEM_TIMER_INVISIBALL_TEXTURE;

	static const std::string ITEM_TIMER_FILLER_SPDBALL_TEXTURE;
	static const std::string ITEM_TIMER_FILLER_UBERBALL_TEXTURE;
	static const std::string ITEM_TIMER_FILLER_INVISIBALL_TEXTURE;

};

#endif
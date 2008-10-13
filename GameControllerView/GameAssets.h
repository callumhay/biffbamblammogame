#ifndef __GAMEASSETS_H__
#define __GAMEASSETS_H__

#include "../GameModel/GameWorld.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/PlayerPaddle.h"
#include "../GameModel/GameItem.h"
#include "../GameModel/GameItemTimer.h"

#include "../Utils/Colour.h"

#include <map>
#include <string>

class TextureFontSet;
class Mesh;
class Camera;
class LevelMesh;
class Skybox;
class GameItem;
class Texture2D;

// Includes all the models, textures, etc. for the game.
class GameAssets {
public:
	enum FontSize  { Small = 16, Medium = 24, Big = 32, Huge = 60 };
	enum FontStyle { GunBlam, ExplosionBoom, BloodCrunch, ElectricZap, AllPurpose }; //Decoish, Cyberpunkish };

private:
	GameWorld::WorldStyle currLoadedStyle;

	// Set of fonts for use in the game, indexed by their style and height
	std::map<FontStyle, std::map<FontSize, TextureFontSet*>> fonts;

	// World-related meshes
	Skybox* skybox;
	Mesh* background;			// Meshes that make up the background scenery
	Mesh* playerPaddle;		// Currently loaded player paddle mesh

	// Level-related meshes
	LevelMesh* levelMesh;

	// Regular meshes - these persist throughout the entire game
	Mesh* ball;		// Ball used to break blocks
	Mesh* item;		// Item, picked up by the player paddle

	// Item related textures and drawlists
	std::map<std::string, Texture2D*> itemTextures;
	std::map<GameItemTimer::TimerType, Texture2D*> itemTimerTextures;
	std::map<GameItemTimer::TimerType, Texture2D*> itemTimerFillerTextures;

	void DeleteWorldAssets();
	void DeleteLevelAssets();
	void DeleteRegularMeshAssets();

	// Asset loading helper functions
	void LoadRegularMeshAssets();
	void LoadRegularFontAssets();
	void LoadDecoStyleAssets();
	void LoadCyberpunkStyleAssets();

	void LoadItemTextures();
	void UnloadItemTextures();

public:
	GameAssets();
	~GameAssets();

	void LoadWorldAssets(GameWorld::WorldStyle style);
	void LoadLevelAssets(GameWorld::WorldStyle worldStyle, const GameLevel* level);

	void DrawLevelPieces(const Camera& camera) const;
	void DrawGameBall(const GameBall& b, const Camera& camera) const;
	void DrawPaddle(const PlayerPaddle& p, const Camera& camera) const;
	void DrawBackground(double dT, const Camera& camera) const;
	void DrawItem(const GameItem& gameItem, const Camera& camera) const;
	void DrawTimers(const std::vector<GameItemTimer*>& timers, int displayWidth, int displayHeight);

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

	// Asset file path constants
	static const std::string RESOURCE_DIR;
	static const std::string FONT_DIR;
	static const std::string MESH_DIR;
	static const std::string SHADER_DIR;
	static const std::string TEXTURE_DIR;

private:
	// Shader assets
	static const std::string CELSHADER_FILEPATH;

	// Regular font assets
	static const std::string FONT_GUNBLAM;
	static const std::string FONT_EXPLOSIONBOOM;
	static const std::string FONT_BLOODCRUNCH;
	static const std::string FONT_SMACKBOUNCE;
	static const std::string FONT_ELECTRICZAP;
	static const std::string FONT_ALLPURPOSE;
	//static const std::string FONT_DECOISH;
	//static const std::string FONT_CYBERPUNKISH;

	// Regular mesh assets
	static const std::string BALL_MESH;
	static const std::string SKYBOX_MESH;
	
	static const std::string ITEM_MESH;
	static const std::string ITEM_LABEL_MATGRP;
	static const std::string ITEM_END_MATGRP;
	static const Colour ITEM_GOOD_COLOUR;
	static const Colour ITEM_BAD_COLOUR;
	static const Colour ITEM_NEUTRAL_COLOUR;

	static const std::string ITEM_SLOWBALL_TEXTURE;
	static const std::string ITEM_FASTBALL_TEXTURE;

	static const std::string ITEM_TIMER_SLOWBALL_TEXTURE;
	static const std::string ITEM_TIMER_FASTBALL_TEXTURE;

	static const std::string ITEM_TIMER_FILLER_SPDBALL_TEXTURE;

	// Deco assets
	static const std::string DECO_PADDLE_MESH;
	static const std::string DECO_SOLID_BLOCK_MESH;
	static const std::string DECO_BACKGROUND_MESH;

	// Cyberpunk assets
	static const std::string CYBERPUNK_PADDLE_MESH;
	static const std::string CYBERPUNK_SOLID_BLOCK_MESH;
	static const std::string CYBERPUNK_BACKGROUND_MESH;
	static const std::string CYBERPUNK_SKYBOX_TEXTURES[6];
};

#endif
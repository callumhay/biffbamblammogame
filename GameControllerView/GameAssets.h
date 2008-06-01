#ifndef __GAMEASSETS_H__
#define __GAMEASSETS_H__

#include "../GameModel/GameWorld.h"
#include "../GameModel/LevelPiece.h"
#include "../GameModel/GameBall.h"
#include "../GameModel/PlayerPaddle.h"

#include <map>
#include <string>

class TextureFontSet;
class Mesh;

// Includes all the models, textures, etc. for the game.
class GameAssets {
public:
	enum FontSize  { Small = 16, Medium = 32, Big = 64 };
	enum FontStyle { GunBlam, ExplosionBoom, BloodCrunch }; //Decoish, Cyberpunkish };

private:
	GameWorld::WorldStyle currLoadedStyle;

	// Set of fonts for use in the game, indexed by their style and height
	std::map<FontStyle, std::map<FontSize, TextureFontSet*>> fonts;

	Mesh* playerPaddle;		// Currently loaded player paddle mesh
	Mesh* block;					// Currently loaded breakable block mesh
	Mesh* ball;						// Currently loaded ball mesh

	void DeleteStyleAssets();

	// Asset loading helper functions
	void LoadRegularFontAssets();
	void LoadDecoStyleAssets();
	void LoadCyberpunkStyleAssets();

public:
	GameAssets();
	~GameAssets();

	void LoadAssets(GameWorld::WorldStyle style);

	void DrawLevelPieceMesh(const LevelPiece& p);
	void DrawGameBall(const GameBall& b);
	void DrawPaddle(const PlayerPaddle& p);

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
	// Asset file path constants
	static const std::string RESOURCE_DIR;
	static const std::string FONT_DIR;
	static const std::string MESH_DIR;

	// Regular font assets
	static const std::string FONT_GUNBLAM;
	static const std::string FONT_EXPLOSIONBOOM;
	static const std::string FONT_BLOODCRUNCH;
	//static const std::string FONT_DECOISH;
	//static const std::string FONT_CYBERPUNKISH;

	// Deco assets
	static const std::string DECO_PADDLE_MESH;
	static const std::string DECO_BALL_MESH;
	static const std::string DECO_BREAKABLE_BLOCK_MESH;

	// Cyberpunk assets
	static const std::string CYBERPUNK_PADDLE_MESH;
	static const std::string CYBERPUNK_BALL_MESH;
	static const std::string CYBERPUNK_BREAKABLE_BLOCK_MESH;

};

#endif
#ifndef __GAMEITEMASSETS_H__
#define __GAMEITEMASSETS_H__

#include "../BlammoEngine/BasicIncludes.h"

class Texture;
class Mesh;
class GameESPAssets;
class Camera;
class GameItem;
class GameItemTimer;
class PointLight;

/**
 * Manages assets related to items - including dropping items and items that are already
 * activated in the game.
 */
class GameItemAssets {

private:
	Mesh* item;	// Item, picked up by the player paddle
	GameESPAssets* espAssets;	// Effect assets

	// Mapping of item names to their respective texture assets
	std::map<std::string, Texture*> itemTextures;
	std::map<std::string, Texture*> itemTimerTextures;
	std::map<std::string, Texture*> itemTimerFillerTextures;

	// Private helper load and unload functions for item assets
	bool LoadItemTextures();
	bool LoadItemMeshes();
	void UnloadItemTextures();
	void UnloadItemMeshes();

	/**
	 * Represents a single timer in the game HUD. This class is used privately
	 * to organize the HUD element's layout and draw it.
	 */
	//class ItemTimerHUDElement {
	//	
	//private:
	//	static const int TIMER_VERTICAL_SPACING = 5;

	//public:
	//
	//
	//};

public:
	GameItemAssets(GameESPAssets* espAssets);
	~GameItemAssets();
	
	bool LoadItemAssets();
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem, 
		const PointLight& fgKeyLight, const PointLight& fgFillLight, const PointLight& ballLight) const;

	void DrawTimers(const std::list<GameItemTimer*>& timers, int displayWidth, int displayHeight);

};
#endif
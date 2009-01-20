#ifndef __GAMEITEMASSETS_H__
#define __GAMEITEMASSETS_H__

#include <map>
#include <string>
#include <list>

class Texture2D;
class Mesh;
class GameESPAssets;
class Camera;
class GameItem;
class GameItemTimer;

class GameItemAssets {

private:
	Mesh* item;	// Item, picked up by the player paddle
	GameESPAssets* espAssets;	// Effect assets

	// Mapping of item names to their respective texture assets
	std::map<std::string, Texture2D*> itemTextures;
	std::map<std::string, Texture2D*> itemTimerTextures;
	std::map<std::string, Texture2D*> itemTimerFillerTextures;

	// Private helper load and unload functions for item assets
	bool LoadItemTextures();
	bool LoadItemMeshes();
	void UnloadItemTextures();
	void UnloadItemMeshes();

public:
	GameItemAssets(GameESPAssets* espAssets);
	~GameItemAssets();
	
	bool LoadItemAssets();
	void DrawItem(double dT, const Camera& camera, const GameItem& gameItem) const;
	void DrawTimers(const std::list<GameItemTimer*>& timers, int displayWidth, int displayHeight);

};
#endif
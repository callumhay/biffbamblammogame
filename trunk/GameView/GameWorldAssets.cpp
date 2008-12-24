#include "GameWorldAssets.h"

// Subclasses that can be created...
#include "DecoWorldAssets.h"

// Static creation method
GameWorldAssets* GameWorldAssets::CreateWorldAssets(GameWorld::WorldStyle world) {
	switch (world) {
		case GameWorld::Deco:
			return new DecoWorldAssets();
		default:
			break;
	}

	return NULL;
}
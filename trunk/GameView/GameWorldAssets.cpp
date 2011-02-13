#include "GameWorldAssets.h"

// Subclasses that can be created...
#include "DecoWorldAssets.h"

// Static creation method
GameWorldAssets* GameWorldAssets::CreateWorldAssets(GameWorld::WorldStyle world) {
	switch (world) {
		case GameWorld::Deco:
			return new DecoWorldAssets();
        case GameWorld::Futurism:
            assert(false);
            // TODO: return new FuturismWorldAssets();
            break;
		default:
			break;
	}

	return NULL;
}
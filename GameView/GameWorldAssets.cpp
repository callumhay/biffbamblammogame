/**
 * GameWorldAssets.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

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
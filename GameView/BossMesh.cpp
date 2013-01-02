/**
 * BossMesh.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2012
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "BossMesh.h"
#include "ClassicalBossMesh.h"

#include "../GameModel/ClassicalBoss.h"

BossMesh* BossMesh::Build(const GameWorld::WorldStyle& style, Boss* boss) {
    BossMesh* result = NULL;
    switch (style) {

        case GameWorld::Classical: {
            assert(dynamic_cast<ClassicalBoss*>(boss) != NULL);
            ClassicalBoss* classicalBoss = static_cast<ClassicalBoss*>(boss);
            result = new ClassicalBossMesh(classicalBoss);
            break;
        }

        case GameWorld::Deco:
            // TODO

        case GameWorld::Futurism:
            // TODO

        default:
            assert(false);
            break;

    }

    return result;
}
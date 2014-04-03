/**
 * PersistentTextureManager.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __PERSISTENTTEXTUREMANAGER_H__
#define __PERSISTENTTEXTUREMANAGER_H__

#include "../BlammoEngine/BasicIncludes.h"

class Texture2D;

class PersistentTextureManager {
public:
    static PersistentTextureManager* GetInstance() {
        if (PersistentTextureManager::instance == NULL) {
            PersistentTextureManager::instance = new PersistentTextureManager();
        }
        return PersistentTextureManager::instance;
    }
    static void DeleteInstance() {
        if (PersistentTextureManager::instance != NULL) {
            delete PersistentTextureManager::instance;
            PersistentTextureManager::instance = NULL;
        }
    }

    Texture2D* PreloadTexture2D(const std::string& texturePath);
    Texture2D* GetLoadedTexture(const std::string& texturePath) const;

private:
    static PersistentTextureManager* instance;

    typedef std::map<std::string, Texture2D*> TextureMap;
    typedef TextureMap::const_iterator TextureMapConstIter;
    typedef TextureMap::iterator TextureMapIter;
    
    TextureMap loadedTextures;

    PersistentTextureManager();
    ~PersistentTextureManager();

    void Clear();

    DISALLOW_COPY_AND_ASSIGN(PersistentTextureManager);
};

inline Texture2D* PersistentTextureManager::GetLoadedTexture(const std::string& texturePath) const {
    TextureMapConstIter findIter = this->loadedTextures.find(texturePath);
    assert(findIter != this->loadedTextures.end());
    return findIter->second;
}

#endif // __PERSISTENTTEXTUREMANAGER_H__
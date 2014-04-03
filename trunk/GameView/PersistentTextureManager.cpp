/**
 * PersistentTextureManager.cpp
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

#include "PersistentTextureManager.h"
#include "../BlammoEngine/Texture2D.h"
#include "../ResourceManager.h"

PersistentTextureManager* PersistentTextureManager::instance = NULL;

PersistentTextureManager::PersistentTextureManager() {
}

PersistentTextureManager::~PersistentTextureManager() {
    this->Clear();
}

Texture2D* PersistentTextureManager::PreloadTexture2D(const std::string& texturePath) {
    // Check whether we've already loaded the texture
    TextureMapIter findIter = this->loadedTextures.find(texturePath);
    if (findIter != this->loadedTextures.end()) {
        return findIter->second;
    }
    
    // Texture hasn't been loaded yet, so load it into memory
    Texture* texture = ResourceManager::GetInstance()->GetImgTextureResource(texturePath, Texture::Trilinear, GL_TEXTURE_2D);
    if (texture == NULL) {
        assert(false);
        return NULL;
    }
    assert(dynamic_cast<Texture2D*>(texture) != NULL);

    Texture2D* tex2D = static_cast<Texture2D*>(texture);
    this->loadedTextures.insert(std::make_pair(texturePath, tex2D));
    return tex2D;
}

void PersistentTextureManager::Clear() {
    for (TextureMapIter iter = this->loadedTextures.begin(); iter != this->loadedTextures.end(); ++iter) {
        ResourceManager::GetInstance()->ReleaseTextureResource(iter->second);
    }
    this->loadedTextures.clear();
}

/****************************************************************************
**
** Copyright (C) 2016
**
** This file is part of the Magus toolkit
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#include "texturelayer_manager.h"

//****************************************************************************/
TextureLayerManager::TextureLayerManager(void)
{
}

//****************************************************************************/
TextureLayerManager::~TextureLayerManager(void)
{
    removeAndDeleteAllTextureLayers();
}

//****************************************************************************/
TextureLayer* TextureLayerManager::createOrRetrieveTextureLayer (const Ogre::IdString& datablockName,
                                                                 Ogre::PbsTextureTypes textureType,
                                                                 const Ogre::String& textureFileName)
{
    // Search the TextureLayer; ignore the textureFileName as search criterium
    TextureLayers::iterator it;
    TextureLayers::iterator itStart = mTextureLayers.begin();
    TextureLayers::iterator itEnd = mTextureLayers.end();
    TextureLayer* tl;
    for (it = itStart; it != itEnd; ++it)
    {
        tl = *it;
        if (tl->mDatablockName == datablockName && tl->mTextureType == textureType)
            return tl;
    }

    // TextureLayer not found, create a new one
    tl = new TextureLayer();
    tl->setDatablockNameAndTexture(datablockName, textureType, textureFileName);
    mTextureLayers.push_back(tl);
    return tl;
}

//****************************************************************************/
void TextureLayerManager::removeAndDeleteTextureLayer (TextureLayer* textureLayer)
{
    TextureLayers::iterator it;
    TextureLayers::iterator itStart = mTextureLayers.begin();
    TextureLayers::iterator itEnd = mTextureLayers.end();
    TextureLayer* tl;
    for (it = itStart; it != itEnd; ++it)
    {
        tl = *it;
        if (tl == textureLayer)
        {
            mTextureLayers.erase(it);
            delete tl;
            return;
        }
    }
}

//****************************************************************************/
void TextureLayerManager::removeAndDeleteAllTextureLayers (void)
{
    TextureLayers::iterator it;
    TextureLayers::iterator itStart = mTextureLayers.begin();
    TextureLayers::iterator itEnd = mTextureLayers.end();
    for (it = itStart; it != itEnd; ++it)
        delete (*it);

    mTextureLayers.clear();
}

//****************************************************************************/
TextureLayerManager::TextureLayers* TextureLayerManager::getTextureLayers (void)
{
    return &mTextureLayers;
}

//****************************************************************************/
TextureLayer* TextureLayerManager::getTextureLayer (Ogre::PbsTextureTypes textureType)
{
    TextureLayers::iterator it;
    TextureLayers::iterator itStart = mTextureLayers.begin();
    TextureLayers::iterator itEnd = mTextureLayers.end();
    TextureLayer* tl;
    for (it = itStart; it != itEnd; ++it)
    {
        tl = *it;
        if (tl->mTextureType == textureType)
            return tl;
    }
}

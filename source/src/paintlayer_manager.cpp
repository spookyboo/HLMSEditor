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

#include "paintlayer_manager.h"

//****************************************************************************/
PaintLayerManager::PaintLayerManager(void)
{
}

//****************************************************************************/
PaintLayerManager::~PaintLayerManager(void)
{
    removeAndDeleteAllPaintLayers();
}

//****************************************************************************/
PaintLayer* PaintLayerManager::createPaintLayer (const Ogre::IdString& datablockName,
                                                 Ogre::PbsTextureTypes textureType,
                                                 const Ogre::String& textureFileName,
                                                 int externalLayerId)
{
    PaintLayer* paintLayer = new PaintLayer(this, externalLayerId);
    TextureLayer* textureLayer = mTextureLayerManager.createOrRetrieveTextureLayer(datablockName, textureType, textureFileName);
    paintLayer->setTextureLayer(textureLayer);
    mPaintLayers.push_back(paintLayer);
    return paintLayer;
}

//****************************************************************************/
PaintLayer* PaintLayerManager::createPaintLayer (int externalLayerId)
{
    PaintLayer* paintLayer = new PaintLayer(this, externalLayerId);
    mPaintLayers.push_back(paintLayer);
    return paintLayer;
}

//****************************************************************************/
PaintLayer* PaintLayerManager::setTextureLayerInPaintLayer (const Ogre::IdString& datablockName,
                                                            Ogre::PbsTextureTypes textureType,
                                                            const Ogre::String& textureFileName,
                                                            int externalLayerId)
{
    PaintLayer* paintLayer = getPaintLayer(externalLayerId);
    if (paintLayer)
    {
        TextureLayer* textureLayer = mTextureLayerManager.createOrRetrieveTextureLayer(datablockName, textureType, textureFileName);
        paintLayer->setTextureLayer(textureLayer);
    }

    return paintLayer;
}

//****************************************************************************/
void PaintLayerManager::removeAndDeletePaintLayer (PaintLayer* paintLayer)
{
    PaintLayers::iterator it;
    PaintLayers::iterator itStart = mPaintLayers.begin();
    PaintLayers::iterator itEnd = mPaintLayers.end();
    PaintLayer* pl;
    for (it = itStart; it != itEnd; ++it)
    {
        pl = *it;
        if (pl == paintLayer)
        {
            mPaintLayers.erase(it);
            delete pl;
            return;
        }
    }
}

//****************************************************************************/
void PaintLayerManager::removeAndDeletePaintLayer (int externalLayerId)
{
    PaintLayers::iterator it;
    PaintLayers::iterator itStart = mPaintLayers.begin();
    PaintLayers::iterator itEnd = mPaintLayers.end();
    PaintLayer* pl;
    for (it = itStart; it != itEnd; ++it)
    {
        pl = *it;
        if (pl->getExternaLayerlId() == externalLayerId)
        {
            mPaintLayers.erase(it);
            delete pl;
            return;
        }
    }
}

//****************************************************************************/
void PaintLayerManager::removeAndDeleteAllPaintLayers (void)
{
    PaintLayers::iterator it;
    PaintLayers::iterator itStart = mPaintLayers.begin();
    PaintLayers::iterator itEnd = mPaintLayers.end();
    for (it = itStart; it != itEnd; ++it)
        delete (*it);

    mPaintLayers.clear();

    mTextureLayerManager.removeAndDeleteAllTextureLayers();
}

//****************************************************************************/
PaintLayers* PaintLayerManager::getPaintLayers (void)
{
    return &mPaintLayers;
}

//****************************************************************************/
PaintLayer* PaintLayerManager::getPaintLayer (int externalLayerId)
{
    PaintLayers::iterator it;
    PaintLayers::iterator itStart = mPaintLayers.begin();
    PaintLayers::iterator itEnd = mPaintLayers.end();
    PaintLayer* pl;
    for (it = itStart; it != itEnd; ++it)
    {
        pl = *it;
        if (pl->getExternaLayerlId() == externalLayerId)
            return pl;
    }

    return 0;
}

//****************************************************************************/
void  PaintLayerManager::enableAllPaintLayers (bool enabled)
{
    PaintLayers::iterator it;
    PaintLayers::iterator itStart = mPaintLayers.begin();
    PaintLayers::iterator itEnd = mPaintLayers.end();
    PaintLayer* pl;
    for (it = itStart; it != itEnd; ++it)
    {
        pl = *it;
        pl->enable(enabled);
    }
}

//****************************************************************************/
void PaintLayerManager::enablePaintLayers (std::vector<int> externalPaintLayerIds, bool enabled)
{
    PaintLayers::iterator it;
    PaintLayers::iterator itStart = mPaintLayers.begin();
    PaintLayers::iterator itEnd = mPaintLayers.end();
    PaintLayer* pl;
    for (it = itStart; it != itEnd; ++it)
    {
        pl = *it;
        std::vector<int>::iterator itExtId;
        std::vector<int>::iterator itExtIdStart = externalPaintLayerIds.begin();
        std::vector<int>::iterator itExtIdEnd = externalPaintLayerIds.end();
        for (itExtId = itExtIdStart; itExtId != itExtIdEnd; ++itExtId)
        {
            if (pl->getExternaLayerlId() == *itExtId)
                pl->enable(enabled);
        }
    }
}

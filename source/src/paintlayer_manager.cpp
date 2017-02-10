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
                                                  const Ogre::String& textureFileName)
{
    PaintLayer* paintLayer = new PaintLayer();
    TextureLayer* textureLayer = mTextureLayerManager.createOrRetrieveTextureLayer(datablockName, textureType, textureFileName);
    paintLayer->setTextureLayer(textureLayer);
    mPaintLayers.push_back(paintLayer);
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

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
#include "OgreLogManager.h"
#include "OgreMesh2.h"
#include "OgreSubMesh2.h"
#include "OgreBitwise.h"

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

//****************************************************************************/
void PaintLayerManager::loadTextureGeneration (Ogre::PbsTextureTypes textureType, Ogre::ushort sequence)
{
    TextureLayer* textureLayer = mTextureLayerManager.getTextureLayer(textureType);
    if (textureLayer && textureLayer->mTextureTypeDefined)
        textureLayer->loadTextureGeneration(sequence);
}

//****************************************************************************/
const MeshIndexUvMapType& PaintLayerManager::getMinMaxUVFromMesh (const Ogre::MeshPtr mesh)
{
    mMeshIndexUvMapType.clear();
    float minU, maxU;
    float minV, maxV;
    float val;
    Ogre::Vector4 minUmaxUminVmaxV;
    size_t index = 0;
    Ogre::Mesh::SubMeshVec::const_iterator subMeshIterator = mesh->getSubMeshes().begin();
    while (subMeshIterator != mesh->getSubMeshes().end())
    {
        Ogre::SubMesh* subMesh = *subMeshIterator;
        Ogre::VertexArrayObjectArray vaos = subMesh->mVao[0];

        if (!vaos.empty())
        {
            // Get the first LOD level
            Ogre::VertexArrayObject *vao = vaos[0];

            Ogre::VertexArrayObject::ReadRequestsArray readRequests;
            Ogre::VertexElement2VecVec vertexDeclaration = vao->getVertexDeclaration();
            Ogre::VertexElement2VecVec::const_iterator it0 = vertexDeclaration.begin();
            Ogre::VertexElement2VecVec::const_iterator en0 = vertexDeclaration.end();

            while( it0 != en0 )
            {
                Ogre::VertexElement2Vec::const_iterator it1 = it0->begin();
                Ogre::VertexElement2Vec::const_iterator en1 = it0->end();

                while( it1 != en1 )
                {
                    if( *it1 == Ogre::VES_TEXTURE_COORDINATES )
                    {
                        readRequests.push_back( Ogre::VES_TEXTURE_COORDINATES );
                        break; // Just get the first texture coordinate set
                    }
                    ++it1;
                }
                ++it0;
            }

            vao->readRequests(readRequests);
            vao->mapAsyncTickets(readRequests);
            minU = 99999999.0f;
            maxU = -99999999.0f;
            minV = 99999999.0f;
            maxV = -99999999.0f;
            const size_t subMeshVerticiesNum = readRequests[0].vertexBuffer->getNumElements(); // Number of vertices per submesh
            // Number of elements per vertex declaration per submesh is not important; the result only contains 1 texcoords set

            for (size_t i = 0; i < subMeshVerticiesNum; ++i)
            {
                if (readRequests[0].type == Ogre::VET_HALF2)
                {
                    Ogre::uint16 const * RESTRICT_ALIAS bufferF16 =
                            reinterpret_cast<Ogre::uint16 const * RESTRICT_ALIAS>( readRequests[0].data );
                    val = Ogre::Bitwise::halfToFloat( bufferF16[0] );
                    minU = std::min(minU, val);
                    maxU = std::max(maxU, val);
                    val = Ogre::Bitwise::halfToFloat( bufferF16[1] );
                    minV = std::min(minV, val);
                    maxV = std::max(maxV, val);

                }
                else
                {
                    float const * RESTRICT_ALIAS bufferF32 =
                            reinterpret_cast<float const * RESTRICT_ALIAS>( readRequests[0].data );
                    val = bufferF32[0];
                    minU = std::min(minU, val);
                    maxU = std::max(maxU, val);
                    val = bufferF32[1];
                    minV = std::min(minV, val);
                    maxV = std::max(maxV, val);
                }

                readRequests[0].data += readRequests[0].vertexBuffer->getBytesPerElement();
            }

            minUmaxUminVmaxV = Ogre::Vector4::ZERO;
            minUmaxUminVmaxV.x = minU;
            minUmaxUminVmaxV.y = maxU;
            minUmaxUminVmaxV.z = minV;
            minUmaxUminVmaxV.w = maxV;
            mMeshIndexUvMapType[index] = minUmaxUminVmaxV;
            Ogre::LogManager::getSingleton().logMessage("UV index(" +
                                                        Ogre::StringConverter::toString(index) +
                                                        "): " +
                                                        Ogre::StringConverter::toString(minUmaxUminVmaxV)); // DEBUG


            vao->unmapAsyncTickets(readRequests);
        }

        subMeshIterator++;
        index++;
    }

    return mMeshIndexUvMapType;
}

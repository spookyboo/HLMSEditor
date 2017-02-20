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

#include "constants.h"
#include "texturelayer.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsManager.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"

//****************************************************************************/
TextureLayer::TextureLayer(void) :
    mDatablock(0),
    mDatablockPbs(0),
    mTextureOnWhichIsPaintedWidth(0),
    mTextureOnWhichIsPaintedHeight(0),
    mTextureOnWhichIsPaintedHasAlpha(false),
    mNumMipMaps(0),
    mTextureTypeDefined(false),
    mCurrentSequence(0),
    mMaxSequence(0)
{
    mTextureType = Ogre::PBSM_DIFFUSE;
    mDatablockName = "";
    mTexture.setNull();
    mBuffers.clear();
    mTextureFileName = "";
}

//****************************************************************************/
TextureLayer::~TextureLayer(void)
{
    if (!mTexture.isNull())
        mTexture.setNull();
}

//****************************************************************************/
void TextureLayer::setDatablockNameAndTexture (const Ogre::IdString& datablockName,
                                               Ogre::PbsTextureTypes textureType,
                                               const Ogre::String& textureFileName)
{
    mDatablockName = datablockName;
    mTextureType = textureType;
    mTextureFileName = textureFileName;
    Ogre::HlmsManager* hlmsManager = Ogre::Root::getSingletonPtr()->getHlmsManager();
    Ogre::HlmsPbs* hlmsPbs = static_cast<Ogre::HlmsPbs*>(hlmsManager->getHlms(Ogre::HLMS_PBS));
    mDatablock = hlmsPbs->getDatablock(datablockName);
    if (mDatablock)
    {
        mDatablockPbs = static_cast<Ogre::HlmsPbsDatablock*>(mDatablock);
        try
        {
            // Texture on GPU; store the buffers in a vector
            if (!mDatablockPbs->getTexture(textureType).isNull())
            {
                mTextureTypeDefined = true;
                mTexture = mDatablockPbs->getTexture(textureType); // TextureType MUST exist, otherwise the application crashes
                mNumMipMaps = mTexture->getNumMipmaps();
                mBuffers.clear();
                for (Ogre::uint8 i = 0; i < mNumMipMaps; ++i)
                    mBuffers.push_back(mTexture->getBuffer(0, i).getPointer());

                // Load the texture as image
                mTextureOnWhichIsPainted.load(textureFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
                mPixelboxTextureOnWhichIsPainted = mTextureOnWhichIsPainted.getPixelBox(0, 0);
                mTextureOnWhichIsPaintedHasAlpha = mTextureOnWhichIsPainted.getHasAlpha();
                mTextureOnWhichIsPaintedWidth = mPixelboxTextureOnWhichIsPainted.getWidth();
                mTextureOnWhichIsPaintedHeight = mPixelboxTextureOnWhichIsPainted.getHeight();
            }
        }
        catch (Ogre::Exception e)
        {
            mTexture.setNull();
            mBuffers.clear();
        }
    }
}

//****************************************************************************/
void TextureLayer::blitTexture (void)
{
    size_t w = mTextureOnWhichIsPaintedWidth;
    size_t h = mTextureOnWhichIsPaintedHeight;
    Ogre::Image textureOnWhichIsPaintedScaled = mTextureOnWhichIsPainted; // Define textureOnWhichIsPaintedScaled each time; reusing results in exception
    for (Ogre::uint8 i = 0; i < mNumMipMaps; ++i)
    {
        mBuffers.at(i)->blitFromMemory(textureOnWhichIsPaintedScaled.getPixelBox(0,0), Ogre::Box(0, 0, 0, w, h, 1));
        w*=0.5f; // Mipmaps always are half of the previous one
        h*=0.5f;
        if (w > 1.0f && h > 1.0f)
            textureOnWhichIsPaintedScaled.resize(w, h);
        else
            break; // Stop when the mipmaps are too small
    }
    textureOnWhichIsPaintedScaled.freeMemory();
}

//****************************************************************************/
void TextureLayer::setNextTextureGeneration (void)
{
     // TODO: Check whether file exists
    mCurrentSequence = mCurrentSequence > mMaxSequence ? mMaxSequence : ++mCurrentSequence;
    Ogre::String textureFileNameGeneration = getTextureFileNameGeneration (mCurrentSequence);
    mTextureOnWhichIsPainted.load(textureFileNameGeneration, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    mPixelboxTextureOnWhichIsPainted = mTextureOnWhichIsPainted.getPixelBox(0, 0);
    mTextureOnWhichIsPaintedHasAlpha = mTextureOnWhichIsPainted.getHasAlpha();
    mTextureOnWhichIsPaintedWidth = mPixelboxTextureOnWhichIsPainted.getWidth();
    mTextureOnWhichIsPaintedHeight = mPixelboxTextureOnWhichIsPainted.getHeight();
    blitTexture();
}

//****************************************************************************/
void TextureLayer::setPreviousTextureGeneration (void)
{
    // TODO: Check whether file exists
    mCurrentSequence = mCurrentSequence < 1 ? 0 : --mCurrentSequence;
    Ogre::String textureFileNameGeneration = getTextureFileNameGeneration (mCurrentSequence);
    mTextureOnWhichIsPainted.load(textureFileNameGeneration, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME); // TODO: File is in resource group????
    mPixelboxTextureOnWhichIsPainted = mTextureOnWhichIsPainted.getPixelBox(0, 0);
    mTextureOnWhichIsPaintedHasAlpha = mTextureOnWhichIsPainted.getHasAlpha();
    mTextureOnWhichIsPaintedWidth = mPixelboxTextureOnWhichIsPainted.getWidth();
    mTextureOnWhichIsPaintedHeight = mPixelboxTextureOnWhichIsPainted.getHeight();
    blitTexture();
}

//****************************************************************************/
void TextureLayer::setFirstTextureGeneration (void)
{
    // TODO: Check whether file exists
    mCurrentSequence = 0;
    mTextureOnWhichIsPainted.load(mTextureFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    mPixelboxTextureOnWhichIsPainted = mTextureOnWhichIsPainted.getPixelBox(0, 0);
    mTextureOnWhichIsPaintedHasAlpha = mTextureOnWhichIsPainted.getHasAlpha();
    mTextureOnWhichIsPaintedWidth = mPixelboxTextureOnWhichIsPainted.getWidth();
    mTextureOnWhichIsPaintedHeight = mPixelboxTextureOnWhichIsPainted.getHeight();
    blitTexture();
}

//****************************************************************************/
void TextureLayer::setLastTextureGeneration (void)
{
    // TODO: Check whether file exists
    mCurrentSequence = mMaxSequence;
    Ogre::String textureFileNameGeneration = getTextureFileNameGeneration (mCurrentSequence);
    mTextureOnWhichIsPainted.load(textureFileNameGeneration, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME); // TODO: File is in resource group????
    mPixelboxTextureOnWhichIsPainted = mTextureOnWhichIsPainted.getPixelBox(0, 0);
    mTextureOnWhichIsPaintedHasAlpha = mTextureOnWhichIsPainted.getHasAlpha();
    mTextureOnWhichIsPaintedWidth = mPixelboxTextureOnWhichIsPainted.getWidth();
    mTextureOnWhichIsPaintedHeight = mPixelboxTextureOnWhichIsPainted.getHeight();
    blitTexture();
}

//****************************************************************************/
void TextureLayer::saveTextureGeneration (void)
{
    // Increase the sequence
    ++mMaxSequence;
    Ogre::String textureFileNameGeneration = getTextureFileNameGeneration (mMaxSequence);
    mTextureOnWhichIsPainted.save(textureFileNameGeneration);

    // TODO: Is this file part of the resourcegroup or should the resourcegroup be initialized again??
}

//****************************************************************************/
const Ogre::String& TextureLayer::getTextureFileNameGeneration (int sequence, bool fullQualified)
{
    // TODO: Restrict to limited number of generations?

    mHelperString = mTextureFileName;

    // Do not go beyond the max sequence number
    sequence = sequence > mMaxSequence ? mMaxSequence : sequence;
    if (sequence > 0)
    {
        // Do not go below sequence number 1 (otherwise the original mTextureFileName is returned)
        Ogre::String strippedTextureFileName = mTextureFileName;
        Ogre::String extension = mTextureFileName;
        strippedTextureFileName.erase(strippedTextureFileName.find_last_of("."), Ogre::String::npos);
        extension.erase(0, extension.find_last_of("."));
        mHelperString = strippedTextureFileName +
                Ogre::StringConverter::toString(sequence) +
                extension;

        if (fullQualified)
            mHelperString = TEMP_PATH + mHelperString;
    }

    return mHelperString;
}

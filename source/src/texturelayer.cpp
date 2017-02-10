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

#include "texturelayer.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsManager.h"
//#include "OgreLogManager.h"
//#include "OgreStringConverter.h"

//****************************************************************************/
TextureLayer::TextureLayer(void) :
    mDatablock(0),
    mDatablockPbs(0),
    mTextureOnWhichIsPaintedWidth(0),
    mTextureOnWhichIsPaintedHeight(0),
    mTextureOnWhichIsPaintedHasAlpha(false),
    mNumMipMaps(0)
{
    mTextureType = Ogre::PbsTextureTypes::PBSM_DIFFUSE;
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
        catch (Ogre::Exception e)
        {
            mTexture.setNull();
            mBuffers.clear();
        }
    }
}

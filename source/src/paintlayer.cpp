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

#include "paintlayer.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsManager.h"
#include "OgreLogManager.h"
#include "OgreFrameStats.h"
#include "OgreStringConverter.h"

//****************************************************************************/
PaintLayer::PaintLayer(void) :
    mDatablock(0),
    mDatablockPbs(0),
    mBrushForce(1.0f),
    mTextureOnWhichIsPaintedWidth(0),
    mTextureOnWhichIsPaintedHeight(0),
    mBrushWidth(0),
    mBrushHeight(0),
    mNumMipMaps(0),
    mPaintEffect(PAINT_EFFECT_COLOR),
    mPaintOverflow(PAINT_OVERFLOW_IGNORE),
    mFractionTextureBrushWidth(1.0f),
    mFractionTextureBrushHeight(1.0f)
{
    mTexture.setNull();
    mBuffers.clear();
    mPaintColour = Ogre::ColourValue(1.0f, 0.0f, 1.0f, 1.0f);
    mFinalColour = Ogre::ColourValue::White;
    mTextureFileName = "";
    mBrushFileName = "";
}

//****************************************************************************/
PaintLayer::~PaintLayer(void)
{
    mTexture.setNull();
}

//****************************************************************************/
void PaintLayer::paint(float u, float v)
{
    // Apply paint effect if there is a texture
    if (mTexture.isNull())
        return;

    /* Loop through the pixelbox of the brush and apply the paint effect to the pixelbox of the texture
     * Note, that only mipmap 0 of the texture is painted, so prevent textures with mipmaps.
     * If PAINT_EFFECT_COLOR, per pixel:
     * - get alpha from brush
     * - res1 = multiply alpha with paintcolour (mPaintColour)
     * - res2 = multiply (1 - alpha) with colour of the texture
     * - New texture colour is res1 + res2
     */
    float alpha;
    for (size_t y = 0; y < mBrushHeight; y++)
    {
        for (size_t x = 0; x < mBrushWidth; x++)
        {
            if (mPaintEffect == PAINT_EFFECT_COLOR)
            {
                // Paint with colour
                alpha = mPixelboxBrush.getColourAt(x, y, 0).a;
                mFinalColour = alpha * mPaintColour;
                mFinalColour = (1.0f - alpha) * mPixelboxTextureOnWhichIsPainted.getColourAt(x, y, 0) + mFinalColour;
                mPixelboxTextureOnWhichIsPainted.setColourAt(mFinalColour,
                                                             calculateTexturePositionX(u, x),
                                                             calculateTexturePositionX(v, y),
                                                             0);
            }
        }
    }

    /* Copy the changed texture to the GPU; beware to also update the mipmaps.
     * The texture on the GPU (mTexture) may contain more mipmaps (or less) than the
     * texture loaded as an image (mTextureOnWhichIsPainted), although they refer to the
     * same texture file. Mipsmaps are updated by means of scaling the texture image.
     */
    size_t w = mTextureOnWhichIsPaintedWidth;
    size_t h = mTextureOnWhichIsPaintedHeight;
    mTextureOnWhichIsPaintedScaled = mTextureOnWhichIsPainted;
    for (Ogre::uint8 i = 0; i < mNumMipMaps; ++i)
    {
        mBuffers.at(i)->blitFromMemory(mTextureOnWhichIsPaintedScaled.getPixelBox(0,0), Ogre::Box(0, 0, 0, w, h, 1));
        w*=0.5f; // Mipmaps always are half of the previous one
        h*=0.5f;
        mTextureOnWhichIsPaintedScaled.resize(w, h);
    }
}

//****************************************************************************/
void PaintLayer::setDatablockNameAndTexture (const Ogre::IdString& datablockName,
                                             Ogre::PbsTextureTypes textureType,
                                             const Ogre::String& textureFileName)
{
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
            for (Ogre::uint8 i = 0; i < mNumMipMaps; ++i)
                mBuffers.push_back(mTexture->getBuffer(0, i).getPointer());

            // Load the texture as image
            mTextureOnWhichIsPainted.load(textureFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            mPixelboxTextureOnWhichIsPainted = mTextureOnWhichIsPainted.getPixelBox(0, 0);
            mTextureOnWhichIsPaintedHasAlpha = mTextureOnWhichIsPainted.getHasAlpha();
            mTextureOnWhichIsPaintedWidth = mPixelboxTextureOnWhichIsPainted.getWidth();
            mTextureOnWhichIsPaintedHeight = mPixelboxTextureOnWhichIsPainted.getHeight();
            if (mBrushWidth > 0)
                mFractionTextureBrushWidth = mTextureOnWhichIsPaintedWidth / mBrushWidth;
            else
                mFractionTextureBrushWidth = 1.0f;
            if (mBrushHeight > 0)
                mFractionTextureBrushHeight = mTextureOnWhichIsPaintedHeight / mBrushHeight;
            else
                mFractionTextureBrushHeight = 1.0f;
            mTextureFileName = textureFileName;
        }
        catch (Ogre::Exception e)
        {
            mTexture.setNull();
            mBuffers.clear();
        }
    }
}

//****************************************************************************/
void PaintLayer::setBrush (const Ogre::String& brushFileName)
{
    try
    {
        mBrushFileName = brushFileName;
        mBrush.load(brushFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        mPixelboxBrush = mBrush.getPixelBox(0, 0);
        mBrushWidth = mBrush.getWidth();
        mBrushHeight = mBrush.getHeight();
        if (mBrushWidth > 0)
            mFractionTextureBrushWidth = mTextureOnWhichIsPaintedWidth / mBrushWidth;
        else
            mFractionTextureBrushWidth = 1.0f;
        if (mBrushHeight > 0)
            mFractionTextureBrushHeight = mTextureOnWhichIsPaintedHeight / mBrushHeight;
        else
            mFractionTextureBrushHeight = 1.0f;
    }
    catch (Ogre::Exception e) {}
}

//****************************************************************************/
size_t PaintLayer::calculateTexturePositionX (float u, size_t brushPositionX)
{
    int pos = mFractionTextureBrushWidth * brushPositionX - (u - 0.5f) * mTextureOnWhichIsPaintedWidth;
    if (mPaintOverflow == PAINT_OVERFLOW_IGNORE)
    {
        if (pos < 0)
            pos = 0;
        else if (pos > mTextureOnWhichIsPaintedWidth)
            pos = mTextureOnWhichIsPaintedWidth;
    }

    return pos;
}
//****************************************************************************/
size_t PaintLayer::calculateTexturePositionY (float v, size_t brushPositionY)
{
    int pos = mFractionTextureBrushHeight * brushPositionY - (v - 0.5f) * mTextureOnWhichIsPaintedHeight;
    if (mPaintOverflow == PAINT_OVERFLOW_IGNORE)
    {
        if (pos < 0)
            pos = 0;
        else if (pos > mTextureOnWhichIsPaintedHeight)
            pos = mTextureOnWhichIsPaintedHeight;
    }

    return pos;
}


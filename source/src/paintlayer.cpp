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
#include "OgreStringConverter.h"

//****************************************************************************/
PaintLayer::PaintLayer(void) :
    mEnabled(true),
    mBrushForce(1.0f),
    mBrushScale(1.0f),
    mBrushWidth(0),
    mBrushHeight(0),
    mHalfBrushWidth(0),
    mHalfBrushHeight(0),
    mHalfBrushWidthScaled(0),
    mHalfBrushHeightScaled(0),
    mPaintEffect(PAINT_EFFECT_COLOR),
    mPaintOverflow(PAINT_OVERFLOW_IGNORE),
    mTextureLayer(0),
    calculatedTexturePositionX(0),
    calculatedTexturePositionY(0),
    mAlpha(1.0f)
{
    mPaintColour = Ogre::ColourValue::White;
    mFinalColour = Ogre::ColourValue::White;
    mBrushFileName = "";
}

//****************************************************************************/
PaintLayer::~PaintLayer(void)
{
}

//****************************************************************************/
void PaintLayer::enable(bool enabled)
{
    mEnabled = enabled;
}

//****************************************************************************/
void PaintLayer::paint(float u, float v)
{
    // Apply paint effect if there is a texture
    if (!mEnabled || !mTextureLayer || mTextureLayer->mTexture.isNull())
        return;

    /* Loop through the pixelbox of the brush and apply the paint effect to the pixelbox of the texture
     * Note, that only mipmap 0 of the texture image is painted, so prevent textures with mipmaps.
     * The texture on the GPU may contain mipmaps; these ARE painted.
     */
    for (size_t y = 0; y < mBrushHeight; y++)
    {
        for (size_t x = 0; x < mBrushWidth; x++)
        {
            if (mPaintEffect == PAINT_EFFECT_COLOR)
            {
                // Paint with colour
                // If PAINT_EFFECT_COLOR, per pixel:
                // - get alpha from brush
                // - res1 = multiply alpha with paintcolour (mPaintColour)
                // - res2 = multiply (1 - alpha) with colour of the texture
                // - New texture colour is res1 + res2
                calculatedTexturePositionX = calculateTexturePositionX(u, x);
                calculatedTexturePositionY = calculateTexturePositionY(v, y);
                mAlpha = mPixelboxBrush.getColourAt(x, y, 0).a;
                mFinalColour = mAlpha * mPaintColour;
                mFinalColour = (1.0f - mAlpha) * mTextureLayer->mPixelboxTextureOnWhichIsPainted.getColourAt(calculatedTexturePositionX,
                                                                                                             calculatedTexturePositionY,
                                                                                                             0) + mFinalColour;
                mTextureLayer->mPixelboxTextureOnWhichIsPainted.setColourAt(mFinalColour,
                                                                            calculatedTexturePositionX,
                                                                            calculatedTexturePositionY,
                                                                            0);
            }
            else if (mPaintEffect == PAINT_EFFECT_ALPHA)
            {
                // Paint with alpha
                // TODO: DOES NOT WORK WITH ALPHA????
                calculatedTexturePositionX = calculateTexturePositionX(u, x);
                calculatedTexturePositionY = calculateTexturePositionY(v, y);
                mAlpha = mPixelboxBrush.getColourAt(x, y, 0).a;
                mFinalColour = mTextureLayer->mPixelboxTextureOnWhichIsPainted.getColourAt(calculatedTexturePositionX,
                                                                                           calculatedTexturePositionY,
                                                                                           0);

                mFinalColour.a = 1.0f - mAlpha;
                mTextureLayer->mPixelboxTextureOnWhichIsPainted.setColourAt(mFinalColour,
                                                                            calculatedTexturePositionX,
                                                                            calculatedTexturePositionY,
                                                                            0);
            }
        }
    }

    /* Copy the changed texture to the GPU; beware to also update the mipmaps.
     * The texture on the GPU (mTexture) may contain more mipmaps (or less) than the
     * texture loaded as an image (mTextureOnWhichIsPainted), although they refer to the
     * same texture file. Mipsmaps are updated by means of scaling the texture image.
     */
    size_t w = mTextureLayer->mTextureOnWhichIsPaintedWidth;
    size_t h = mTextureLayer->mTextureOnWhichIsPaintedHeight;
    mTextureOnWhichIsPaintedScaled = mTextureLayer->mTextureOnWhichIsPainted;
    for (Ogre::uint8 i = 0; i < mTextureLayer->mNumMipMaps; ++i)
    {
        mTextureLayer->mBuffers.at(i)->blitFromMemory(mTextureOnWhichIsPaintedScaled.getPixelBox(0,0), Ogre::Box(0, 0, 0, w, h, 1));
        w*=0.5f; // Mipmaps always are half of the previous one
        h*=0.5f;
        mTextureOnWhichIsPaintedScaled.resize(w, h);
    }
}

//****************************************************************************/
void PaintLayer::setTextureLayer (TextureLayer* textureLayer)
{
    mTextureLayer = textureLayer;
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
        mHalfBrushWidth = 0.5f * mBrushWidth;
        mHalfBrushWidthScaled = mBrushScale * mHalfBrushWidth;
        mBrushHeight = mBrush.getHeight();
        mHalfBrushHeight = 0.5f * mBrushHeight;
        mHalfBrushHeightScaled = mBrushScale * mHalfBrushHeight;
    }
    catch (Ogre::Exception e) {}
}

//****************************************************************************/
void PaintLayer::setPaintEffect (PaintEffects paintEffect)
{
    mPaintEffect = paintEffect;
}

//****************************************************************************/
void PaintLayer::setPaintColour (const Ogre::ColourValue& colourValue)
{
    mPaintColour = colourValue;
}

//****************************************************************************/
void PaintLayer::setBrushScale (float brushScale)
{
    mBrushScale = brushScale;
    mHalfBrushWidthScaled = mBrushScale * mHalfBrushWidth;
    mHalfBrushHeightScaled = mBrushScale * mHalfBrushHeight;
}

//****************************************************************************/
size_t PaintLayer::calculateTexturePositionX (float u, size_t brushPositionX)
{
    if (!mTextureLayer)
        return 0;

    int pos = (u * mTextureLayer->mTextureOnWhichIsPaintedWidth) - mHalfBrushWidthScaled + mBrushScale * brushPositionX;
    if (mPaintOverflow == PAINT_OVERFLOW_IGNORE)
    {
        if (pos < 0)
            pos = 0;
        else if (pos > mTextureLayer->mTextureOnWhichIsPaintedWidth)
            pos = mTextureLayer->mTextureOnWhichIsPaintedWidth;
    }

    return pos;
}

//****************************************************************************/
size_t PaintLayer::calculateTexturePositionY (float v, size_t brushPositionY)
{
    if (!mTextureLayer)
        return 0;

    int pos = (v * mTextureLayer->mTextureOnWhichIsPaintedHeight) - mHalfBrushHeightScaled + mBrushScale * brushPositionY;
    if (mPaintOverflow == PAINT_OVERFLOW_IGNORE)
    {
        if (pos < 0.0f)
            pos = 0.0f;
        else if (pos > mTextureLayer->mTextureOnWhichIsPaintedHeight)
            pos = mTextureLayer->mTextureOnWhichIsPaintedHeight;
    }

    return pos;
}

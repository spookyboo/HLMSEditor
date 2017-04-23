/****************************************************************************
**
** Copyright (C) 2016 - 2017
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
#include "paintlayer_manager.h"
#include "OgreHlmsPbs.h"
#include "OgreHlmsManager.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "constants.h"
#include "math.h"

//****************************************************************************/
PaintLayer::PaintLayer(PaintLayerManager* paintLayerManager, int externalLayerId) :
    mPaintLayerManager(paintLayerManager),
    mExternaLayerlId(externalLayerId),
    mEnabled(true),
    mVisible(true),
    mBrushWidth(0),
    mBrushHeight(0),
    mHalfBrushWidth(0),
    mHalfBrushWidthScaled(0),
    mHalfBrushHeight(0),
    mHalfBrushHeightScaled(0),
    mBrushWidthMinusOne(0),
    mBrushHeightMinusOne(0),
    mForce(1.0f),
    mScale(0.1f),
    mPaintEffect(PAINT_EFFECT_COLOR),
    mPaintOverflow(PAINT_OVERFLOW_CONTINUE),
    mTextureLayer(0),
    mCalculatedTexturePositionX(0),
    mCalculatedTexturePositionY(0),
    mAlpha(1.0f),
    mAlphaDecay(0.0f),
    mRotate(false),
    mRotationAngle(0.0f),
    mSinRotationAngle(0.0f),
    mCosRotationAngle(0.0f),
    mPosX(0),
    mPosY(0),
    mPosXrotated(0),
    mPosYrotated(0),
    mTranslate(false),
    mTranslationFactorX(0.0f),
    mTranslationFactorY(0.0f),
    mTranslationX(0),
    mTranslationY(0),
    mMirrorHorizontal(false),
    mMirrorVertical(false),
    mJitterRotate(false),
    mJitterTranslate(false),
    mJitterScale(false),
    mJitterForce(false),
    mJitterPaintColour(false),
    mJitterMirrorHorizontal(false),
    mJitterMirrorVertical(false),
    mJitterRotationAngleInterval(0.0f),
    mJitterTranslationInterval(0.0f),
    mJitterScaleInterval(0.0f),
    mJitterForceInterval(0.0f),
    mJitterPaintColourInterval(0.0f),
    mJitterMirrorHorizontalInterval(0.0f),
    mJitterMirrorVerticalInterval(0.0f),
    mJitterElapsedTime(0.0f),
    mJitterRotateElapsedTime(0.0f),
    mJitterTranslationElapsedTime(0.0f),
    mJitterScaleElapsedTime(0.0f),
    mJitterForceElapsedTime(0.0f),
    mJitterPaintColourElapsedTime(0.0f),
    mJitterMirrorHorizontalElapsedTime(0.0f),
    mJitterMirrorVerticalElapsedTime(0.0f),
    mJitterRotationAngleMin(0.0f),
    mJitterRotationAngleMax(360.0f),
    mJitterTranslationFactorXmin(-1),
    mJitterTranslationFactorXmax(1),
    mJitterTranslationFactorYmin(-1),
    mJitterTranslationFactorYmax(1),
    mJitterScaleMin(0.0f),
    mJitterScaleMax(1.0f),
    mJitterForceMin(0.1f),
    mJitterForceMax(1.0f),
    mCarbonCopydata(0),
    mCarbonCopyTextureScale(1.0f),
    mSmudgeDecay(0.05f)
{
    mPaintColour = Ogre::ColourValue::White;
    mFinalColour = Ogre::ColourValue::White;
    mJitterPaintColourMin = Ogre::ColourValue::Black;
    mJitterPaintColourMax = Ogre::ColourValue::White;
    mBrushFileName = "";
    dummyDatablockId = "";
    mHelperOgreString = "";
    mCarbonCopyTextureFileName = "";
    mStartTime = clock();
}

//****************************************************************************/
PaintLayer::~PaintLayer(void)
{
    if (mCarbonCopydata)
        delete [] mCarbonCopydata;
}

//****************************************************************************/
void PaintLayer::enable(bool enabled)
{
    mEnabled = enabled;
}

//****************************************************************************/
void PaintLayer::setVisible (bool visible)
{
    mVisible = visible;
}

//****************************************************************************/
bool PaintLayer::isVisible(void)
{
    return mVisible;
}

//****************************************************************************/
void PaintLayer::paint(float u, float v, bool start)
{
    // Apply paint effect if there is a texture
    if (!mEnabled || !mVisible || !mTextureLayer)
        return;

    /* If there are jitter effects, they are applied first. This means that certain jitter attributes are set
     * to s specific value, so the brush image is applied to the texture according to these jitter attributes.
     */
    determineJitterEffects();


    /* In case of a smudge effect, the alpha value gradually decreases
     */
    if (mPaintEffect == PAINT_EFFECT_SMUDGE)
    {
        if (start)
        {
            mAlphaDecay = 1.0f;
        }
        else
        {
            mAlphaDecay -= mSmudgeDecay;
            mAlphaDecay = mAlphaDecay > 0.0f ? mAlphaDecay : 0.0f;
        }
    }

    /* Loop through the pixelbox of the brush and apply the paint effect to the pixelbox of the texture
     * Note, that only mipmap 0 of the texture image is painted, so prevent textures with mipmaps.
     * The texture on the GPU may contain mipmaps; these ARE painted.
     */
    for (size_t y = 0; y < mBrushHeight; y++)
    {
        for (size_t x = 0; x < mBrushWidth; x++)
        {
            // If mirroring, rotation and/or translation are enabled, calculate the new pixel position of the brush
            mPosX = x;
            mPosY = y;
            if (mMirrorHorizontal)
            {
                mPosY = mBrushHeightMinusOne - mPosY;
            }
            if (mMirrorVertical)
            {
                mPosX = mBrushWidthMinusOne - mPosX;
            }
            if (mRotate)
            {
                // Calculate the rotated pixel position
                mPosX -= mHalfBrushWidth;
                mPosY -= mHalfBrushHeight;
                mPosXrotated = mPosX * mCosRotationAngle - mPosY * mSinRotationAngle;
                mPosYrotated = mPosX * mSinRotationAngle + mPosY * mCosRotationAngle;
                mPosX = mPosXrotated + mHalfBrushWidth;
                mPosY = mPosYrotated + mHalfBrushHeight;

                // Determine whether the pixel position still fits within the brush dimensions
                if (mPosX < 0 || mPosX >= mBrushWidth || mPosY < 0 || mPosY >= mBrushHeight)
                    continue;
            }
            if (mTranslate)
            {
                // Calculate the translated pixel position
                mPosX += mTranslationX;
                mPosY += mTranslationY;

                // Determine whether the pixel position still fits within the brush dimensions
                if (mPosX < 0 || mPosX >= mBrushWidth || mPosY < 0 || mPosY >= mBrushHeight)
                    continue;
            }

            // Calculate the position of the brush pixel to a texture position
            mCalculatedTexturePositionX = calculateTexturePositionX(u, x);
            mCalculatedTexturePositionY = calculateTexturePositionY(v, y);

            if (mPaintEffect == PAINT_EFFECT_COLOR)
            {
                // Paint with colour
                mAlpha = mForce * mPixelboxBrush.getColourAt(mPosX, mPosY, 0).a;
                mFinalColour = mAlpha * mPaintColour;
                mFinalColour = (1.0f - mAlpha) * mTextureLayer->mPixelboxTextureOnWhichIsPainted.getColourAt(mCalculatedTexturePositionX,
                                                                                                             mCalculatedTexturePositionY,
                                                                                                             0) + mFinalColour;
            }
            else if (mPaintEffect == PAINT_EFFECT_ERASE)
            {
                // Erase: Retrieve the colourvalue of the original texture and blend it with the colour value of the texture on which is painted
                mAlpha = mForce * mPixelboxBrush.getColourAt(mPosX, mPosY, 0).a;
                mFinalColour = mAlpha * mTextureLayer->mPixelboxOriginalTexture.getColourAt(mCalculatedTexturePositionX,
                                                                                            mCalculatedTexturePositionY,
                                                                                            0);

                mFinalColour = (1.0f - mAlpha) * mTextureLayer->mPixelboxTextureOnWhichIsPainted.getColourAt(mCalculatedTexturePositionX,
                                                                                                            mCalculatedTexturePositionY,
                                                                                                            0) + mFinalColour;
            }
            else if (mPaintEffect == PAINT_EFFECT_ALPHA)
            {
                // Paint with alpha value of the brush (note, that the texture must have alpha!)
                mAlpha = mForce * mPixelboxBrush.getColourAt(mPosX, mPosY, 0).a;
                mFinalColour = mTextureLayer->mPixelboxTextureOnWhichIsPainted.getColourAt(mCalculatedTexturePositionX,
                                                                                           mCalculatedTexturePositionY,
                                                                                           0);
                mFinalColour.a -= mAlpha;
            }
            else if (mPaintEffect == PAINT_EFFECT_TEXTURE)
            {
                // Paint with textured brush
                mFinalColour = mPixelboxBrush.getColourAt(mPosX, mPosY, 0);
                mAlpha = mForce * mFinalColour.a;
                mFinalColour *= mAlpha;
                mFinalColour = (1.0f - mAlpha) * mTextureLayer->mPixelboxTextureOnWhichIsPainted.getColourAt(mCalculatedTexturePositionX,
                                                                                                            mCalculatedTexturePositionY,
                                                                                                            0) + mFinalColour;
            }
            else if (mPaintEffect == PAINT_EFFECT_CARBON_COPY)
            {
                // Paint with another texture; this texture has the same dimensions as the target texture
                // The shape of the brush is used to copy the Carbon Copy texture on the target texture (similar to carbon copy)
                mFinalColour = mPixelboxCarbonCopyTexture.getColourAt(mCalculatedTexturePositionX,
                                                                      mCalculatedTexturePositionY,
                                                                      0);
                mAlpha = mForce * mPixelboxBrush.getColourAt(mPosX, mPosY, 0).a;
                mAlpha *= mFinalColour.a;
                mFinalColour *= mAlpha;
                mFinalColour = (1.0f - mAlpha) * mTextureLayer->mPixelboxTextureOnWhichIsPainted.getColourAt(mCalculatedTexturePositionX,
                                                                                                            mCalculatedTexturePositionY,
                                                                                                            0) + mFinalColour;
            }
            else if (mPaintEffect == PAINT_EFFECT_SMUDGE)
            {
                // First get the colour from the target map
                Ogre::ColourValue targetTextureColour = mTextureLayer->mPixelboxTextureOnWhichIsPainted.getColourAt(mCalculatedTexturePositionX,
                                                                                                                    mCalculatedTexturePositionY,
                                                                                                                    0);
                // Determine alpha from brush
                mAlpha = mAlphaDecay * mForce * mPixelboxBrush.getColourAt(mPosX, mPosY, 0).a;
                //mAlpha = mAlpha > 1.0f ? 1.0f : mAlpha;
                //mAlpha = mAlpha < 0.0f ? 0.0f : mAlpha;

                // Fill the brush texture with the initiation target map value if painting starts
                if (start)
                {
                    // The pixelbox smudge colour is equal to the target map colour
                    // Gradually fill the mPixelboxBrushSmudge with the targetTextureColour
                    mFinalColour = targetTextureColour;
                    mPixelboxBrushSmudge.setColourAt(targetTextureColour, mPosX, mPosY, 0);
                }
                else
                {
                    // Determine colour of pixelbox smudge
                    mFinalColour = mPixelboxBrushSmudge.getColourAt(mPosX, mPosY, 0);
                }

                // Create the final colour for the target map
                mFinalColour = (1.0f - mAlpha) * targetTextureColour + mAlpha * mFinalColour;
            }

            // Set the final paint colour
            mTextureLayer->mPixelboxTextureOnWhichIsPainted.setColourAt(mFinalColour,
                                                                        mCalculatedTexturePositionX,
                                                                        mCalculatedTexturePositionY,
                                                                        0);
        }
    }

    /* Copy the changed texture to the GPU; beware to also update the mipmaps.
     * The texture on the GPU (mTexture) may contain more mipmaps (or less) than the
     * texture loaded as an image (mTextureOnWhichIsPainted), although they refer to the
     * same texture file. Mipsmaps are updated by means of scaling the texture image.
     */
    mTextureLayer->blitTexture();
}

//****************************************************************************/
void PaintLayer::setTextureLayer (TextureLayer* textureLayer)
{
    mTextureLayer = textureLayer;
}

//****************************************************************************/
TextureLayer* PaintLayer::getTextureLayer (void)
{
    return mTextureLayer;
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
        mHalfBrushWidthScaled = mScale * mHalfBrushWidth;
        mBrushWidthMinusOne = mBrushWidth - 1;
        mBrushHeight = mBrush.getHeight();
        mHalfBrushHeight = 0.5f * mBrushHeight;
        mHalfBrushHeightScaled = mScale * mHalfBrushHeight;
        mBrushHeightMinusOne = mBrushHeight - 1;
        mTranslationX = mTranslationFactorX * mBrushWidth;
        mTranslationY = mTranslationFactorY * mBrushHeight;
        mBrushSmudge = mBrush; // To get an image with the same dimensions as the brush
        mPixelboxBrushSmudge = mBrushSmudge.getPixelBox(0, 0);
        for (size_t y = 0; y < mBrushHeight; y++)
            for (size_t x = 0; x < mBrushWidth; x++)
                mPixelboxBrushSmudge.setColourAt(Ogre::ColourValue::Black, x, y, 0);
    }
    catch (Ogre::Exception e) {}
}

//****************************************************************************/
void PaintLayer::setPaintEffect (PaintEffects paintEffect)
{
    mPaintEffect = paintEffect;
}

//****************************************************************************/
void PaintLayer::setPaintOverflow (PaintOverflowTypes paintOverflow)
{
    mPaintOverflow = paintOverflow;
}

//****************************************************************************/
void PaintLayer::setCarbonCopyTextureFileName (const Ogre::String& textureFileName)
{
    mCarbonCopyTextureFileName = textureFileName;
    createCarbonCopyTexture();
}

//****************************************************************************/
const Ogre::String& PaintLayer::getCarbonCopyTextureFileName (void)
{
    return mCarbonCopyTextureFileName;
}

//****************************************************************************/
void PaintLayer::setCarbonCopyScale (float scale)
{
    mCarbonCopyTextureScale = scale;
    createCarbonCopyTexture();
}

//****************************************************************************/
float PaintLayer::getCarbonCopyScale (void)
{
    return mCarbonCopyTextureScale;
}

//****************************************************************************/
void PaintLayer::setPaintColour (const Ogre::ColourValue& colourValue)
{
    mPaintColour = colourValue;
}

//****************************************************************************/
void PaintLayer::setJitterPaintColour (const Ogre::ColourValue& paintColourMin, const Ogre::ColourValue& paintColourMax)
{
    mJitterPaintColour = true;
    mJitterPaintColourMin = paintColourMin;
    mJitterPaintColourMax = paintColourMax;
}

//****************************************************************************/
void PaintLayer::setJitterPaintColourMin (const Ogre::ColourValue& paintColourMin)
{
    mJitterPaintColour = true;
    mJitterPaintColourMin = paintColourMin;
}

//****************************************************************************/
void PaintLayer::setJitterPaintColourMax (const Ogre::ColourValue& paintColourMax)
{
    mJitterPaintColour = true;
    mJitterPaintColourMax = paintColourMax;
}

//****************************************************************************/
void PaintLayer::setJitterPaintColourInterval (float interval)
{
    mJitterPaintColourInterval = interval;
}

//****************************************************************************/
void PaintLayer::resetPaintColour (void)
{
    mJitterPaintColour = false;
}

//****************************************************************************/
bool PaintLayer::hasJitterPaintEnabled (void)
{
    return mJitterPaintColour;
}

//****************************************************************************/
void PaintLayer::setJitterPaintEnabled (bool enabled)
{
    mJitterPaintColour = enabled;
}

//****************************************************************************/
void PaintLayer::setForce (float force)
{
    mForce = force;
}

//****************************************************************************/
void PaintLayer::setJitterForce (float jitterForceMin, float jitterForceMax)
{
    mJitterForce = true;
    mJitterForceMin = jitterForceMin;
    mJitterForceMax = jitterForceMax;
}

//****************************************************************************/
void PaintLayer::setJitterForceMin (float jitterForceMin)
{
    mJitterForce = true;
    mJitterForceMin = jitterForceMin;
}

//****************************************************************************/
void PaintLayer::setJitterForceMax (float jitterForceMax)
{
    mJitterForce = true;
    mJitterForceMax = jitterForceMax;
}

//****************************************************************************/
void PaintLayer::setJitterForceInterval (float interval)
{
    mJitterForceInterval = interval;
}

//****************************************************************************/
void PaintLayer::resetForce (void)
{
    mJitterForce = false;
}

//****************************************************************************/
bool PaintLayer::hasJitterForceEnabled (void)
{
    return mJitterForce;
}

//****************************************************************************/
void PaintLayer::setJitterForceEnabled (bool enabled)
{
    mJitterForce = enabled;
}

//****************************************************************************/
void PaintLayer::setRotationAngle (float rotationAngle)
{
    mRotationAngle = rotationAngle;
    mRotate = true;
    mSinRotationAngle = sin(rotationAngle * Ogre::Math::PI / 180);
    mCosRotationAngle = cos(rotationAngle * Ogre::Math::PI / 180);
}

//****************************************************************************/
void PaintLayer::setJitterRotationAngle (float rotationAngleMin, float rotationAngleMax)
{
    mRotate = true;
    mJitterRotate = true;
    mJitterRotationAngleMin = rotationAngleMin;
    mJitterRotationAngleMax = rotationAngleMax;
}

//****************************************************************************/
void PaintLayer::setJitterRotationAngleMin (float rotationAngleMin)
{
    mRotate = true;
    mJitterRotate = true;
    mJitterRotationAngleMin = rotationAngleMin;
}

//****************************************************************************/
void PaintLayer::setJitterRotationAngleMax (float rotationAngleMax)
{
    mRotate = true;
    mJitterRotate = true;
    mJitterRotationAngleMax = rotationAngleMax;
}

//****************************************************************************/
void PaintLayer::setJitterRotationAngleInterval (float interval)
{
    mJitterRotationAngleInterval = interval;
}

//****************************************************************************/
void PaintLayer::resetRotation (void)
{
    mRotate = false;
    mJitterRotate = false;
}

//****************************************************************************/
bool PaintLayer::hasJitterRotationEnabled (void)
{
    return mJitterRotate;
}

//****************************************************************************/
void PaintLayer::setJitterRotationEnabled (bool enabled)
{
    mJitterRotate = enabled;
}

//****************************************************************************/
void PaintLayer::setTranslationFactor (float translationFactorX, float translationFactorY)
{
    mTranslate = true;
    mTranslationFactorX = translationFactorX;
    mTranslationFactorY = translationFactorY;
    mTranslationX = translationFactorX * mBrushWidth;
    mTranslationY = translationFactorY * mBrushHeight;
}

//****************************************************************************/
void PaintLayer::setJitterTranslationFactor (float jitterTranslationFactorXmin,
                                             float jitterTranslationFactorXmax,
                                             float jitterTranslationFactorYmin,
                                             float jitterTranslationFactorYmax)
{
    mTranslate = true;
    mJitterTranslate = true;
    mJitterTranslationFactorXmin = jitterTranslationFactorXmin;
    mJitterTranslationFactorXmax = jitterTranslationFactorXmax;
    mJitterTranslationFactorYmin = jitterTranslationFactorYmin;
    mJitterTranslationFactorYmax = jitterTranslationFactorYmax;
}

//****************************************************************************/
void PaintLayer::setJitterTranslationFactorX (float jitterTranslationFactorXmin,
                                              float jitterTranslationFactorXmax)
{
    mTranslate = true;
    mJitterTranslate = true;
    mJitterTranslationFactorXmin = jitterTranslationFactorXmin;
    mJitterTranslationFactorXmax = jitterTranslationFactorXmax;
}

//****************************************************************************/
void PaintLayer::setJitterTranslationFactorY (float jitterTranslationFactorYmin,
                                              float jitterTranslationFactorYmax)
{
    mTranslate = true;
    mJitterTranslate = true;
    mJitterTranslationFactorYmin = jitterTranslationFactorYmin;
    mJitterTranslationFactorYmax = jitterTranslationFactorYmax;
}

//****************************************************************************/
const Ogre::Vector2& PaintLayer::getJitterTranslationFactorX (void)
{
    mHelperVector2.x = mJitterTranslationFactorXmin;
    mHelperVector2.y = mJitterTranslationFactorXmax;
    return mHelperVector2;
}

//****************************************************************************/
const Ogre::Vector2& PaintLayer::getJitterTranslationFactorY (void)
{
    mHelperVector2.x = mJitterTranslationFactorYmin;
    mHelperVector2.y = mJitterTranslationFactorYmax;
    return mHelperVector2;
}

//****************************************************************************/
void PaintLayer::setJitterTranslationInterval (float interval)
{
    mJitterTranslationInterval = interval;
}

//****************************************************************************/
void PaintLayer::resetTranslation (void)
{
    mTranslate = false;
    mJitterTranslate = false;
}

//****************************************************************************/
bool PaintLayer::hasJitterTranslationEnabled(void)
{
    return mJitterTranslate;
}

//****************************************************************************/
void PaintLayer::setJitterTranslationEnabled (bool enabled)
{
    mJitterTranslate = enabled;
}

//****************************************************************************/
void PaintLayer::setScale (float scale)
{
    mScale = scale;
    mHalfBrushWidthScaled = mScale * mHalfBrushWidth;
    mHalfBrushHeightScaled = mScale * mHalfBrushHeight;
}

//****************************************************************************/
void PaintLayer::setJitterScale (float jitterScaleMin, float jitterScaleMax)
{
    mJitterScale = true;
    mJitterScaleMax = jitterScaleMax;
    mJitterScaleMin = jitterScaleMin;
}

//****************************************************************************/
void PaintLayer::setJitterScaleMin (float jitterScaleMin)
{
    mJitterScale = true;
    mJitterScaleMin = jitterScaleMin;
}

//****************************************************************************/
void PaintLayer::setJitterScaleMax (float jitterScaleMax)
{
    mJitterScale = true;
    mJitterScaleMax = jitterScaleMax;
}

//****************************************************************************/
void PaintLayer::setJitterScaleInterval (float interval)
{
    mJitterScaleInterval = interval;
}

//****************************************************************************/
void PaintLayer::resetScale (void)
{
    mJitterScale = false;
}

//****************************************************************************/
bool PaintLayer::hasJitterScaleEnabled(void)
{
    return mJitterScale;
}

//****************************************************************************/
void PaintLayer::setJitterScaleEnabled(bool enabled)
{
    mJitterScale = enabled;
}

//****************************************************************************/
void PaintLayer::setMirrorHorizontal (bool mirrored)
{
    mMirrorHorizontal = mirrored;
}

//****************************************************************************/
void PaintLayer::setJitterMirrorHorizontal (bool enabled)
{
    mJitterMirrorHorizontal = enabled;
}

//****************************************************************************/
void PaintLayer::setJitterMirrorHorizontalInterval (float interval)
{
    mJitterMirrorHorizontalInterval = interval;
}

//****************************************************************************/
void PaintLayer::resetMirrorHorizontal (void)
{
    mMirrorHorizontal = false;
    mJitterMirrorHorizontal = false;
}

//****************************************************************************/
bool PaintLayer::hasJitterMirrorHorizontalEnabled(void)
{
    return mJitterMirrorHorizontal;
}

//****************************************************************************/
void PaintLayer::setJitterMirrorHorizontalEnabled (bool enabled)
{
    mJitterMirrorHorizontal = enabled;
}

//****************************************************************************/
void PaintLayer::setMirrorVertical (bool mirrored)
{
    mMirrorVertical = mirrored;
}

//****************************************************************************/
void PaintLayer::setJitterMirrorVertical (bool enabled)
{
    mJitterMirrorVertical = enabled;
}

//****************************************************************************/
void PaintLayer::setJitterMirrorVerticalInterval (float interval)
{
    mJitterMirrorVerticalInterval = interval;
}

//****************************************************************************/
void PaintLayer::resetMirrorVertical (void)
{
    mMirrorVertical = false;
    mJitterMirrorVertical = false;
}

//****************************************************************************/
bool PaintLayer::hasJitterMirrorVerticalEnabled(void)
{
    return mJitterMirrorVertical;
}

//****************************************************************************/
void PaintLayer::setJitterMirrorVerticalEnabled (bool enabled)
{
    mJitterMirrorVertical = enabled;
}

//****************************************************************************/
size_t PaintLayer::calculateTexturePositionX (float u, size_t brushPositionX)
{
    if (!mTextureLayer)
        return 0;

    int w = mTextureLayer->mTextureOnWhichIsPaintedWidth;
    int pos = (u * w) - mHalfBrushWidthScaled + mScale * brushPositionX;

    if (mPaintOverflow == PAINT_OVERFLOW_IGNORE)
    {
        if (pos < 0)
            pos = 0;
        else if (pos >= w)
            pos = w - 1;
        return pos;
    }
    else if (mPaintOverflow == PAINT_OVERFLOW_CONTINUE)
    {
        if (pos < 0)
            pos += w;
        else if (pos >= w)
            pos -= w;
        return pos;
    }

    return pos;
}

//****************************************************************************/
size_t PaintLayer::calculateTexturePositionY (float v, size_t brushPositionY)
{
    if (!mTextureLayer)
        return 0;

    int h = mTextureLayer->mTextureOnWhichIsPaintedHeight;
    int pos = (v * h) - mHalfBrushHeightScaled + mScale * brushPositionY;

    if (mPaintOverflow == PAINT_OVERFLOW_IGNORE)
    {
        if (pos < 0)
            pos = 0;
        else if (pos >= h)
            pos = h - 1;
        return pos;
    }
    else if (mPaintOverflow == PAINT_OVERFLOW_CONTINUE)
    {
        if (pos < 0)
            pos += h;
        else if (pos >= h)
            pos -= h;
        return pos;
    }

    return pos;
}

//****************************************************************************/
void PaintLayer::determineJitterEffects (void)
{
    // Get the elapsed time
    mEndTime = clock();
    mJitterElapsedTime = (float)(mEndTime - mStartTime) / CLOCKS_PER_SEC;
    mStartTime = mEndTime;

    // Determine whether there are jitter effects
    if (mJitterRotate)
    {
        mJitterRotateElapsedTime += mJitterElapsedTime;
        if (mJitterRotateElapsedTime > mJitterRotationAngleInterval)
        {
            setRotationAngle (randomBetweenTwoFloats(mJitterRotationAngleMin, mJitterRotationAngleMax));
            mJitterRotateElapsedTime = 0.0f;
        }
    }

    if (mJitterTranslate)
    {
        mJitterTranslationElapsedTime += mJitterElapsedTime;
        if (mJitterTranslationElapsedTime > mJitterTranslationInterval)
        {
            setTranslationFactor(randomBetweenTwoFloats(mJitterTranslationFactorXmin, mJitterTranslationFactorXmax),
                                 randomBetweenTwoFloats(mJitterTranslationFactorYmin, mJitterTranslationFactorYmax));
            mJitterTranslationElapsedTime = 0.0f;
        }
    }

    if (mJitterScale)
    {
        mJitterScaleElapsedTime += mJitterElapsedTime;
        if (mJitterScaleElapsedTime > mJitterScaleInterval)
        {
            setScale(randomBetweenTwoFloats(mJitterScaleMin, mJitterScaleMax));
            mJitterScaleElapsedTime = 0.0f;
        }
    }

    if (mJitterForce)
    {
        mJitterForceElapsedTime += mJitterElapsedTime;
        if (mJitterForceElapsedTime > mJitterForceInterval)
        {
            setForce(randomBetweenTwoFloats(mJitterForceMin, mJitterForceMax));
            mJitterForceElapsedTime = 0.0f;
        }
    }

    if (mJitterPaintColour)
    {
        mJitterPaintColourElapsedTime += mJitterElapsedTime;
        if (mJitterPaintColourElapsedTime > mJitterPaintColourInterval)
        {
            setPaintColour(randomBetweenTwoColours(mJitterPaintColourMin, mJitterPaintColourMax));
            mJitterPaintColourElapsedTime = 0.0f;
        }
    }

    if (mJitterMirrorHorizontal)
    {
        mJitterMirrorHorizontalElapsedTime += mJitterElapsedTime;
        if (mJitterMirrorHorizontalElapsedTime > mJitterMirrorHorizontalInterval)
        {
            mMirrorHorizontal = randomBool();
            mJitterMirrorHorizontalElapsedTime = 0.0f;
        }
    }

    if (mJitterMirrorVertical)
    {
        mJitterMirrorVerticalElapsedTime += mJitterElapsedTime;
        if (mJitterMirrorVerticalElapsedTime > mJitterMirrorVerticalInterval)
        {
            mMirrorVertical = randomBool();
            mJitterMirrorVerticalElapsedTime = 0.0f;
        }
    }
}

//****************************************************************************/
const Ogre::IdString& PaintLayer::getDatablockId (void)
{
    if (!mTextureLayer)
        return dummyDatablockId;

    return mTextureLayer->mDatablockId;
}

//****************************************************************************/
void PaintLayer::saveTextureGeneration (void)
{
    // It is never possible to save a texture when the paintlayer is not visible
    if (!mVisible)
        return;

    mTextureLayer->saveTextureGeneration ();
}

//****************************************************************************/
void PaintLayer::createCarbonCopyTexture (void)
{
    // mCarbonCopyTextureFileName is only filled if the effect is actually set
    if (mCarbonCopyTextureFileName == "")
        return;

    if (!mTextureLayer)
        return;

    Ogre::Image tempCarbonCopyTexture;

    // Load the Carbon Copy texture; take scaling into account (scaling is implemented as a resize)
    Ogre::uint32 w = mTextureLayer->mTextureOnWhichIsPaintedWidth;
    Ogre::uint32 h = mTextureLayer->mTextureOnWhichIsPaintedHeight;
    tempCarbonCopyTexture.load(mCarbonCopyTextureFileName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    Ogre::uint32 widthLoadedImage = mCarbonCopyTextureScale * tempCarbonCopyTexture.getWidth();
    Ogre::uint32 heightLoadedImage = mCarbonCopyTextureScale * tempCarbonCopyTexture.getHeight();
    widthLoadedImage = widthLoadedImage  > w ? w : widthLoadedImage;
    heightLoadedImage = heightLoadedImage  > h ? h : heightLoadedImage;
    tempCarbonCopyTexture.resize(widthLoadedImage, heightLoadedImage);
    Ogre::uint32 xMapped = 0;
    Ogre::uint32 yMapped = 0;

    // Delete the 'old' data if available
    if (mCarbonCopydata)
    {
        delete [] mCarbonCopydata;
        mCarbonCopydata = 0;
    }

    // Create an empty image and fill it with the loaded texture
    size_t formatSize = Ogre::PixelUtil::getNumElemBytes(Ogre::PF_R8G8B8A8);
    mCarbonCopydata = new uchar[w * h * formatSize];
    mCarbonCopyTexture.loadDynamicImage(mCarbonCopydata, w, h, Ogre::PF_A8R8G8B8);

    // Copy the loaded texture data into the final Carbon Copy texture
    // Take the dimensions into account
    Ogre::PixelBox tempPixelboxCarbonCopyTexture = tempCarbonCopyTexture.getPixelBox(0, 0);
    mPixelboxCarbonCopyTexture = mCarbonCopyTexture.getPixelBox(0, 0);
    Ogre::ColourValue col;
    for (Ogre::uint32 y = 0; y < h; y++)
    {
        yMapped = y % heightLoadedImage;
        for (Ogre::uint32 x = 0; x < w; x++)
        {
            // Determine the colour value
            xMapped = x % widthLoadedImage;
            col = tempPixelboxCarbonCopyTexture.getColourAt(xMapped, yMapped, 0);
            mPixelboxCarbonCopyTexture.setColourAt(col, x, y, 0);
        }
    }
}

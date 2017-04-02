/****************************************************************************
**
** Copyright (C) 2016
**
** This file is generated by the Magus toolkit
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

#ifndef PAINT_LAYER_H
#define PAINT_LAYER_H

#include "OgreRoot.h"
#include "OgreImage.h"
#include "OgreTexture.h"
#include "OgreMath.h"
#include "OgreVector2.h"
#include "OgreColourValue.h"
#include "OgreTexture.h"
#include "OgrePixelBox.h"
#include "OgreItem.h"
#include "OgreHlmsPbsDatablock.h"
#include "OgreHlmsDatablock.h"
#include "OgreHardwarePixelBuffer.h"
#include "texturelayer.h"
#include <ctime>

/****************************************************************************
 This class contains functions used by the Hlms editor painter window. A
 paintlayer represents a layer, assigned to a certain texture of a Hlms
 material. E.g. A PaintLayer can be assinged to a diffuse texture. The paint
 characteristics are set in the PaintLayer object.

 Each Hlms material can have multiple PaintLayer assigned to it. Each texture
 type (diffuse, normal, ...) can also have multiple PaintLayer assigned to it.
 Layers can be painted individually, but painting can also be done at once
 (a mouse paint stroke involves multiple PaintLayer objects).
 ***************************************************************************/

class PaintLayerManager;
class PaintLayer
{
    public:
        enum PaintEffects
        {
            PAINT_EFFECT_COLOR, /// Paint with color
            PAINT_EFFECT_ERASE, /// Erase what was painted, using the original texture
            PAINT_EFFECT_ALPHA, /// Paint with alpha
            PAINT_EFFECT_TEXTURE, /// Use the colours of the brush
            PAINT_EFFECT_CARBON_COPY, /// Use another texture to 'carbon copy' that texture to the destination texture; uses a brush
            PAINT_EFFECT_SMUDGE /// Smudge the target texture using the shape of the brush
        };

        enum PaintOverflowTypes
        {
            PAINT_OVERFLOW_IGNORE, /// If the brush exceeds the texture areas on which is painted, the overflow is ignored
            PAINT_OVERFLOW_CONTINUE  /// If the brush exceeds the texture areas on which is painted, the overflow is continued on the opposite corner
        };

        /* Constructor/destructor
         * The constructor can have an external id, used for fast search search
         */
        PaintLayer(PaintLayerManager* paintLayerManager, int externalLayerId = -1);
        ~PaintLayer(void);

        /* Enable or disable the layer for painting
         */
        void enable(bool enabled);
        bool isEnabled (void) {return mEnabled;}

        /* If not visible, the layer cannot be painted on
         */
        void setVisible (bool visible);
        bool isVisible(void);

        /* Apply the paint effect.
         * u and v are the texture coordinates, between [0..1]
         * 'start' indicates whether the painting was started or ongoing
         */
        void paint(float u, float v, bool start);

        /* Set/get the Carbon Copy texture settings
         */
        void setCarbonCopyTextureFileName (const Ogre::String& textureFileName);
        const Ogre::String& getCarbonCopyTextureFileName (void);

        /* Set/get the Carbon Copye texture scale
         */
        void setCarbonCopyScale (float scale);
        float getCarbonCopyScale (void);

        /* Set the name of the brush used for painting.
         */
        void setBrush (const Ogre::String& brushFileName);

        /* Set/get the reference to the texture.
         */
        void setTextureLayer (TextureLayer* textureLayer);
        TextureLayer* getTextureLayer (void);

        /* Set/get scale of the brush.
         */
        void setScale (float scale);
        float getScale (void) const {return mScale;}

        /* Set/get jitter scale.
         * This setting creates a jittering effect in which the scale changes between two values
         */
        void setJitterScale (float jitterScaleMin, float jitterScaleMax);
        void setJitterScaleMin (float jitterScaleMin);
        void setJitterScaleMax (float jitterScaleMax);
        float getJitterScaleMin (void) const {return mJitterScaleMin;}
        float getJitterScaleMax (void) const {return mJitterScaleMax;}

        /* Determines the frequency of the jitter effect. The interval is set to seconds.
         */
        void setJitterScaleInterval (float interval);
        float getJitterScaleInterval (void) const {return mJitterScaleInterval;}

        /* Determines whether there is jitter scale
         */
        bool hasJitterScaleEnabled (void);
        void setJitterScaleEnabled (bool enabled);

        /* Exclude scaling the brush image
         * This also stops scale jitter.
         */
        void resetScale (void);

        /* Determine how much pressure is put on the brush. Value [0..1]
         */
        void setForce (float force);
        float getForce (void) const {return mForce;}

        /* Set jitter brush force.
         * This setting creates a jittering effect in which the brush force changes between two values
         */
        void setJitterForce (float jitterForceMin, float jitterForceMax);
        void setJitterForceMin (float jitterForceMin);
        void setJitterForceMax (float jitterForceMax);
        float getJitterForceMin (void) const {return mJitterForceMin;}
        float getJitterForceMax (void) const {return mJitterForceMax;}

        /* Determines the frequency of the jitter effect. The interval is set to seconds.
         */
        void setJitterForceInterval (float interval);
        float getJitterForceInterval (void) const {return mJitterForceInterval;}

        /* Determines whether there is jitter force
         */
        bool hasJitterForceEnabled (void);
        void setJitterForceEnabled (bool enabled);

        /* Stop with force jitter
         */
        void resetForce (void);

        /* Determine the rotation of the angle in degrees [0..360]
         */
        void setRotationAngle (float rotationAngle);
        float getRotationAngle (void) const {return mRotationAngle;}

        /* Set/get jitter angle for rotating the brush.
         * This setting creates a jittering effect in which the brush rotation changes between two values
         */
        void setJitterRotationAngle (float rotationAngleMin, float rotationAngleMax);
        void setJitterRotationAngleMin (float rotationAngleMin);
        void setJitterRotationAngleMax (float rotationAngleMax);
        float getJitterRotationAngleMin (void) const {return mJitterRotationAngleMin;}
        float getJitterRotationAngleMax (void) const {return mJitterRotationAngleMax;}

        /* Determines the frequency of the jitter effect. The interval is set to seconds.
         */
        void setJitterRotationAngleInterval (float interval);
        float getJitterRotationAngleInterval (void) const {return mJitterRotationAngleInterval;}

        /* Determines whether there is jitter rotation
         */
        bool hasJitterRotationEnabled (void);
        void setJitterRotationEnabled (bool enabled);

        /* Exclude rotating the brush image
         * This also stops rotation jitter
         */
        void resetRotation (void);

        /* Determine the translation. This is a relative value [0..1]
         */
        void setTranslationFactor (float translationFactorX, float translationFactorY);
        float getTranslationFactorX (void) const {return mTranslationFactorX;}
        float getTranslationFactorY (void) const {return mTranslationFactorY;}

        /* Set jitter brush translation.
         * This setting creates a jittering effect in which the brush translation changes between two values
         */
        void setJitterTranslationFactor (float jitterTranslationFactorXmin,
                                         float jitterTranslationFactorXmax,
                                         float jitterTranslationFactorYmin,
                                         float jitterTranslationFactorYmax);
        void setJitterTranslationFactorX (float jitterTranslationFactorXmin,
                                         float jitterTranslationFactorXmax);
        void setJitterTranslationFactorY (float jitterTranslationFactorYmin,
                                         float jitterTranslationFactorYmax);
        const Ogre::Vector2& getJitterTranslationFactorX (void);
        const Ogre::Vector2& getJitterTranslationFactorY (void);

        /* Determines the frequency of the jitter effect. The interval is set to seconds.
         */
        void setJitterTranslationInterval (float interval);
        float getJitterTranslationInterval (void) {return mJitterTranslationInterval;}

        /* Determines whether there is jitter translation
         */
        bool hasJitterTranslationEnabled (void);
        void setJitterTranslationEnabled (bool enabled);

        /* Exclude translating the brush image
         * This also stops translation jitter.
         */
        void resetTranslation (void);

        /* Set/get the paint effect.
         */
        void setPaintEffect (PaintEffects paintEffect);
        PaintEffects getPaintEffect (void) const {return mPaintEffect;}

        /* Set/get the paint overflow.
         */
        void setPaintOverflow (PaintOverflowTypes paintOverflow);
        PaintOverflowTypes getPaintOverflow (void) const {return mPaintOverflow;}

        /* Set/get colour used for painting.
         */
        void setPaintColour (const Ogre::ColourValue& colourValue);
        const Ogre::ColourValue& getPaintColour (void) const {return mPaintColour;}

        /* Set jitter colour for painting the brush.
         * This setting creates a jittering effect in which the paint colour changes between two values
         */
        void setJitterPaintColour (const Ogre::ColourValue& paintColourMin, const Ogre::ColourValue& paintColourMax);
        void setJitterPaintColourMin (const Ogre::ColourValue& paintColourMin);
        void setJitterPaintColourMax (const Ogre::ColourValue& paintColourMax);
        const Ogre::ColourValue& getJitterPaintColourMin (void) const {return mJitterPaintColourMin;}
        const Ogre::ColourValue& getJitterPaintColourMax (void) const {return mJitterPaintColourMax;}

        /* Determines the frequency of the jitter effect. The interval is set to seconds.
         */
        void setJitterPaintColourInterval (float interval);
        float getJitterPaintColourInterval (void) const {return mJitterPaintColourInterval;}

        /* Determines whether there is jitter paint
         */
        bool hasJitterPaintEnabled (void);
        void setJitterPaintEnabled (bool enabled);

        /* Stop with paint colour jitter
         */
        void resetPaintColour (void);

        /* Set/get the smudge decay value
         */
        void setSmudgeDecay(float decay) {mSmudgeDecay = decay;}
        float getSmudgeDecay(void) const {return mSmudgeDecay;}

        /* Mirror the brush horizontally
         */
        void setMirrorHorizontal (bool mirrored);
        bool getMirrorHorizontal (void) const {return mMirrorHorizontal;}

        /* Set jitter mirror horizontal.
         * This setting creates an alternating mirror (horizontally) effect of the brush
         */
        void setJitterMirrorHorizontal (bool enabled);
        bool getJitterMirrorHorizontal (void) const {return mJitterMirrorHorizontal;}

        /* Determines the frequency of the jitter effect. The interval is set to seconds.
         */
        void setJitterMirrorHorizontalInterval (float interval);
        float getJitterMirrorHorizontalInterval (void) const {return mJitterMirrorHorizontalInterval;}

        /* Determines whether there is jitter mirrot horizontal
         */
        bool hasJitterMirrorHorizontalEnabled (void);
        void setJitterMirrorHorizontalEnabled (bool enabled);

        /* Exclude the mirror (horizontally) effect.
         * Stop the jittering effect
         */
        void resetMirrorHorizontal (void);

        /* Mirror the brush vertically
         */
        void setMirrorVertical (bool mirrored);
        bool getMirrorVertical (void) const {return mMirrorVertical;}

        /* Set jitter mirror vertical.
         * This setting creates an alternating mirror (vertically) effect of the brush
         */
        void setJitterMirrorVertical (bool enabled);
        bool getJitterMirrorVertical (void) const {return mJitterMirrorVertical;}

        /* Determines the frequency of the jitter effect. The interval is set to seconds.
         */
        void setJitterMirrorVerticalInterval (float interval);
        float getJitterMirrorVerticalInterval (void) const {return mJitterMirrorVerticalInterval;}

        /* Determines whether there is jitter mirrot vertical
         */
        bool hasJitterMirrorVerticalEnabled (void);
        void setJitterMirrorVerticalEnabled (bool enabled);

        /* Exclude the mirror (vertical) effect.
         * Stop the jittering effect
         */
        void resetMirrorVertical (void);

        /* Returns the datablock id (IdString) of the datablock used by this PaintLayer
         */
        const Ogre::IdString& getDatablockId (void);

        /* Returns the external layer Id
         * Is -1 when not used
         */
        int getExternaLayerlId (void) const {return mExternaLayerlId;}

        /* Save the texture file associated by this PaintLayer. Each file is padded with a sequence number,
         * used for undo and redo functions.
         */
        void saveTextureGeneration (void);

        /* Set the smudge image to black, using the dimensions of the brush
         */

    private:
            int mExternaLayerlId;                           // May be used as a reference for external classes; is -1 if no external id is passed
            clock_t mStartTime;
            clock_t mEndTime;
            bool mEnabled;                                  // If enabled, the layer is painted, otherwise it is skipped
            bool mVisible;                                  // If visible, the layer can be painted on, otherwise it is skipped (even if it was enabled)
            Ogre::Image mBrush;                             // Image of the brush
            Ogre::String mBrushFileName;                    // Full qualified name of the brush file
            Ogre::PixelBox mPixelboxBrush;                  // Pixelbox of mBrush; for speed purposes, it is created when the brush is set
            Ogre::Image mBrushSmudge;                       // Image of the smudge brush
            Ogre::PixelBox mPixelboxBrushSmudge;            // Pixelbox of the target texture, using the shape of the brush; used for smudge effect
            size_t mBrushWidth;                             // Width of mBrush
            size_t mBrushHeight;                            // Height of mBrush
            size_t mHalfBrushWidth;                         // For efficient calculation
            size_t mHalfBrushWidthScaled;                   // For efficient calculation
            size_t mHalfBrushHeight;                        // For efficient calculation
            size_t mHalfBrushHeightScaled;                  // For efficient calculation
            size_t mBrushWidthMinusOne;                     // For efficient calculation
            size_t mBrushHeightMinusOne;                    // For efficient calculation
            float mForce;                                   // Factor that determines how must pressure is put on the brush; value between [0.0f, 1.0f].
            float mScale;                                   // Factor that scales the brush; value between [0.0f, 1.0f].
            Ogre::ColourValue mPaintColour;                 // The colour used for painting. Red, Green and Blue values are used when PAINT_EFFECT_COLOR is passed.
                                                            // Alpha value is used if PAINT_EFFECT_ALPHA is used.
            Ogre::ColourValue mFinalColour;                 // The calculated colour, based on mPaintColour and the brush, applied to the texture.
            PaintEffects mPaintEffect;                      // Type of paint effect.
            PaintOverflowTypes mPaintOverflow;              // Determine what happens if the brush exceeds the texture areas on which is painted.
            TextureLayer* mTextureLayer;                    // Reference to the texture (can be shared by other PaintLayers)
            size_t mCalculatedTexturePositionX;
            size_t mCalculatedTexturePositionY;
            float mAlpha;
            float mAlphaDecay;
            bool mRotate;                                   // If true, the brush is rotated
            float mRotationAngle;                           // The angle of rotating the brush
            float mSinRotationAngle;                        // Sin of the angle of rotating
            float mCosRotationAngle;                        // Cos of the angle of rotating
            int mPosX;
            int mPosY;
            int mPosXrotated;
            int mPosYrotated;
            bool mTranslate;                                // If true, the brush is translated
            float mTranslationFactorX;                      // The relative translation factor for X [-1..1]
            float mTranslationFactorY;                      // The relative translation factor for Y [-1..1]
            int mTranslationX;                              // The absolute X translation value in pixels; can be negative or positive
            int mTranslationY;                              // The absolute Y translation value in pixels; can be negative or positive
            bool mMirrorHorizontal;                         // Mirrors a brush horizontally
            bool mMirrorVertical;                           // Mirrors a brush vertically
            float mSmudgeDecay;                             // Determines how long the smudge effect of one paintstroke lasts

            // Jitter attributes
            bool mJitterRotate;                             // If true, a jittering rotation is applied
            bool mJitterTranslate;                          // If true, a jittering translation is applied
            bool mJitterScale;                              // If true, a jittering scale is applied
            bool mJitterForce;                              // If true, a jittering brush force is applied
            bool mJitterPaintColour;                        // If true, a brush with jittering colours is applied
            bool mJitterMirrorHorizontal;                   // If true, the brush is randomly mirrored horizontally
            bool mJitterMirrorVertical;                     // If true, the brush is randomly mirrored vertically
            float mJitterRotationAngleInterval;             // Interval of the jitter in seconds
            float mJitterTranslationInterval;               // ,,
            float mJitterScaleInterval;                     // ,,
            float mJitterForceInterval;                     // ,,
            float mJitterPaintColourInterval;               // ,,
            float mJitterMirrorHorizontalInterval;          // ,,
            float mJitterMirrorVerticalInterval;            // ,,
            float mJitterElapsedTime;                       // Latest time probe
            float mJitterRotateElapsedTime;                 // Latest time probe for this specific effect
            float mJitterTranslationElapsedTime;            // ,,
            float mJitterScaleElapsedTime;                  // ,,
            float mJitterForceElapsedTime;                  // ,,
            float mJitterPaintColourElapsedTime;            // ,,
            float mJitterMirrorHorizontalElapsedTime;       // ,,
            float mJitterMirrorVerticalElapsedTime;         // ,,
            float mJitterRotationAngleMin;                  // Min. value used for jittering rotation
            float mJitterRotationAngleMax;                  // Max. value used for jittering rotation
            float mJitterTranslationFactorXmin;             // Min. value used for jittering translation X factor
            float mJitterTranslationFactorXmax;             // Max. value used for jittering translation X factor
            float mJitterTranslationFactorYmin;             // Min. value used for jittering translation Y factor
            float mJitterTranslationFactorYmax;             // Max. value used for jittering translation Y factor
            float mJitterScaleMin;                          // Min. value used for jittering scale
            float mJitterScaleMax;                          // Max. value used for jittering scale
            float mJitterForceMin;                          // Min. value used for jittering force
            float mJitterForceMax;                          // Max. value used for jittering force
            Ogre::ColourValue mJitterPaintColourMin;        // Min. value used for jittering paint colour
            Ogre::ColourValue mJitterPaintColourMax;        // Max. value used for jittering paint colour
            PaintLayerManager* mPaintLayerManager;
            Ogre::IdString dummyDatablockId;
            Ogre::Vector2 mHelperVector2;
            Ogre::String mHelperOgreString;
            Ogre::Image mCarbonCopyTexture;                 // The (final) texture used for the Carbon Copy effect
                                                            // It is build from a loaded texture which may have different dimensions
            Ogre::PixelBox mPixelboxCarbonCopyTexture;      // Pixelbox of the final Carbon Copy texture
            Ogre::String  mCarbonCopyTextureFileName;       // Full qualified name of the texture file used in a Carbon Copy effect; this attribute is private,
                                                            // because the acces must be done by means of the get/set functions, which contain additional actions
            uchar* mCarbonCopydata;                         // Data used for the Carbon Copy texture
            float mCarbonCopyTextureScale;                  // Scaling factor of the loaded Carbon Copy texture

            // Private functions
            size_t calculateTexturePositionX (float u, size_t brushPositionX);
            size_t calculateTexturePositionY (float v, size_t brushPositionY);
            void determineJitterEffects (void);

            /* Create the texture used for the Carbon Copy effect
             */
            void createCarbonCopyTexture (void);
};

typedef std::vector<PaintLayer*> PaintLayers;

#endif


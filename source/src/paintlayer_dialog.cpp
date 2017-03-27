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

#include <QtWidgets>
#include <QVBoxLayout>
#include "constants.h"
#include "paintlayer_dialog.h"
#include "paintlayer_widget.h"
#include "asset_assetwidget.h"
#include "asset_propertywidget.h"

//****************************************************************************/
PaintLayerDialog::PaintLayerDialog(PaintLayerWidget* paintLayerWidget, QtLayer* layer) :
    QDialog(paintLayerWidget),
    mPaintLayerWidget(paintLayerWidget),
    mQtLayer(layer)
{
    mTextureTypeSelectProperty = 0;
    mPaintEffectSelectProperty = 0;
    mPaintOverflowSelectProperty = 0;

    mBurnTextureProperty  = 0;

    mPaintColourProperty = 0;
    mPaintJitterCheckboxProperty = 0;
    mPaintColourJitterMinProperty = 0;
    mPaintColourJitterMaxProperty = 0;
    mPaintColourJitterIntervalProperty = 0;

    mForceProperty = 0;
    mJitterForceCheckboxProperty = 0;
    mJitterForceMinProperty = 0;
    mJitterForceMaxProperty = 0;
    mJitterForceIntervalProperty = 0;

    mScaleProperty = 0;
    mJitterScaleCheckboxProperty = 0;
    mJitterScaleMinProperty = 0;
    mJitterScaleMaxProperty = 0;
    mJitterScaleIntervalProperty = 0;

    mRotationAngleProperty = 0;
    mJitterRotationAngleCheckboxProperty = 0;
    mJitterRotationAngleMinProperty = 0;
    mJitterRotationAngleMaxProperty = 0;
    mJitterRotationAngleIntervalProperty = 0;

    mTranslationXProperty = 0;
    mTranslationYProperty = 0;
    mJitterTranslationCheckboxProperty = 0;
    mJitterTranslationXMinProperty = 0;
    mJitterTranslationXMaxProperty = 0;
    mJitterTranslationYMinProperty = 0;
    mJitterTranslationYMaxProperty = 0;
    mJitterTranslationIntervalProperty = 0;

    mMirrorHorizontalProperty = 0;
    mJitterMirrorHorizontalProperty = 0;
    mJitterMirrorHorizontalIntervalProperty = 0;

    mMirrorVerticalProperty = 0;
    mJitterMirrorVerticalProperty = 0;
    mJitterMirrorVerticalIntervalProperty = 0;

    QFile File(QString("dark.qss"));
    File.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(File.readAll());
    setStyleSheet(styleSheet);

    setWindowTitle(QString("Properties paint layer: ") + layer->name);
    QVBoxLayout* mainLayout = new QVBoxLayout;
    Magus::QtAssetWidget* assetWidget = new Magus::QtAssetWidget(QString(""), "", this);
    connect(assetWidget, SIGNAL(valueChanged(QtProperty*)), this, SLOT(propertyValueChanged(QtProperty*)));
    assetWidget->setHeaderHidden(true); // Don't use a header

    // Create containers
    mContainerGeneral = assetWidget->createContainer(CONTAINER_PAINTLAYER_GENERAL, QString("General"));
    mContainerGeneral->setTitleBold(true);
    mContainerBurn = assetWidget->createContainer(CONTAINER_PAINTLAYER_BURN, QString("Texture Burn"));
    mContainerBurn->setTitleBold(true);
    mContainerPaint = assetWidget->createContainer(CONTAINER_PAINTLAYER_COLOUR, QString("Colour painting"));
    mContainerPaint->setTitleBold(true);
    mContainerScale = assetWidget->createContainer(CONTAINER_PAINTLAYER_SCALE, QString("Brush scale"));
    mContainerScale->setTitleBold(true);
    mContainerForce = assetWidget->createContainer(CONTAINER_PAINTLAYER_FORCE, QString("Brush force"));
    mContainerForce->setTitleBold(true);
    mContainerRotationAngle = assetWidget->createContainer(CONTAINER_PAINTLAYER_ROTATION_ANGLE, QString("Brush rotation angle"));
    mContainerRotationAngle->setTitleBold(true);
    mContainerTranslation = assetWidget->createContainer(CONTAINER_PAINTLAYER_TRANSLATION, QString("Brush translation"));
    mContainerTranslation->setTitleBold(true);
    mContainerMirrorHorizontal = assetWidget->createContainer(CONTAINER_PAINTLAYER_MIRROR_HORIZONTAL, QString("Brush mirror horizontal"));
    mContainerMirrorHorizontal->setTitleBold(true);
    mContainerMirrorVertical = assetWidget->createContainer(CONTAINER_PAINTLAYER_MIRROR_VERTICAL, QString("Brush mirror vertical"));
    mContainerMirrorVertical->setTitleBold(true);

    // *******************************************************************************************************************
    // ***************************************************** General *****************************************************
    // *******************************************************************************************************************
    QStringList stringListAvailableTextures = paintLayerWidget->getAvailableTextureTypes();
    mTextureTypeSelectProperty = static_cast<Magus::QtSelectProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_GENERAL, PROPERTY_TEXTURE_TYPE, QString("Texture type"), Magus::QtProperty::SELECT));
    mTextureTypeSelectProperty->addValues(stringListAvailableTextures);
    mTextureTypeSelectProperty->setCurentIndex(-1);

    // ******** Paint effect ********
    QStringList stringListPaintEffects;
    stringListPaintEffects << PAINT_EFFECT_COLOR_QSTRING <<
                              PAINT_EFFECT_ERASE_QSTRING <<
                              PAINT_EFFECT_ALPHA_QSTRING <<
                              PAINT_EFFECT_TEXTURE_QSTRING <<
                              PAINT_EFFECT_BURN_QSTRING;
    mPaintEffectSelectProperty = static_cast<Magus::QtSelectProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_GENERAL, PROPERTY_PAINT_EFFECT, QString("Paint effect"), Magus::QtProperty::SELECT));
    mPaintEffectSelectProperty->addValues(stringListPaintEffects);

    // ******** Paint overflow ********
    QStringList stringListPaintOverflow;
    stringListPaintOverflow << PAINT_OVERFLOW_IGNORE_QSTRING <<
                               PAINT_OVERFLOW_CONTINUE_QSTRING;
    mPaintOverflowSelectProperty = static_cast<Magus::QtSelectProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_GENERAL, PROPERTY_PAINT_OVERFLOW, QString("Overflow"), Magus::QtProperty::SELECT));
    mPaintOverflowSelectProperty->addValues(stringListPaintOverflow);
    mPaintOverflowSelectProperty->setCurentIndex(1); // Continue is the default

    // *******************************************************************************************************************
    // ***************************************************** Burn *****************************************************
    // *******************************************************************************************************************
    mBurnTextureProperty = static_cast<Magus::QtTextureProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_BURN, PROPERTY_BURN_TEXTURE, QString("Texture"), Magus::QtProperty::TEXTURE));
    enableBurnContainer (false);

    // ************************************************************************************************************************
    // ***************************************************** Paint colour *****************************************************
    // ************************************************************************************************************************
    mPaintColourProperty = static_cast<Magus::QtColorProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_COLOUR, PROPERTY_PAINT_COLOUR, QString("Paint colour"), Magus::QtProperty::COLOR));

    // ******** Jitter paint colour checkbox ********
    mPaintJitterCheckboxProperty = static_cast<Magus::QtCheckBoxProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_COLOUR, PROPERTY_JITTER_PAINT_COLOUR_ENABLED, QString("Jitter colour"), Magus::QtProperty::CHECKBOX));
    mPaintJitterCheckboxProperty->setValue(false);

    // ******** Jitter paint colour (min/max) ********
    mPaintColourJitterMinProperty = static_cast<Magus::QtColorProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_COLOUR, PROPERTY_JITTER_PAINT_COLOUR_MIN, QString("Jitter paint colour min"), Magus::QtProperty::COLOR));
    mPaintColourJitterMinProperty->setVisible(false);

    mPaintColourJitterMaxProperty = static_cast<Magus::QtColorProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_COLOUR, PROPERTY_JITTER_PAINT_COLOUR_MAX, QString("Jitter paint colour max"), Magus::QtProperty::COLOR));
    mPaintColourJitterMaxProperty->setVisible(false);

    // ******** Jitter paint colour interval (seconds) ********
    mPaintColourJitterIntervalProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_COLOUR, PROPERTY_JITTER_PAINT_COLOUR_INTERVAL, QString("Jitter paint colour interval (sec.)"), Magus::QtProperty::SLIDER_DECIMAL));
    mPaintColourJitterIntervalProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mPaintColourJitterIntervalProperty->setValue(0.0f);
    mPaintColourJitterIntervalProperty->setVisible(false);

    // *****************************************************************************************************************
    // ***************************************************** Scale *****************************************************
    // *****************************************************************************************************************
    mScaleProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_SCALE, PROPERTY_SCALE, QString("Brush scale"), Magus::QtProperty::SLIDER_DECIMAL));
    mScaleProperty->setSliderRange (0.0f, 1.0f, 0.125f);
    mScaleProperty->setValue(0.1f);

    // ******** Jitter scale checkbox ********
    mJitterScaleCheckboxProperty = static_cast<Magus::QtCheckBoxProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_SCALE, PROPERTY_JITTER_SCALE_ENABLED, QString("Jitter brush scale"), Magus::QtProperty::CHECKBOX));
    mJitterScaleCheckboxProperty->setValue(false);

    // ******** Jitter scale (min/max) ********
    mJitterScaleMinProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_SCALE, PROPERTY_JITTER_SCALE_MIN, QString("Jitter brush scale min"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterScaleMinProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mJitterScaleMinProperty->setValue(0.0f);
    mJitterScaleMinProperty->setVisible(false);

    mJitterScaleMaxProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_SCALE, PROPERTY_JITTER_SCALE_MAX, QString("Jitter brush scale max"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterScaleMaxProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mJitterScaleMaxProperty->setValue(1.0f);
    mJitterScaleMaxProperty->setVisible(false);

    // ******** Jitter scale interval (seconds) ********
    mJitterScaleIntervalProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_SCALE, PROPERTY_JITTER_SCALE_INTERVAL, QString("Jitter brush scale interval"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterScaleIntervalProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mJitterScaleIntervalProperty->setValue(1.0f);
    mJitterScaleIntervalProperty->setVisible(false);

    // *****************************************************************************************************************
    // ***************************************************** Force *****************************************************
    // *****************************************************************************************************************
    mForceProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_FORCE, PROPERTY_FORCE, QString("Brush force"), Magus::QtProperty::SLIDER_DECIMAL));
    mForceProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mForceProperty->setValue(1.0f);

    // ******** Jitter force checkbox  ********
    mJitterForceCheckboxProperty = static_cast<Magus::QtCheckBoxProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_FORCE, PROPERTY_JITTER_FORCE_ENABLED, QString("Jitter brush force"), Magus::QtProperty::CHECKBOX));
    mJitterForceCheckboxProperty->setValue(false);

    // ******** Jitter force (min/max) ********
    mJitterForceMinProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_FORCE, PROPERTY_JITTER_FORCE_MIN, QString("Jitter brush force min"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterForceMinProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mJitterForceMinProperty->setValue(0.0f);
    mJitterForceMinProperty->setVisible(false);

    mJitterForceMaxProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_FORCE, PROPERTY_JITTER_FORCE_MAX, QString("Jitter brush force max"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterForceMaxProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mJitterForceMaxProperty->setValue(0.0f);
    mJitterForceMaxProperty->setVisible(false);

    // ******** Jitter force interval (seconds) ********
    mJitterForceIntervalProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_FORCE, PROPERTY_JITTER_FORCE_INTERVAL, QString("Jitter brush force interval"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterForceIntervalProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mJitterForceIntervalProperty->setValue(1.0f);
    mJitterForceIntervalProperty->setVisible(false);

    // **************************************************************************************************************************
    // ***************************************************** Rotation angle *****************************************************
    // **************************************************************************************************************************
    mRotationAngleProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_ROTATION_ANGLE, PROPERTY_ROTATION_ANGLE, QString("Brush rotation angle"), Magus::QtProperty::SLIDER_DECIMAL));
    mRotationAngleProperty->setSliderRange (0.0f, 360.0f, 1.0f);
    mRotationAngleProperty->setValue(0.0f);

    // ******** Jitter rotation angle checkbox  ********
    mJitterRotationAngleCheckboxProperty = static_cast<Magus::QtCheckBoxProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_ROTATION_ANGLE, PROPERTY_JITTER_ROTATION_ANGLE_ENABLED, QString("Jitter brush rotation angle"), Magus::QtProperty::CHECKBOX));
    mJitterRotationAngleCheckboxProperty->setValue(false);

    // ******** Jitter rotation angle (min/max) ********
    mJitterRotationAngleMinProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_ROTATION_ANGLE, PROPERTY_JITTER_ROTATION_ANGLE_MIN, QString("Jitter brush rotation angle min"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterRotationAngleMinProperty->setSliderRange (0.0f, 360.0f, 1.0f);
    mJitterRotationAngleMinProperty->setValue(0.0f);
    mJitterRotationAngleMinProperty->setVisible(false);

    mJitterRotationAngleMaxProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_ROTATION_ANGLE, PROPERTY_JITTER_ROTATION_ANGLE_MAX, QString("Jitter brush rotation angle max"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterRotationAngleMaxProperty->setSliderRange (0.0f, 360.0f, 1.0f);
    mJitterRotationAngleMaxProperty->setValue(360.0f);
    mJitterRotationAngleMaxProperty->setVisible(false);

    // ******** Jitter rotation angle interval (seconds) ********
    mJitterRotationAngleIntervalProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_ROTATION_ANGLE, PROPERTY_JITTER_ROTATION_ANGLE_INTERVAL, QString("Jitter brush rotation interval"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterRotationAngleIntervalProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mJitterRotationAngleIntervalProperty->setValue(0.0f);
    mJitterRotationAngleIntervalProperty->setVisible(false);

    // ***********************************************************************************************************************
    // ***************************************************** Translation *****************************************************
    // ***********************************************************************************************************************
    mTranslationXProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_TRANSLATION, PROPERTY_TRANSLATION_X, QString("Translation X"), Magus::QtProperty::SLIDER_DECIMAL));
    mTranslationXProperty->setSliderRange (-1.0f, 1.0f, 0.01f);
    mTranslationXProperty->setValue(0.0f);
    mTranslationXProperty->setVisible(true);

    mTranslationYProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_TRANSLATION, PROPERTY_TRANSLATION_Y, QString("Translation Y"), Magus::QtProperty::SLIDER_DECIMAL));
    mTranslationYProperty->setSliderRange (-1.0f, 1.0f, 0.01f);
    mTranslationYProperty->setValue(0.0f);
    mTranslationYProperty->setVisible(true);

    // ******** Jitter translation checkbox  ********
    mJitterTranslationCheckboxProperty = static_cast<Magus::QtCheckBoxProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_TRANSLATION, PROPERTY_JITTER_TRANSLATION_ENABLED, QString("Jitter translation"), Magus::QtProperty::CHECKBOX));
    mJitterTranslationCheckboxProperty->setValue(false);

    // ******** Jitter translation (min/max X, min.max Y) ********
    mJitterTranslationXMinProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_TRANSLATION, PROPERTY_JITTER_TRANSLATION_X_MIN, QString("Jitter translation X min"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterTranslationXMinProperty->setSliderRange (-1.0f, 1.0f, 0.01f);
    mJitterTranslationXMinProperty->setValue(-1.0f);
    mJitterTranslationXMinProperty->setVisible(false);

    mJitterTranslationXMaxProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_TRANSLATION, PROPERTY_JITTER_TRANSLATION_X_MAX, QString("Jitter translation X max"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterTranslationXMaxProperty->setSliderRange (-1.0f, 1.0f, 0.01f);
    mJitterTranslationXMaxProperty->setValue(1.0f);
    mJitterTranslationXMaxProperty->setVisible(false);

    mJitterTranslationYMinProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_TRANSLATION, PROPERTY_JITTER_TRANSLATION_Y_MIN, QString("Jitter translation Y min"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterTranslationYMinProperty->setSliderRange (-1.0f, 1.0f, 0.01f);
    mJitterTranslationYMinProperty->setValue(-1.0f);
    mJitterTranslationYMinProperty->setVisible(false);

    mJitterTranslationYMaxProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_TRANSLATION, PROPERTY_JITTER_TRANSLATION_Y_MAX, QString("Jitter translation Y max"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterTranslationYMaxProperty->setSliderRange (-1.0f, 1.0f, 0.01f);
    mJitterTranslationYMaxProperty->setValue(1.0f);
    mJitterTranslationYMaxProperty->setVisible(false);

    // ******** Jitter translation interval (seconds) ********
    mJitterTranslationIntervalProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_TRANSLATION, PROPERTY_JITTER_TRANSLATION_INTERVAL, QString("Jitter translation interval"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterTranslationIntervalProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mJitterTranslationIntervalProperty->setValue(0.0f);
    mJitterTranslationIntervalProperty->setVisible(false);

    // *****************************************************************************************************************************
    // ***************************************************** Mirror Horizontal *****************************************************
    // *****************************************************************************************************************************
    mMirrorHorizontalProperty = static_cast<Magus::QtCheckBoxProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_MIRROR_HORIZONTAL, PROPERTY_MIRROR_HORIZONTAL, QString("Horizontal mirror"), Magus::QtProperty::CHECKBOX));

    // ******** Jitter mirror horizontal checkbox  ********
    mJitterMirrorHorizontalProperty = static_cast<Magus::QtCheckBoxProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_MIRROR_HORIZONTAL, PROPERTY_JITTER_MIRROR_HORIZONTAL_ENABLED, QString("Jitter horizontal mirror"), Magus::QtProperty::CHECKBOX));
    mJitterMirrorHorizontalProperty->setValue(false);

    // ******** Jitter mirror horizontal interval (seconds) ********
    mJitterMirrorHorizontalIntervalProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_MIRROR_HORIZONTAL, PROPERTY_JITTER_MIRROR_HORIZONTAL_INTERVAL, QString("Jitter horizontal mirror interval"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterMirrorHorizontalIntervalProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mJitterMirrorHorizontalIntervalProperty->setValue(0.0f);
    mJitterMirrorHorizontalIntervalProperty->setVisible(false);

    // ***************************************************************************************************************************
    // ***************************************************** Mirror Vertical *****************************************************
    // ***************************************************************************************************************************
    mMirrorVerticalProperty = static_cast<Magus::QtCheckBoxProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_MIRROR_VERTICAL, PROPERTY_MIRROR_VERTICAL, QString("Vertical mirror"), Magus::QtProperty::CHECKBOX));

    // ******** Jitter mirror vertical checkbox  ********
    mJitterMirrorVerticalProperty = static_cast<Magus::QtCheckBoxProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_MIRROR_VERTICAL, PROPERTY_JITTER_MIRROR_VERTICAL_ENABLED, QString("Jitter vertical mirror"), Magus::QtProperty::CHECKBOX));
    mJitterMirrorVerticalProperty->setValue(false);

    // ******** Jitter mirror vertical interval (seconds) ********
    mJitterMirrorVerticalIntervalProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(CONTAINER_PAINTLAYER_MIRROR_VERTICAL, PROPERTY_JITTER_MIRROR_VERTICAL_INTERVAL, QString("Jitter vertical mirror interval"), Magus::QtProperty::SLIDER_DECIMAL));
    mJitterMirrorVerticalIntervalProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mJitterMirrorVerticalIntervalProperty->setValue(0.0f);
    mJitterMirrorVerticalIntervalProperty->setVisible(false);

    // Close button
    QPushButton* closeButton = new QPushButton(tr("Close"));
    connect(closeButton, &QAbstractButton::clicked, this, &QWidget::close);
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    // Layout
    mainLayout->addWidget(assetWidget);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);
    assetWidget->expandAllContainers();
}

//****************************************************************************/
PaintLayerDialog::~PaintLayerDialog(void)
{
}

//****************************************************************************/
void PaintLayerDialog::propertyValueChanged(QtProperty* property)
{
    if (!property)
        return;

    Magus::QtCheckBoxProperty* checkboxProperty;
    Magus::QtSelectProperty* selectProperty;

    switch (property->mPropertyId)
    {
        case PROPERTY_JITTER_PAINT_COLOUR_ENABLED:
        {
            if (!mPaintColourProperty)
                return; // The other properties are not yet created

            checkboxProperty = static_cast<Magus::QtCheckBoxProperty*>(property);
            mPaintColourProperty->setVisible(!checkboxProperty->getValue());
            mPaintColourJitterMinProperty->setVisible(checkboxProperty->getValue());
            mPaintColourJitterMaxProperty->setVisible(checkboxProperty->getValue());
            mPaintColourJitterIntervalProperty->setVisible(checkboxProperty->getValue());
        }
        break;

        case PROPERTY_PAINT_EFFECT:
        {
            if (!mPaintJitterCheckboxProperty)
                return; // The other properties are not yet created

            selectProperty = static_cast<Magus::QtSelectProperty*>(property);
            if (selectProperty->getCurrentText() == PAINT_EFFECT_COLOR_QSTRING)
            {
                // Disable Burn container
                enableBurnContainer (false);

                // Enable colour container
                mContainerPaint->setVisible(true);
                mPaintJitterCheckboxProperty->setVisible(true);
                bool jitter = mPaintJitterCheckboxProperty->getValue();
                mPaintColourProperty->setVisible(!jitter);
                mPaintColourJitterMinProperty->setVisible(jitter);
                mPaintColourJitterMaxProperty->setVisible(jitter);
                mPaintColourJitterIntervalProperty->setVisible(jitter);
            }
            else if (selectProperty->getCurrentText() == PAINT_EFFECT_BURN_QSTRING)
            {
                // Enable Burn container
                enableBurnContainer (true);

                // Disable colour container
                enableColourContainer (false);

                //QMessageBox::information(0, QString("Warning"), QString("Not implemented yet"));
            }
            else
            {
                // Disable Burn container
                enableBurnContainer (false);

                // Disable colour container
                enableColourContainer (false);
            }
        }
        break;

        case PROPERTY_JITTER_SCALE_ENABLED:
        {
            if (!mScaleProperty)
                return; // The other properties are not yet created

            checkboxProperty = static_cast<Magus::QtCheckBoxProperty*>(property);
            mScaleProperty->setVisible(!checkboxProperty->getValue());
            mJitterScaleMinProperty->setVisible(checkboxProperty->getValue());
            mJitterScaleMaxProperty->setVisible(checkboxProperty->getValue());
            mJitterScaleIntervalProperty->setVisible(checkboxProperty->getValue());
        }
        break;

        case PROPERTY_JITTER_FORCE_ENABLED:
        {
            if (!mForceProperty)
                return; // The other properties are not yet created

            checkboxProperty = static_cast<Magus::QtCheckBoxProperty*>(property);
            mForceProperty->setVisible(!checkboxProperty->getValue());
            mJitterForceMinProperty->setVisible(checkboxProperty->getValue());
            mJitterForceMaxProperty->setVisible(checkboxProperty->getValue());
            mJitterForceIntervalProperty->setVisible(checkboxProperty->getValue());
        }
        break;

        case PROPERTY_JITTER_ROTATION_ANGLE_ENABLED:
        {
            if (!mRotationAngleProperty)
                return; // The other properties are not yet created

            checkboxProperty = static_cast<Magus::QtCheckBoxProperty*>(property);
            mRotationAngleProperty->setVisible(!checkboxProperty->getValue());
            mJitterRotationAngleMinProperty->setVisible(checkboxProperty->getValue());
            mJitterRotationAngleMaxProperty->setVisible(checkboxProperty->getValue());
            mJitterRotationAngleIntervalProperty->setVisible(checkboxProperty->getValue());
        }
        break;

        case PROPERTY_JITTER_TRANSLATION_ENABLED:
        {
            if (!mTranslationXProperty)
                return; // The other properties are not yet created

            checkboxProperty = static_cast<Magus::QtCheckBoxProperty*>(property);
            mTranslationXProperty->setVisible(!checkboxProperty->getValue());
            mTranslationYProperty->setVisible(!checkboxProperty->getValue());
            mJitterTranslationXMinProperty->setVisible(checkboxProperty->getValue());
            mJitterTranslationXMaxProperty->setVisible(checkboxProperty->getValue());
            mJitterTranslationYMinProperty->setVisible(checkboxProperty->getValue());
            mJitterTranslationYMaxProperty->setVisible(checkboxProperty->getValue());
            mJitterTranslationIntervalProperty->setVisible(checkboxProperty->getValue());
        }
        break;

        case PROPERTY_JITTER_MIRROR_HORIZONTAL_ENABLED:
        {
            if (!mMirrorHorizontalProperty)
                return; // The other properties are not yet created

            checkboxProperty = static_cast<Magus::QtCheckBoxProperty*>(property);
            mMirrorHorizontalProperty->setVisible(!checkboxProperty->getValue());
            mJitterMirrorHorizontalIntervalProperty->setVisible(checkboxProperty->getValue());
        }
        break;

        case PROPERTY_JITTER_MIRROR_VERTICAL_ENABLED:
        {
            if (!mMirrorVerticalProperty)
                return; // The other properties are not yet created

            checkboxProperty = static_cast<Magus::QtCheckBoxProperty*>(property);
            mMirrorVerticalProperty->setVisible(!checkboxProperty->getValue());
            mJitterMirrorVerticalIntervalProperty->setVisible(checkboxProperty->getValue());
        }
        break;
    }
}

//****************************************************************************/
void PaintLayerDialog::enableColourContainer(bool enabled)
{
    mContainerPaint->setVisible(enabled);
    mPaintColourProperty->setVisible(enabled);
    mPaintJitterCheckboxProperty->setVisible(enabled);
    mPaintJitterCheckboxProperty->setValue(enabled);
    mPaintColourJitterMinProperty->setVisible(enabled);
    mPaintColourJitterMaxProperty->setVisible(enabled);
    mPaintColourJitterIntervalProperty->setVisible(enabled);
}

//****************************************************************************/
void PaintLayerDialog::enableBurnContainer(bool enabled)
{
    mContainerBurn->setVisible(enabled);
    mBurnTextureProperty->setVisible(enabled);
}

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
#include "asset_containerwidget.h"
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
    mPaintJitterCheckboxProperty = 0;
    mPaintColourProperty = 0;
    mPaintColourJitterMinProperty = 0;
    mPaintColourJitterMaxProperty = 0;
    mPaintColourJitterIntervalProperty = 0;
    mForceProperty = 0;
    mScaleProperty = 0;

    QFile File(QString("dark.qss"));
    File.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(File.readAll());
    setStyleSheet(styleSheet);

    setWindowTitle(QString("Properties paint layer: ") + layer->name);
    QVBoxLayout* mainLayout = new QVBoxLayout;
    Magus::QtAssetWidget* assetWidget = new Magus::QtAssetWidget(QString(""), "", this);
    connect(assetWidget, SIGNAL(valueChanged(QtProperty*)), this, SLOT(propertyValueChanged(QtProperty*)));
    assetWidget->setHeaderHidden(true); // Don't use a header

    // Create container - General
    Magus::QtContainerWidget* container = 0;
    container = assetWidget->createContainer(1, QString(""));
    container->setHeaderHidden(true);

    // ******** Texture type ********
    QStringList stringListAvailableTextures = paintLayerWidget->getAvailableTextureTypes();
    mTextureTypeSelectProperty = static_cast<Magus::QtSelectProperty*>
            (assetWidget->createProperty(1, PROPERTY_TEXTURE_TYPE, QString("Texture type"), Magus::QtProperty::SELECT));
    mTextureTypeSelectProperty->addValues(stringListAvailableTextures);
    mTextureTypeSelectProperty->setCurentIndex(-1);

    // ******** Paint effect ********
    QStringList stringListPaintEffects;
    stringListPaintEffects << PAINT_EFFECT_COLOR_QSTRING <<
                              PAINT_EFFECT_ALPHA_QSTRING <<
                              PAINT_EFFECT_TEXTURE_QSTRING;
    mPaintEffectSelectProperty = static_cast<Magus::QtSelectProperty*>
            (assetWidget->createProperty(1, PROPERTY_PAINT_EFFECT, QString("Paint effect"), Magus::QtProperty::SELECT));
    mPaintEffectSelectProperty->addValues(stringListPaintEffects);

    // ******** Paint overflow ********
    QStringList stringListPaintOverflow;
    stringListPaintOverflow << PAINT_OVERFLOW_IGNORE_QSTRING <<
                               PAINT_OVERFLOW_CONTINUE_QSTRING;
    mPaintOverflowSelectProperty = static_cast<Magus::QtSelectProperty*>
            (assetWidget->createProperty(1, PROPERTY_PAINT_OVERFLOW, QString("Overflow"), Magus::QtProperty::SELECT));
    mPaintOverflowSelectProperty->addValues(stringListPaintOverflow);
    mPaintOverflowSelectProperty->setCurentIndex(1); // Continue is the default

    // ******** Jitter paint colour checkbox ********
    mPaintJitterCheckboxProperty = static_cast<Magus::QtCheckBoxProperty*>
            (assetWidget->createProperty(1, PROPERTY_JITTER_PAINT_COLOUR, QString("Jitter colour"), Magus::QtProperty::CHECKBOX));
    mPaintJitterCheckboxProperty->setValue(false);

    // ******** Paint colour ********
    mPaintColourProperty = static_cast<Magus::QtColorProperty*>
            (assetWidget->createProperty(1, PROPERTY_PAINT_COLOUR, QString("Paint colour"), Magus::QtProperty::COLOR));

    // ******** Jitter paint colour (min/max) ********
    mPaintColourJitterMinProperty = static_cast<Magus::QtColorProperty*>
            (assetWidget->createProperty(1, PROPERTY_JITTER_PAINT_COLOUR_MIN, QString("Jitter colour min"), Magus::QtProperty::COLOR));
    mPaintColourJitterMinProperty->setVisible(false);

    mPaintColourJitterMaxProperty = static_cast<Magus::QtColorProperty*>
            (assetWidget->createProperty(1, PROPERTY_JITTER_PAINT_COLOUR_MAX, QString("Jitter colour max"), Magus::QtProperty::COLOR));
    mPaintColourJitterMaxProperty->setVisible(false);

    // ******** Jitter paint colour interval (seconds) ********
    mPaintColourJitterIntervalProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(1, PROPERTY_JITTER_PAINT_COLOUR_INTERVAL, QString("Jitter colour interval (sec.)"), Magus::QtProperty::SLIDER_DECIMAL));
    mPaintColourJitterIntervalProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mPaintColourJitterIntervalProperty->setValue(0.0f);
    mPaintColourJitterIntervalProperty->setVisible(false);

    // ******** Jitter scale checkbox ********
    // TODO: 8

    // ******** Scale ********
    mScaleProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(1, PROPERTY_SCALE, QString("Brush scale"), Magus::QtProperty::SLIDER_DECIMAL));
    mScaleProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mScaleProperty->setValue(0.1f);

    // ******** Jitter scale (min/max) ********
    // TODO: 9

    // ******** Jitter scale interval (seconds) ********
    // TODO: 10

    // ******** Jitter force checkbox  ********
    // TODO

    // ******** Force ********
    mForceProperty = static_cast<Magus::QtSliderDecimalProperty*>
            (assetWidget->createProperty(1, PROPERTY_FORCE, QString("Brush force"), Magus::QtProperty::SLIDER_DECIMAL));
    mForceProperty->setSliderRange (0.0f, 1.0f, 0.005f);
    mForceProperty->setValue(1.0f);

    // ******** Jitter force (min/max) ********
    // TODO

    // ******** Jitter force interval (seconds) ********
    // TODO

    // ******** Jitter rotation angle checkbox  ********
    // TODO

    // ******** Rotation angle ********
    // TODO

    // ******** Jitter rotation angle (min/max) ********
    // TODO

    // ******** Jitter rotation angle interval (seconds) ********
    // TODO

    // ******** Jitter translation checkbox  ********
    // TODO

    // ******** Translation ********
    // TODO

    // ******** Jitter translation (min/max X, min.max Y) ********
    // TODO

    // ******** Jitter translation interval (seconds) ********
    // TODO

    // ******** Jitter mirror horizontal checkbox  ********
    // TODO

    // ******** Mirror horizontal ********
    // TODO

    // ******** Jitter mirror horizontal (alternating) ********
    // TODO

    // ******** Jitter mirror horizontal interval (seconds) ********
    // TODO

    // ******** Jitter mirror vertical checkbox  ********
    // TODO

    // ******** Mirror vertical ********
    // TODO

    // ******** Jitter mirror vertical (alternating) ********
    // TODO

    // ******** Jitter mirror vertical interval (seconds) ********
    // TODO

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
        case PROPERTY_JITTER_PAINT_COLOUR:
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
                mPaintJitterCheckboxProperty->setVisible(true);
                bool jitter = mPaintJitterCheckboxProperty->getValue();
                mPaintColourProperty->setVisible(!jitter);
                mPaintColourJitterMinProperty->setVisible(jitter);
                mPaintColourJitterMaxProperty->setVisible(jitter);
                mPaintColourJitterIntervalProperty->setVisible(jitter);
            }
            else
            {
                mPaintColourProperty->setVisible(false);
                mPaintJitterCheckboxProperty->setVisible(false);
                mPaintJitterCheckboxProperty->setValue(false);
                mPaintColourJitterMinProperty->setVisible(false);
                mPaintColourJitterMaxProperty->setVisible(false);
                mPaintColourJitterIntervalProperty->setVisible(false);
            }
        }
        break;
    }
}

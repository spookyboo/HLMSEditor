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

#ifndef PAINTLAYER_DIALOG_H
#define PAINTLAYER_DIALOG_H

#include <QDialog>
#include "asset_propertywidget_select.h"
#include "asset_propertywidget_color.h"
#include "asset_propertywidget_slider_decimal.h"
#include "asset_propertywidget_checkbox.h"
#include "asset_containerwidget.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

using namespace Magus;
class PaintLayerWidget;
struct QtLayer;
class PaintLayerDialog : public QDialog
{
    Q_OBJECT

    public:
        PaintLayerDialog(PaintLayerWidget* parent, QtLayer* layer);
        ~PaintLayerDialog(void);

        // Make the properties public for convenience
        QtSelectProperty* mTextureTypeSelectProperty;
        QtSelectProperty* mPaintEffectSelectProperty;
        QtSelectProperty* mPaintOverflowSelectProperty;

        QtColorProperty* mPaintColourProperty;
        QtCheckBoxProperty* mPaintJitterCheckboxProperty;
        QtColorProperty* mPaintColourJitterMinProperty;
        QtColorProperty* mPaintColourJitterMaxProperty;
        QtSliderDecimalProperty* mPaintColourJitterIntervalProperty;

        QtSliderDecimalProperty* mForceProperty;
        QtCheckBoxProperty* mJitterForceCheckboxProperty;
        QtSliderDecimalProperty* mJitterForceMinProperty;
        QtSliderDecimalProperty* mJitterForceMaxProperty;
        QtSliderDecimalProperty* mJitterForceIntervalProperty;

        QtSliderDecimalProperty* mScaleProperty;
        QtCheckBoxProperty* mJitterScaleCheckboxProperty;
        QtSliderDecimalProperty* mJitterScaleMinProperty;
        QtSliderDecimalProperty* mJitterScaleMaxProperty;
        QtSliderDecimalProperty* mJitterScaleIntervalProperty;

        QtSliderDecimalProperty* mRotationAngleProperty;
        QtCheckBoxProperty* mJitterRotationAngleCheckboxProperty;
        QtSliderDecimalProperty* mJitterRotationAngleMinProperty;
        QtSliderDecimalProperty* mJitterRotationAngleMaxProperty;
        QtSliderDecimalProperty* mJitterRotationAngleIntervalProperty;

        QtCheckBoxProperty* mJitterTranslationCheckboxProperty;
        QtSliderDecimalProperty* mTranslationXProperty;
        QtSliderDecimalProperty* mTranslationYProperty;
        QtSliderDecimalProperty* mJitterTranslationXMinProperty;
        QtSliderDecimalProperty* mJitterTranslationXMaxProperty;
        QtSliderDecimalProperty* mJitterTranslationYMinProperty;
        QtSliderDecimalProperty* mJitterTranslationYMaxProperty;
        QtSliderDecimalProperty* mJitterTranslationIntervalProperty;

        QtCheckBoxProperty* mMirrorHorizontalProperty;
        QtCheckBoxProperty* mJitterMirrorHorizontalProperty;
        QtSliderDecimalProperty* mJitterMirrorHorizontalIntervalProperty;

        QtCheckBoxProperty* mMirrorVerticalProperty;
        QtCheckBoxProperty* mJitterMirrorVerticalProperty;
        QtSliderDecimalProperty* mJitterMirrorVerticalIntervalProperty;

    private slots:
        void propertyValueChanged(QtProperty* property);

    private:
        void createIcons();
        PaintLayerWidget* mPaintLayerWidget;
        QtLayer* mQtLayer;
        Magus::QtContainerWidget* mContainerGeneral;
        Magus::QtContainerWidget* mContainerPaint;
        Magus::QtContainerWidget* mContainerScale;
        Magus::QtContainerWidget* mContainerForce;
        Magus::QtContainerWidget* mContainerRotationAngle;
        Magus::QtContainerWidget* mContainerTranslation;
        Magus::QtContainerWidget* mContainerMirrorHorizontal;
        Magus::QtContainerWidget* mContainerMirrorVertical;
};

#endif

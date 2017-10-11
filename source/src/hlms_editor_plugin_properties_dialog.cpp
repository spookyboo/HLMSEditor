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

#include <QtWidgets>

#include "hlms_editor_plugin_properties_dialog.h"
#include "mainwindow.h"

//****************************************************************************/
PluginPropertiesDialog::PluginPropertiesDialog (MainWindow* parent) : QDialog(parent)
{
    setWindowTitle("Plugin properties");
    setMinimumSize(600, 800);
    mMainLayout = new QVBoxLayout;
    mPropertiesLayout = new QVBoxLayout;

    // Create a buttonbox
    mButtonBoxLayout = new QHBoxLayout;
    mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    mButtonBoxLayout->addWidget(mButtonBox);
    mMainLayout->addLayout(mPropertiesLayout, 1000);
    mMainLayout->addLayout(mButtonBoxLayout, 1);
    connect(mButtonBox, SIGNAL(accepted()), this, SLOT(handleOkAndAccept()));
    setLayout(mMainLayout);
}

//****************************************************************************/
void PluginPropertiesDialog::addProperty (Ogre::HlmsEditorPluginData::PLUGIN_PROPERTY property)
{
    QLabel* label = new QLabel();
    label->setText(property.labelName.c_str());
    QHBoxLayout* propertyLayout = new QHBoxLayout;
    propertyLayout->addWidget(label, 1000);
    mPropertiesLayout->addLayout(propertyLayout);
    PropertyAndWidgetStruct strct;
    strct.property = property;
    switch (property.type)
    {
        case Ogre::HlmsEditorPluginData::BOOL:
        {
            QCheckBox* checkBox = new QCheckBox();
            checkBox->setChecked(property.boolValue);
            propertyLayout->addWidget(checkBox, 1);
            strct.widget = checkBox;
            mPropertyAndWidgetMap[property.propertyName] = strct;
        }
        break;
        case Ogre::HlmsEditorPluginData::STRING:
        {
            QLineEdit* edit = new QLineEdit();
            edit->setText(property.stringValue.c_str());
            propertyLayout->addWidget(edit, 1000);
            strct.widget = edit;
            mPropertyAndWidgetMap[property.propertyName] = strct;
        }
        break;
        case Ogre::HlmsEditorPluginData::FLOAT:
        {
            QLineEdit* edit = new QLineEdit();
            QRegExp mRegularExpression = QRegExp("[+-]?([0-9]+\\.([0-9]+)?|\\.[0-9]+)([eE][+-]?[0-9]+)?"); // floating point
            QRegExpValidator* validator = new QRegExpValidator(mRegularExpression);
            edit->setValidator(validator);
            QString s = QString::number(property.floatValue, 'f', 5);
            edit->setText(s);
            propertyLayout->addWidget(edit, 1000);
            strct.widget = edit;
            mPropertyAndWidgetMap[property.propertyName] = strct;
        }
        default:
        {
            QLineEdit* edit = new QLineEdit();
            QRegExp mRegularExpression = QRegExp("[+-]?([0-9]+\\.([0-9]+)?|\\.[0-9]+)([eE][+-]?[0-9]+)?");
            QRegExpValidator* validator = new QRegExpValidator(mRegularExpression);
            edit->setValidator(validator);
            QString s = QString::number(property.shortValue);
            edit->setText(s);
            propertyLayout->addWidget(edit, 1000);
            strct.widget = edit;
            mPropertyAndWidgetMap[property.propertyName] = strct;
        }
    }
}

//****************************************************************************/
std::map<std::string, Ogre::HlmsEditorPluginData::PLUGIN_PROPERTY> PluginPropertiesDialog::getProperties (void)
{
    // Get the values from the widgets
    std::map<std::string, PropertyAndWidgetStruct>::iterator it;
    std::map<std::string, PropertyAndWidgetStruct>::iterator itEnd = mPropertyAndWidgetMap.end();
    mProperties.clear();
    Ogre::HlmsEditorPluginData::PLUGIN_PROPERTY property;
    PropertyAndWidgetStruct strct;
    for (it = mPropertyAndWidgetMap.begin(); it != itEnd; ++it)
    {
        strct = it-> second;
        property = strct.property;

        switch (property.type)
        {
            case Ogre::HlmsEditorPluginData::BOOL:
            {
                QCheckBox* checkBox = static_cast<QCheckBox*>(strct.widget);
                property.boolValue = checkBox->isChecked();
            }
            break;
            case Ogre::HlmsEditorPluginData::STRING:
            {
                QLineEdit* edit = static_cast<QLineEdit*>(strct.widget);
                property.stringValue = edit->text().toStdString();
            }
            break;
            case Ogre::HlmsEditorPluginData::FLOAT:
            {
                QLineEdit* edit = static_cast<QLineEdit*>(strct.widget);
                QString s = edit->text();
                double d = s.toDouble();
                property.floatValue = d;
            }
            break;
            case Ogre::HlmsEditorPluginData::UNSIGNED_SHORT:
            {
                QLineEdit* edit = static_cast<QLineEdit*>(strct.widget);
                QString s = edit->text();
                unsigned short sv = s.toShort();
                property.shortValue = sv;
            }
            break;
            case Ogre::HlmsEditorPluginData::INT:
            {
                QLineEdit* edit = static_cast<QLineEdit*>(strct.widget);
                QString s = edit->text();
                int i = s.toInt();
                property.intValue = i;
            }
            break;
        }

        mProperties[property.propertyName] = property;
    }

    return mProperties;
}

//****************************************************************************/
void PluginPropertiesDialog::handleOkAndAccept(void)
{
    accept();
}

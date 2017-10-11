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

#ifndef PLUGIN_PROPERTIES_DIALOG_H
#define PLUGIN_PROPERTIES_DIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QMap>
#include "hlms_editor_plugin.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class MainWindow;
class PluginPropertiesDialog : public QDialog
{
    Q_OBJECT

    public:
        struct PropertyAndWidgetStruct
        {
            Ogre::HlmsEditorPluginData::PLUGIN_PROPERTY property;
            QWidget* widget;
        };

        PluginPropertiesDialog (MainWindow* parent);
        void addProperty (Ogre::HlmsEditorPluginData::PLUGIN_PROPERTY property);
        std::map<std::string, Ogre::HlmsEditorPluginData::PLUGIN_PROPERTY> getProperties (void);

    public slots:
        void handleOkAndAccept(void);

    private:
        QVBoxLayout* mMainLayout;
        QVBoxLayout* mPropertiesLayout;
        QDialogButtonBox* mButtonBox;
        QHBoxLayout* mButtonBoxLayout;
        std::map<std::string, PropertyAndWidgetStruct> mPropertyAndWidgetMap;
        std::map<std::string, Ogre::HlmsEditorPluginData::PLUGIN_PROPERTY> mProperties;
};

#endif

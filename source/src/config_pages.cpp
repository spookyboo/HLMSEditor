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
#include <QMessageBox>
#include "constants.h"
#include "config_pages.h"

//****************************************************************************/
GeneralPage::GeneralPage(QWidget *parent)
    : QWidget(parent)
{
    // First load the settings
    loadSettings();

    // Paths
    QGroupBox* pathGroup = new QGroupBox(tr("Paths"));
    QLabel* importLabel = new QLabel(tr("Import dir:"));
    mImportEdit = new QLineEdit;
    mImportEdit->setText(mImportPath);
    mImportEdit->setEnabled(false);
    QPushButton* importButton = new QPushButton(tr(".."));
    connect(importButton, SIGNAL(clicked(bool)), this, SLOT(doSetImportDir(void)));
    QGridLayout* pathLayout = new QGridLayout;
    pathLayout->addWidget(importLabel, 0, 0);
    pathLayout->addWidget(mImportEdit, 0, 1);
    pathLayout->addWidget(importButton, 0, 2);
    pathGroup->setLayout(pathLayout);

    // Restore
    QGroupBox* restoreGroup = new QGroupBox(tr("Restore"));
    QPushButton* restoreButton = new QPushButton(tr("Restore all settings"));
    connect(restoreButton, SIGNAL(clicked(bool)), this, SLOT(doResetAllSettings(void)));
    QHBoxLayout *restoreLayout = new QHBoxLayout;
    restoreLayout->addStretch(2);
    restoreLayout->addWidget(restoreButton);
    restoreLayout->addStretch(2);
    restoreGroup->setLayout(restoreLayout);

    // Set layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(pathGroup);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(restoreGroup);
    mainLayout->addStretch(1);
    setLayout(mainLayout);
}

//****************************************************************************/
void GeneralPage::doSetImportDir (void)
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    if (dialog.exec())
    {
        QStringList fileNames = dialog.selectedFiles();
        mImportPath = fileNames.at(0) + "/";
        mImportEdit->setText(mImportPath);
        saveSettings();
        displaySettingsChangedMessage();
    }
}

//****************************************************************************/
void GeneralPage::doResetAllSettings (void)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Warning", "Are you sure you want to restore all settings?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        QFile::remove(FILE_RECENT_HLMS_FILES);
        QFile::remove(FILE_RECENT_PROJECT_FILES);

        // Copy resources_default.cfg to resources.cfg
        QFile defaultResources(FILE_RESOURCES_DEFAULT);
        if (defaultResources.exists())
        {
            QFile::remove(FILE_RESOURCES);
            defaultResources.copy(FILE_RESOURCES);
        }

        // Copy settings_default.cfg to settings.cfg
        QFile defaultSettings(FILE_SETTINGS_DEFAULT);
        if (defaultSettings.exists())
        {
            QFile::remove(FILE_SETTINGS);
            defaultSettings.copy(FILE_SETTINGS);
        }

        QMessageBox::information(0, QString("Info"), QString("The restored settings are active after the application has been restarted"));
    }
}

//****************************************************************************/
void GeneralPage::loadSettings(void)
{
    mImportPath = "";
    QSettings settings(FILE_SETTINGS, QSettings::IniFormat);
    mImportPath = settings.value(SETTINGS_IMPORT_PATH).toString();
    if (mImportPath.isEmpty())
        mImportPath = DEFAULT_IMPORT_PATH;
}

//****************************************************************************/
void GeneralPage::saveSettings(void)
{
    QFile file(FILE_SETTINGS);
    if (file.open(QFile::WriteOnly|QFile::Truncate))
    {
        QTextStream stream(&file);
        stream << SETTINGS_IMPORT_PATH
               << " = "
               << "\""
               << mImportPath
               << "\""
               << "\n";
        file.close();
    }
}


//****************************************************************************/
void GeneralPage::displaySettingsChangedMessage(void)
{
    QMessageBox::information(0, QString("Info"), QString("The changed settings are active after the application has been restarted"));
}

//****************************************************************************/
//****************************************************************************/
//****************************************************************************/
HlmsPage::HlmsPage(QWidget *parent)
    : QWidget(parent)
{
}

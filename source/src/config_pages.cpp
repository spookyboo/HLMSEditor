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
#include <QTableWidgetItem>
#include <QMessageBox>
#include "constants.h"
#include "hlms_builder.h"
#include "config_pages.h"

//****************************************************************************/
GeneralPage::GeneralPage(ConfigDialog *parent)
    : QWidget(parent),
      mParent(parent),
      mSkyboxTableWidgetSignalsEnabled(false)
{
    // Paths
    QGroupBox* pathGroup = new QGroupBox(tr("Paths"));
    QLabel* importLabel = new QLabel(tr("Import dir:"));
    mImportEdit = new QLineEdit;
    mImportEdit->setText(mParent->mImportPath);
    mImportEdit->setEnabled(false);
    QPushButton* importButton = new QPushButton(tr(".."));
    connect(importButton, SIGNAL(clicked(bool)), this, SLOT(doSetImportDir(void)));
    QGridLayout* pathLayout = new QGridLayout;
    pathLayout->addWidget(importLabel, 0, 0);
    pathLayout->addWidget(mImportEdit, 0, 1);
    pathLayout->addWidget(importButton, 0, 2);
    pathGroup->setLayout(pathLayout);

    // Skybox table
    QGroupBox* skyboxGroup = new QGroupBox(tr("Skybox"));
    mSkyboxTableWidget = new QTableWidget(1, 2, this);
    mSkyboxTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mSkyboxTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mSkyboxTableWidget->setAcceptDrops(false);
    mSkyboxTableWidget->setShowGrid(false);
    mSkyboxTableWidget->viewport()->installEventFilter(this);
    mSkyboxTableWidget->setColumnWidth(0, 80);
    mSkyboxTableWidget->setColumnWidth(1, 256);
    mSkyboxTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    mSkyboxTableWidget->verticalHeader()->hide();
    setStyleSheet("QLineEdit{padding: 0 0px; margin-left: 0px; margin-right: 0px; max-height: 28px; height: 28px;}");
    connect(mSkyboxTableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(doSkyBoxCellChanged(int,int)));

    // Set headers skybox table
    QStringList headers;
    headers << tr("Enabled") << tr("Skybox cubemap");
    mSkyboxTableWidget->setHorizontalHeaderLabels(headers);
    QFont font;
    font.setBold(true);
    mSkyboxTableWidget->horizontalHeader()->setFont(font);
    mSkyboxTableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    QVBoxLayout* skyboxLayout = new QVBoxLayout;
    skyboxLayout->addWidget(mSkyboxTableWidget);

    // Add new skybox
    QLabel* skyboxLabel = new QLabel(tr("Add skybox:"));
    mLoadSkyboxEdit = new QLineEdit;
    mLoadSkyboxEdit->setEnabled(false);
    QPushButton* loadSkyboxButton = new QPushButton(tr(".."));
    connect(loadSkyboxButton, SIGNAL(clicked(bool)), this, SLOT(doLoadSkyboxFile(void)));
    QGridLayout* skyboxNewLayout = new QGridLayout;
    skyboxNewLayout->addWidget(skyboxLabel, 0, 0);
    skyboxNewLayout->addWidget(mLoadSkyboxEdit, 0, 1);
    skyboxNewLayout->addWidget(loadSkyboxButton, 0, 2);
    skyboxLayout->addLayout(skyboxNewLayout);
    skyboxGroup->setLayout(skyboxLayout);

    // Restore
    QGroupBox* restoreGroup = new QGroupBox(tr("Restore"));
    QPushButton* restoreButton = new QPushButton(tr("Restore all settings"));
    connect(restoreButton, SIGNAL(clicked(bool)), this, SLOT(doResetAllSettings(void)));
    QHBoxLayout* restoreLayout = new QHBoxLayout;
    restoreLayout->addStretch(2);
    restoreLayout->addWidget(restoreButton);
    restoreLayout->addStretch(2);
    restoreGroup->setLayout(restoreLayout);

    // Set layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(pathGroup);
    mainLayout->addSpacing(12);
    mainLayout->addWidget(skyboxGroup);
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
        mParent->mImportPath = fileNames.at(0) + "/";
        mImportEdit->setText(mParent->mImportPath);
        mParent->saveSettings();
        //QMessageBox::information(0, QString("Info"), QString("The import path is immediately active"));
    }
}

//****************************************************************************/
void GeneralPage::doLoadSkyboxFile (void)
{
    QString fileName = QFileDialog::getOpenFileName(this, QString("Add cubemap skybox"),
                                                    QString(""),
                                                    QString("DDS (*.dds);;"
                                                            "All files (*.*)"));

    if (!fileName.isEmpty())
    {
        QFileInfo fileInfo(fileName);
        QTableWidgetItem* item1 = new QTableWidgetItem();
        QTableWidgetItem* item2 = new QTableWidgetItem();
        unsigned int row = mSkyboxTableWidget->rowCount();
        mSkyboxTableWidget->insertRow(row-1);
        item1->setCheckState(Qt::Checked);
        item2->setText(fileInfo.fileName());
        mSkyboxTableWidget->setItem(row-1, 0, item1);
        mSkyboxTableWidget->setItem(row-1, 1, item2);
        mParent->saveSettings();

        // Add the location of the cubemap also to the resourcelocations and save the config file
        HlmsBuilder builder;
        Ogre::Root::getSingletonPtr()->addResourceLocation(fileInfo.path().toStdString(),
                                                           "FileSystem",
                                                           Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        builder.saveAllResourcesLocations();
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
void GeneralPage::setSkyBoxMap (QMap<QString, QString> skyBoxMap)
{
    QMap<QString, QString>::iterator it = skyBoxMap.begin();
    QMap<QString, QString>::iterator itEnd = skyBoxMap.end();
    QString key;
    QString value;
    QTableWidgetItem* item1;
    QTableWidgetItem* item2;
    unsigned int row = 0;
    mSkyboxTableWidgetSignalsEnabled = false;
    while (it != itEnd)
    {
        value = it.value();
        item1 = new QTableWidgetItem();
        item2 = new QTableWidgetItem();
        mSkyboxTableWidget->insertRow(row);
        item1->setCheckState(Qt::Checked);
        item2->setText(value);
        mSkyboxTableWidget->setItem(row, 0, item1);
        mSkyboxTableWidget->setItem(row, 1, item2);
        ++it;
        ++row;
    }
    mSkyboxTableWidgetSignalsEnabled = true;
}

//****************************************************************************/
void GeneralPage::doSkyBoxCellChanged(int row,int col)
{
    if (!mSkyboxTableWidgetSignalsEnabled)
        return;

    QTableWidgetItem* item1 = mSkyboxTableWidget->item(row, 0);
    QTableWidgetItem* item2 = mSkyboxTableWidget->item(row, 1);
    if (!item1 || !item2)
        return;

    bool enabled = item1->checkState() == Qt::Checked;
    QString skyboxName = item2->text();
    mParent->skyBoxTableUpdated(enabled, skyboxName);
    mParent->saveSettings();
    QMessageBox::information(0, QString("Info"), QString("The settings are active after the application has been restarted"));
}

//****************************************************************************/
//****************************************************************************/
//****************************************************************************/
HlmsPage::HlmsPage(ConfigDialog *parent)
    : QWidget(parent),
      mParent(parent)
{
    // PBS
    QGroupBox* pbsGroup = new QGroupBox(tr("PBS"));
    QHBoxLayout* pbsLayout = new QHBoxLayout;
    // TODO: Add widgets to the pbsLayout
    pbsGroup->setLayout(pbsLayout);

    // Unlit
    QGroupBox* unlitGroup = new QGroupBox(tr("Unlit"));
    QHBoxLayout* unlitLayout = new QHBoxLayout;
    // TODO: Add widgets to the unlitLayout
    unlitGroup->setLayout(unlitLayout);

    // Texture/Samplerblock
    QGroupBox* samplerBlockGroup = new QGroupBox(tr("Texture/Samplerblock"));
    QVBoxLayout* samplerBlockLayout = new QVBoxLayout;

    // Filter
    QHBoxLayout* filterLayout = new QHBoxLayout();
    QLabel* filterLabel = new QLabel(tr("Default Min, Mag, Mip Filter"));
    mFilterList = new QComboBox();
    QStringList stringListTextureFilterOptions;
    stringListTextureFilterOptions << QString("No filtering")
                                   << QString("Point")
                                   << QString("Linear")
                                   << QString("Anisotropic");
    QStringListModel* model = new QStringListModel(stringListTextureFilterOptions);
    mFilterList->setModel(model);
    mFilterList->setMaxVisibleItems(4);
    mFilterList->setCurrentIndex(mParent->mSamplerblockFilterIndex);
    filterLayout->addWidget(filterLabel, 1);
    filterLayout->addWidget(mFilterList, 2);
    connect(mFilterList, SIGNAL(currentIndexChanged(QString)), this, SLOT(doFilterChanged(void)));

    // Layout
    samplerBlockLayout->addLayout(filterLayout);
    samplerBlockLayout->addStretch(1);
    samplerBlockGroup->setLayout(samplerBlockLayout);

    // Set layout
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(pbsGroup);
    mainLayout->addWidget(unlitGroup);
    mainLayout->addWidget(samplerBlockGroup);
    setLayout(mainLayout);
}

//****************************************************************************/
void HlmsPage::doFilterChanged(void)
{
    mParent->mSamplerblockFilterIndex = mFilterList->currentIndex();
    mParent->saveSettings();
    //QMessageBox::information(0, QString("Info"), QString("The filter settings are immediately active"));
}

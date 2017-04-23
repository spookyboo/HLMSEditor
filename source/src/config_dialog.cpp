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

#include "config_dialog.h"
#include "mainwindow.h"
#include "config_pages.h"

//****************************************************************************/
ConfigDialog::ConfigDialog(MainWindow* parent) : QDialog(parent)
{
    loadSettings();
    contentsWidget = new QListWidget;
    contentsWidget->setViewMode(QListView::IconMode);
    contentsWidget->setIconSize(QSize(96, 84));
    contentsWidget->setMovement(QListView::Static);
    contentsWidget->setMaximumWidth(128);
    contentsWidget->setSpacing(12);

    // Pages
    pagesWidget = new QStackedWidget;
    mGeneralPage = new GeneralPage(this);
    mGeneralPage->setSkyBoxMap(mSkyBoxMap);
    mHlmsPage = new HlmsPage(this);
    pagesWidget->addWidget(mGeneralPage);
    pagesWidget->addWidget(mHlmsPage);

    createIcons();
    contentsWidget->setCurrentRow(0);

    // Close button
    QPushButton* closeButton = new QPushButton(tr("Close"));
    connect(closeButton, &QAbstractButton::clicked, this, &QWidget::close);

    QHBoxLayout *horizontalLayout = new QHBoxLayout;
    horizontalLayout->addWidget(contentsWidget);
    horizontalLayout->addWidget(pagesWidget);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch(1);
    buttonsLayout->addWidget(closeButton);

    setMinimumWidth(600);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(horizontalLayout);
    mainLayout->addSpacing(12);
    mainLayout->addLayout(buttonsLayout);
    setLayout(mainLayout);
    setWindowTitle(tr("Config Dialog"));
    //showMaximized();
    show();
}

//****************************************************************************/
void ConfigDialog::createIcons()
{
    QListWidgetItem *generalButton = new QListWidgetItem(contentsWidget);
    generalButton->setIcon(QIcon(ICON_GENERATE));
    generalButton->setText(tr("General"));
    generalButton->setTextAlignment(Qt::AlignHCenter);
    generalButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    QListWidgetItem *hlmsButton = new QListWidgetItem(contentsWidget);
    hlmsButton->setIcon(QIcon(ICON_HLMS));
    hlmsButton->setText(tr("Hlms"));
    hlmsButton->setTextAlignment(Qt::AlignHCenter);
    hlmsButton->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    connect(contentsWidget,
            SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

//****************************************************************************/
void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (!current)
        current = previous;

    pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

//****************************************************************************/
void ConfigDialog::okAndAccept(void)
{
    accept();
}

//****************************************************************************/
void ConfigDialog::loadSettings(void)
{
    mImportPath = "";
    QSettings settings(FILE_SETTINGS, QSettings::IniFormat);
    mImportPath = settings.value(SETTINGS_IMPORT_PATH).toString();
    if (mImportPath.isEmpty())
        mImportPath = DEFAULT_IMPORT_PATH;

    mSamplerblockFilterIndex = settings.value(SETTINGS_SAMPLERBLOCK_FILTER_INDEX).toInt();
    mNumberOfSkyBoxes = settings.value(SETTINGS_NUMBER_OF_SKYBOXES).toInt();

    // Load the skybox names
    QString key;
    QString value;
    for (unsigned int i = 0; i < mNumberOfSkyBoxes; ++i)
    {
        key = SETTINGS_PREFIX_SKYBOX + QVariant(i).toString();
        value = settings.value(key).toString();
        mSkyBoxMap[key] = value;
    }
}

//****************************************************************************/
void ConfigDialog::saveSettings(void)
{
    QString key;
    QString value;
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
        stream << SETTINGS_SAMPLERBLOCK_FILTER_INDEX
               << " = "
               << "\""
               << mSamplerblockFilterIndex
               << "\""
               << "\n";
        stream << SETTINGS_NUMBER_OF_SKYBOXES
               << " = "
               << "\""
               << mNumberOfSkyBoxes
               << "\""
               << "\n";

        // Save the skybox names
        QMap<QString, QString>::iterator it = mSkyBoxMap.begin();
        QMap<QString, QString>::iterator itEnd = mSkyBoxMap.end();
        while (it != itEnd)
        {
            key = it.key();
            value = it.value();
            stream << key
                   << " = "
                   << "\""
                   << value
                   << "\""
                   << "\n";
            ++it;
        }

        file.close();
    }
}

//****************************************************************************/
void ConfigDialog::skyBoxTableUpdated(bool enabled, QString skyboxName)
{
    // Rebuild the map
    QMap<QString, QString> newMap;
    QMap<QString, QString>::iterator it = mSkyBoxMap.begin();
    QMap<QString, QString>::iterator itEnd = mSkyBoxMap.end();
    unsigned int number = 0;
    QString key;
    QString value;
    bool found = false;
    while (it != itEnd)
    {
        key = tr("skybox") + QVariant(number).toString();
        value = it.value();
        if (skyboxName == value)
        {
            found = true;
            if (enabled)
            {
                newMap[key] = value;
                ++number;
            }
            // If not enabled, skip it
        }
        else
        {
            newMap[key] = value;
            ++number;
        }
        ++it;
    }

    // The skyboxName was not found, but it is enabled, so it has to be added to the map
    // This is at the end of the map, so the order of the map and the table of the config page are not in sync
    if (!found && enabled)
    {
        key = tr("skybox") + QVariant(number).toString();
        newMap[key] = skyboxName;
    }

    mSkyBoxMap.clear();
    mSkyBoxMap = newMap;
    mNumberOfSkyBoxes = mSkyBoxMap.size();
}

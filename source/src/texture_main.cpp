/****************************************************************************
**
** Copyright (C) 2016 - 2017
**
** This file is generated by the Magus toolkit
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF METABILITY AND FITNESS FOR
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

// Include
#include <QString>
#include <QFile>
#include <QMenuBar>
#include "magus_core.h"
#include "texture_main.h"
#include "texture_thumbs.h"
#include "constants.h"

//****************************************************************************/
TextureMain::TextureMain(const QString& iconDir, QWidget* parent) : QMainWindow(parent), mIsClosing(false)
{
    mIconDir = iconDir;
    mSelectedFileName = "";

    // Perform standard functions
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDockWindows();
    showMaximized();
}

//****************************************************************************/
TextureMain::~TextureMain(void)
{
}

//****************************************************************************/
QVector<Magus::QtResourceInfo*>& TextureMain::getResources (void)
{
    // Delegate to mTextureTreeDockWidget
    return mTextureTreeDockWidget->getResources();
}

//****************************************************************************/
void TextureMain::setResources (const QVector<Magus::QtResourceInfo*>& resources)
{
    // Delegate to mTextureThumbsDockWidget
    mTextureThumbsDockWidget->setResources(resources);

    // Delegate to mTextureTreeDockWidget
    mTextureTreeDockWidget->setResources(resources);
    mSelectedFileName = "";
}

//****************************************************************************/
void TextureMain::closeEvent(QCloseEvent* event)
{
    mIsClosing = true;
}

//****************************************************************************/
void TextureMain::createActions(void)
{
}

//****************************************************************************/
void TextureMain::createMenus(void)
{
}

//****************************************************************************/
void TextureMain::createToolBars(void)
{
}

//****************************************************************************/
void TextureMain::createStatusBar(void)
{
}

//****************************************************************************/
void TextureMain::createDockWindows(void)
{
    // Textures (tree)
    mTextureTreeDockWidget = new TextureTreeDockWidget(mIconDir, QString(""), this);
    connect(mTextureTreeDockWidget, SIGNAL(resourceSelected(int,int,int,QString,QString,int)), this, SLOT(handleTextureSelected(int,int,int,const QString&,const QString&, int)));
    connect(mTextureTreeDockWidget, SIGNAL(resourceDoubleClicked(int,int,int,QString,QString)), this, SLOT(handleTextureDoubleClicked(int,int,int,const QString&,const QString&)));
    connect(mTextureTreeDockWidget, SIGNAL(resourceDeleted(int,int,int,QString,QString)), this, SLOT(handleTextureDeleted(int,int,int,QString,QString)));
    //connect(mTextureTreeDockWidget, SIGNAL(resourcesDeleted()), this, SLOT(handleTexturesDeleted()));
    connect(mTextureTreeDockWidget, SIGNAL(resourceSearched(QString)), this, SLOT(handleTextureSearched(QString)));
    connect(mTextureTreeDockWidget, SIGNAL(resourceSearchReset()), this, SLOT(handleTextureSearchReset()));
    connect(mTextureTreeDockWidget, SIGNAL(customContextMenuItemSelected(QString)), this, SLOT(handleCustomContextMenuItemSelected(QString)));
    connect(mTextureTreeDockWidget, SIGNAL(resourceAdded(int,int,int,QString,QString,int)), this, SLOT(handleTextureAdded(int,int,int,QString,QString,int)));
    connect(mTextureTreeDockWidget, SIGNAL(resourceMoved(int,int,int,QString,QString,int)), this, SLOT(handleTextureMoved(int,int,int,QString,QString,int)));
    connect(mTextureTreeDockWidget, SIGNAL(resourceChanged(int,int,int,QString,QString,int)), this, SLOT(handleTextureChanged(int,int,int,QString,QString,int)));
    addDockWidget(Qt::LeftDockWidgetArea, mTextureTreeDockWidget);

    // Thumbs
    mTextureThumbsDockWidget = new TextureThumbsDockWidget(mIconDir, QString(""), this);
    connect(mTextureThumbsDockWidget, SIGNAL(assetSelected(QString,QString)), this, SLOT(handleThumbSelected(QString,QString)));
    connect(mTextureThumbsDockWidget, SIGNAL(textureFileDropped(QString,QString)), this, SLOT(handleTextureFileDropped(QString,QString)));
    connect(mTextureThumbsDockWidget, SIGNAL(assetDoubleClicked(QString,QString)), this, SLOT(handleThumbDoubleClicked(QString,QString)));
    connect(mTextureThumbsDockWidget, SIGNAL(assetDeleted(QString,QString)), this, SLOT(handleThumbDeleted(QString,QString)));
    addDockWidget(Qt::RightDockWidgetArea, mTextureThumbsDockWidget);
}

//****************************************************************************/
void TextureMain::addTextureFile (const QString& fileName)
{
    mTextureTreeDockWidget->addTextureFile(fileName, TOOL_SOURCES_LEVEL_X000_TEXTURE);
    mTextureThumbsDockWidget->addTextureFile(fileName);
}

//****************************************************************************/
void TextureMain::addTextureFile (const QString& fileName, const QString& group)
{
    int groupId = mTextureTreeDockWidget->addGroup(group);
    mTextureTreeDockWidget->addTextureFile(fileName, groupId);
    mTextureTreeDockWidget->expand(TOOL_SOURCES_LEVEL_X000_TEXTURE);
    mTextureThumbsDockWidget->addTextureFile(fileName);
}

//****************************************************************************/
void TextureMain::deleteTexture (const QString& fileName)
{
    mTextureTreeDockWidget->deleteTextureFile(fileName);
    mTextureThumbsDockWidget->deleteTextureFile(fileName);
}

//****************************************************************************/
void TextureMain::handleTextureSelected(int toplevelId,
                                        int parentId,
                                        int resourceId,
                                        const QString& name,
                                        const QString& baseName,
                                        int resourceType)
{
    if (resourceType == Magus::TOOL_RESOURCETREE_KEY_TYPE_TOPLEVEL_GROUP)
    {
        // If the toplevel (root) group is selected, show all items
        mTextureThumbsDockWidget->resetFilter();
    }
    else if (resourceType == Magus::TOOL_RESOURCETREE_KEY_TYPE_GROUP)
    {
        // If a group is selected, only show the items in the group (transfer to QStringList)
        QVector<Magus::QtResourceInfo*> resources = mTextureTreeDockWidget->getResources(resourceId);
        QStringList names;
        foreach (Magus::QtResourceInfo* info, resources)
        {
            // TODO: Also apply any searchPattern in case the search filter was still active
            names.append(info->fullQualifiedName);
        }
        mTextureThumbsDockWidget->filter(names); // Also resets the filter
    }
    else if (resourceType == Magus::TOOL_RESOURCETREE_KEY_TYPE_ASSET)
    {
        QVector<Magus::QtResourceInfo*> resources = mTextureTreeDockWidget->getResources(parentId);
        QStringList names;
        foreach (Magus::QtResourceInfo* info, resources)
        {
            // TODO: Also apply any searchPattern in case the search filter was still active
            names.append(info->fullQualifiedName);
        }

        // Display the thumbs
        mTextureThumbsDockWidget->filter(names); // Also resets the filter

        // Set the thumb of the selected item in the tree
        QString selectedThumb = mTextureTreeDockWidget->getCurrentResourceName();
        mTextureThumbsDockWidget->setSelectThumb(selectedThumb);
    }
}

//****************************************************************************/
void TextureMain::handleTextureDoubleClicked(int toplevelId, int parentId, int resourceId, const QString& name, const QString& baseName)
{
    emit textureDoubleClicked(toplevelId, parentId, resourceId, name, baseName);
}

//****************************************************************************/
void TextureMain::handleTextureDeleted(int toplevelId, int parentId, int resourceId, const QString& name, const QString& baseName)
{
    // Delete name from mTextureThumbsDockWidget
    mTextureThumbsDockWidget->deleteTextureFile(name);
    mSelectedFileName = "";
    emit textureMutationOccured();
}

//****************************************************************************/
void TextureMain::handleTextureAdded(int toplevelId, int parentId, int resourceId, const QString& name, const QString& baseName, int resourceType)
{
    // Emit that a change in the tree occured (reason to save the current state)
    emit textureMutationOccured();
}

//****************************************************************************/
void TextureMain::handleTextureMoved(int toplevelId, int parentId, int resourceId, const QString& name, const QString& baseName, int resourceType)
{
    // Emit that a change in the tree occured (reason to save the current state)
    emit textureMutationOccured();
}

//****************************************************************************/
void TextureMain::handleTextureChanged(int toplevelId, int parentId, int resourceId, const QString& name, const QString& baseName, int resourceType)
{
    // The name of the texture is changed
    emit textureMutationOccured();
}

//****************************************************************************/
void TextureMain::handleTextureSearched(const QString& searchPattern)
{
    // Apply filtering to mTextureThumbsDockWidget
    mTextureThumbsDockWidget->filter(searchPattern);
    mSelectedFileName = "";
}

//****************************************************************************/
void TextureMain::handleTextureSearchReset(void)
{
    // Reset the filtering in mTextureThumbsDockWidget
    mTextureThumbsDockWidget->resetFilter();
    mSelectedFileName = "";
}

//****************************************************************************/
void TextureMain::handleThumbDeleted(const QString& name, const QString& baseName)
{
    // Note, that in contradiction to the MaterialMain::handleThumbDeleted the name can be used,
    // because it represents the filename of the thumb/texture in both
    // mTextureTreeDockWidget and mTextureThumbDockWidget
    mTextureTreeDockWidget->deleteTextureFile(name);
    mSelectedFileName = "";
}

//****************************************************************************/
void TextureMain::handleThumbSelected(const QString& name, const QString& baseName)
{
    // Note, that in contradiction to the MaterialMain::handleThumbSelected the name can be used,
    // because it represents the filename of the thumb/texture in both
    // mTextureTreeDockWidget and mTextureThumbDockWidget
    mSelectedFileName = mTextureTreeDockWidget->setSelectAssetQuiet(name); // name = filename texture
}

//****************************************************************************/
void TextureMain::handleTextureFileDropped(const QString& name, const QString& baseName)
{
    // A texture file was dropped on the mTextureThumbDockWidget. Also add it
    // to the mTextureTreeDockWidget
    mTextureTreeDockWidget->addTextureFile(name);
}

//****************************************************************************/
void TextureMain::handleThumbDoubleClicked(const QString& name, const QString& baseName)
{
    emit textureDoubleClicked(0, 0, 0, name, baseName);
}

//****************************************************************************/
void TextureMain::handleCustomContextMenuItemSelected(const QString& menuItemText)
{
    emit customContextMenuItemSelected(menuItemText);
}

//****************************************************************************/
QMessageBox::StandardButton TextureMain::fileDoesNotExistsWarning(const QString& fileName)
{
    return QMessageBox::question(0,
                                 "Warning",
                                 fileName + QString(" does not exist. Remove it from the Texture browser?"),
                                 QMessageBox::Yes|QMessageBox::No);
}

//****************************************************************************/
const QString& TextureMain::getDraggedFileName (void)
{
    return mTextureTreeDockWidget->getDraggedFileName();
}

//****************************************************************************/
const QPixmap* TextureMain::getCurrentPixmap (void)
{
    return mTextureThumbsDockWidget->getCurrentPixmap();
}

//****************************************************************************/
void TextureMain::clearResources (void)
{
    mTextureTreeDockWidget->clearContent();
    mTextureThumbsDockWidget->clearContent();
}



/****************************************************************************
**
** Copyright (C) 2016 - 2017
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

#ifndef RenderwindowDOCKWIDGET_H
#define RenderwindowDOCKWIDGET_H

#include "constants.h"
#include <QtWidgets>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QToolBar>
#include <QTabWidget>
#include "ogre3_renderman.h"
#include "tb_transformationwidget.h"
#include "OgreHlmsPbsDatablock.h"
#include "OgreHlmsUnlitDatablock.h"

QT_BEGIN_NAMESPACE
class QDockWidget;
QT_END_NAMESPACE

class MainWindow;

struct MeshStruct
{
    QString meshName;
    QVector3D scale;
};


/****************************************************************************
 This class represents an Undo/Redo stack for painting
 ***************************************************************************/
class UndoRedoStack
{
    public:
        struct UndoRedoStackEntry
        {
            Ogre::PbsTextureTypes textureType;
            Ogre::ushort textureSequence;
        };

        UndoRedoStack (void);
        ~UndoRedoStack (void);

        /* Delete all entries from the stack
         */
        void clearStack (void);

        /* Returns the size of the stack
         */
        int getStackSize (void);

        /* Move one step back in the stack.
         * The first entry cannot be passed
         */
        UndoRedoStackEntry undo (void);

        /* Move one step forward in the stack
         * The last entry cannot be passed
         */
        UndoRedoStackEntry redo (void);

        /* Add a new entry to the stack
         */
        void addEntry (const UndoRedoStackEntry& entry);

    private:
        QVector<UndoRedoStackEntry> mStack;
        int mStackPointer;
};


/****************************************************************************
 This class represents a DockWidget
 ***************************************************************************/
class RenderwindowDockWidget : public QDockWidget
{
	Q_OBJECT

	public:
		RenderwindowDockWidget(QString title, MainWindow* parent, Qt::WindowFlags flags = 0);
		~RenderwindowDockWidget(void);
		void createActions(void);
		void createMenus(void);
		void createToolBars(void);
        void updateTransformationWidgetFromOgreWidget(void);
        void addToMeshMap(const QString name,
                          const QString meshName,
                          QVector3D scale);
        void mousePressEventPublic (QMouseEvent* e);
        void enterEventPublic (QEvent* event);
        void leaveEventPublic (QEvent* event);
        void addUndoRedoStackEntry (Ogre::PbsTextureTypes textureType, Ogre::ushort textureSequence);
        void clearUndoRedoStackEntry (void);
        void notifyHlmsPropertiesPbsSamplerblockVisible (bool visible,
                                                         const Ogre::IdString& datablockId,
                                                         const Ogre::PbsTextureTypes textureType); // Called when a pbs samplerblock is visible/invisible
        void notifyHlmsPropertiesUnlitSamplerblockVisible (bool visible,
                                                           const Ogre::IdString& datablockId,
                                                           Ogre::uint8 textureType); // Called when an unlit samplerblock is visible/invisible
        void notifyOffsetTextureUpdated (float offsetX, float offsetY); // Texture offset is changed
        void setCameraOrientationChanged (const Ogre::Quaternion& orientation); // Called when the camera in the Ogrewidget rotates around the item
        void setCameraPositionChanged (const Ogre::Vector3& position); // Called when the camera in the Ogrewidget moves

    public slots:
        void handleTogglePaintMode(void); // Must be public, because it is also used by the ogre widget

	private slots:
        void doChangeMeshAction(QString *actionText);
        void handleToggleModelAndLight(void);
        void handleMarker(void);
        void handleToggleHoover(void);
        void handleToggleOffsetTexture(void);
        void handleUndo(void);
        void handleRedo(void);
        void doTransformationWidgetValueChanged(void);
        void doChangeBackgroundAction(void);
        void contextMenuSelected(QAction* action);

    protected:
        // This function is used to preload the meshes in the dropdown listbox. Strictly speaking this is not needed,
        // but for some reason, after a V2 mesh is loaded from file (by means of File -> Open -> Mesh), the MeshSerializer
        // thinks it should support a lower version of the mesh in the dropdown listbox.
        // Something has been screwed up (Ogre bug?) but I didn't bother to look into it. Preloading of all meshes in
        // the models.cfg solves the issue.
        void preLoadMeshMap(void);

        virtual void resizeEvent(QResizeEvent *e);
        virtual void mousePressEvent( QMouseEvent* e );
        void addMeshNameToContextMenu (const QString& meshName, bool checked);
        void addSkyBoxNameToContextMenu (const QString& skyBoxName, bool checked);
        void setCheckedMeshNameInContextMenu (const QString& meshName);
        void setCheckedSkyBoxNameInContextMenu (const QString& skyBoxName);
        void setModelAndLight(bool enabled);
        void setHoover(bool enabled);
        void setPaintMode(bool enabled);
        void setOffsetTextureMode(bool enabled, bool showMessage = false);

	private:
		MainWindow* mParent;
        QMainWindow* mInnerMain;
        Magus::QOgreWidget* mOgreWidget;
        QAction* mChangeBackgroundAction;
        QToolBar* mHToolBar;
        QPushButton* mButtonToggleModelAndLight;
        QPushButton* mButtonTogglePaint;
        QPushButton* mButtonMarker;
        QPushButton* mButtonToggleHoover;
        QPushButton* mButtonOffsetTexture;
        QPushButton* mButtonUndo;
        QPushButton* mButtonRedo;
        QMap<QString, MeshStruct> mMeshMap;
        Magus::TransformationWidget* mTransformationWidget;
        QMenu* mMeshMenu;
        bool mButtonModelActive;
        bool mToggleHooverOn; // If true, use mouseover to highlight the subItems
        bool mTogglePaintMode; // If true, painting mode is active
        bool mToggleOffsetTexture; // If true, moving textures (offset) is active
        QIcon* mLightIcon;
        QIcon* mPaintOnIcon;
        QIcon* mPaintOffIcon;
        QIcon* mModelIcon;
        QIcon* mMarkerIcon;
        QIcon* mHooverOnIcon;
        QIcon* mHooverOffIcon;
        QIcon* mOffsetTextureOnIcon;
        QIcon* mOffsetTextureOffIcon;
        QIcon* mPaintUndoOffIcon;
        QIcon* mPaintUndoOnIcon;
        QIcon* mPaintRedoOffIcon;
        QIcon* mPaintRedoOnIcon;
        QMenu* mContextMenu;
        QMenu* mMeshesSubMenu;
        QActionGroup* mActionGroupMeshes;
        QMenu* mSkyBoxSubMenu;
        QActionGroup* mActionGroupSkyBox;
        UndoRedoStack mUndoRedoStack;
        Ogre::IdString mCurrentPbsDatablockBlock; // Use the IdString instead of a pointer, because the pointer may change
        Ogre::IdString mCurrentUnlitDatablockBlock; // Use the IdString instead of a pointer, because the pointer may change
        Ogre::PbsTextureTypes mCurrentPbsTextureType;
        Ogre::uint8 mCurrentUnlitTextureType;
};

#endif


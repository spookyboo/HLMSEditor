/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __HLMS_EDITOR_PLUGIN_H__
#define __HLMS_EDITOR_PLUGIN_H__

#include "OgreString.h"

static const Ogre::String GENERAL_HLMS_PLUGIN_NAME = "HlmsEditorPlugin";

namespace Ogre
{
    enum PLUGIN_ACTION_FLAG
    {
        PAF_PRE_IMPORT_MK_DIR = 1 << 0,
        PAF_POST_IMPORT_OPEN_PROJECT = 1 << 1,
        PAF_POST_IMPORT_SAVE_RESOURCE_LOCATIONS = 1 << 2
    };

    /** Class to pass data from Hlms editor to plugins */
    class HlmsEditorPluginData
    {
        public:
            // Public Hlms Editor data, exposed to the plugin

            // Input (input for the plugin, output from the HLMS Editor)
            String mInProjectName; // The name of the project; without path and without extenstion
            String mInProjectPath; // The path where the projectfile, material configfile and texture configfile are stored
            String mInMaterialFileName; // The name of the material configfile
            String mInTextureFileName; // The name of the texture configfile
            String mInFileDialogName; // The full qualified name of the file selected by means of a filedialog (used for import/export)
            String mInFileDialogBaseName; // The name of the file selected by means of a filedialog (used for import/export), without path AND without extension
            String mInFileDialogPath; // The path of the file selected by means of a filedialog (used for import/export)
            //String mInImportExportPath; // The default path used to import or export (= mInFileDialogPath when a filedialog is used)
            String mInImportPath; // The default path used to import
            String mInExportPath; // The default path used to export
            std::vector<String> mInMaterialFileNameVector; // Vector with material names (fileName) in the material browser
            std::vector<String> mInTextureFileNameVector; // Vector with texture names (fileName) in the texture browser
            std::vector<String> mInTexturesUsedByDatablocks; // Vector with texture basenames from all the Pbs and Unlit datablocks in the material browser
            Ogre::Item* mInItem; // The currently selected Item in the renderwidget
            Ogre::RenderWindow* mInRenderWindow; // The renderwindow of the renderwidget
            Ogre::SceneManager* mInSceneManager; // The scenemanager used in the renderwidget

            // Input/output (data is passed two-way)

            /* Pointer to the currently editted datablock, set by the HLMS Editor; It can also be used to return a pointer
             * from the plugin to the HLMS editor (in case isImport() return true). This pointer will be used to generate
             * a node structure.
             * Note, that there is a possibility that it becomes a problem to access data from the OgreHlmsPbs / OgreHlmsUnlit components
             */
            Ogre::HlmsDatablock* mInOutCurrentDatablock;


            // Output (output from the plugin, input for the HLMS Editor)
            String mOutExportReference; // To be filled in by the plugin; this can be a (file) reference of the export
            String mOutSuccessText; // In case the function was executed correctly, this text can be displayed
            String mOutErrorText; // In case the function was not executed correctly, this errortext can be displayed

            // Constructor
            HlmsEditorPluginData (void)
            {
                mInProjectName = "";
                mInProjectPath = "";
                mInMaterialFileName = "";
                mInTextureFileName = "";
                mInFileDialogName = "";
                mInFileDialogBaseName = "";
                mInFileDialogPath = "";
                //mInImportExportPath = "";
                mInImportPath = "";
                mInExportPath = "";
                mInItem = 0;
                mInRenderWindow = 0;
                mInSceneManager = 0;
                mInOutCurrentDatablock = 0;
                mOutErrorText = "Error while performing this function";
                mOutExportReference = "";
                mOutSuccessText = "";
                mInMaterialFileNameVector.clear();
                mInTextureFileNameVector.clear();
                mInTexturesUsedByDatablocks.clear();
            }

            // Destructor
            virtual ~HlmsEditorPluginData (void) {}
    };

    /** Abstract class for Hlms editor plugins */
    class HlmsEditorPlugin
    {
		public:
            HlmsEditorPlugin (void) {}
            virtual ~HlmsEditorPlugin (void) {}

            // The plugin indicates that the HLMS Editor must open a filedialog before the
            // executeImport() function is added. The filename (mInFileDialogName + mInFileDialogPath) must be passed
            virtual bool isOpenFileDialogForImport (void) const = 0;

            // Does the plugin perform an import?
			virtual bool isImport (void) const = 0;
			
            // The plugin indicates that the HLMS Editor must open a filedialog before the
            // executeExport () function is added. The output directory (mInFileDialogPath) must be passed
            virtual bool isOpenFileDialogForExport (void) const = 0;

            // If true, the HLMS editor fills the mInTexturesUsedByDatablocks vector
            // This is a performance improvement (otherwise the editor always provides the data even if not needed)
            virtual bool isTexturesUsedByDatablocksForExport (void) const = 0;

            // Does the plugin perform an export?
			virtual bool isExport (void) const = 0;

			// Return the text to be presented in the import menu
			virtual const String& getImportMenuText (void) const = 0;

			// Return the text to be presented in the export menu
			virtual const String& getExportMenuText (void) const = 0;

			// Perform the import
            virtual bool executeImport (HlmsEditorPluginData* data) = 0;

			// Perform the export
            virtual bool executeExport (HlmsEditorPluginData* data) = 0;

            // The editor must perform (additional) pre- and/or post actions
            virtual void performPreImportActions (void) = 0;
            virtual void performPostImportActions (void) = 0;

            // The editor must perform (additional) pre- and/or post actions
            virtual void performPreExportActions (void) = 0;
            virtual void performPostExportActions (void) = 0;

            // The plugin can also inform the editor to perform a pre- or post action
            virtual unsigned int getActionFlag (void) = 0;
    };
}

#endif

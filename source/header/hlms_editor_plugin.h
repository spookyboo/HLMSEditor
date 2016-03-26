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
    /** Class to pass data from Hlms editor to plugins */
    class HlmsEditorPluginData
    {
        public:
            // Public Hlms Editor data, exposed to the plugin

            // Input
            String mInProjectName; // The name of the project; without path and without extenstion
            String mInProjectPath; // The path where the projectfile, material configfile and texture configfile are stored
            String mInMaterialFileName; // The name of the material configfile
            String mInTextureFileName; // The name of the texture configfile
            std::vector<String> mInMaterialFileNameVector; // Vector with material names (fileName) in the material browser
            Ogre::Item* mInItem; // The currently selected Item in the renderwidget
            Ogre::RenderWindow* mInRenderWindow; // The renderwindow of the renderwidget
            Ogre::SceneManager* mInSceneManager; // The scenemanager used in the renderwidget

            // Input/output
            Ogre::HlmsDatablock* mInOutCurrenDatablock; // Pointer to the currently editted datablock

            // Output
            String mOutSuccessText; // In case the function was executed correctly, this text can be displayed
            String mOutErrorText; // In case the function was not executed correctly, this errortext can be displayed

            // Constructor
            HlmsEditorPluginData (void)
            {
                mInProjectName = "";
                mInProjectPath = "";
                mInMaterialFileName = "";
                mInTextureFileName = "";
                mInItem = 0;
                mInRenderWindow = 0;
                mInSceneManager = 0;
                mInOutCurrenDatablock = 0;
                mOutErrorText = "Error while performing this function";
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
		
			// Does the plugin perform an import?
			virtual bool isImport (void) const = 0;
			
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
	};
}

#endif

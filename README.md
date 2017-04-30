# HLMS Editor
HLMS Editor

Editor to create Ogre3D High Level Material Shader materials (Ogre 2.0+)

![HLMS Editor](/HLMSEditor.png)

**Installation:**  
Step 1. Download the zip or clone the 'HLMSEditor' repository from Github.  
Step 2. Compile the sources of Ogre3D version 2.1 (from https://bitbucket.org/sinbad/ogre)  
Step 3. Copy the following Ogre 2.1 DLL's/EXE's to the HLMSEditor/bin directory  
* OgreHlmsPbs.dll
* OgreHlmsPbs_d.dll
* OgreHlmsUnlit.dll
* OgreHlmsUnlit_d.dll
* OgreMain.dll
* OgreMain_d.dll
* RenderSystem_Direct3D11.dll
* RenderSystem_Direct3D11_d.dll
* RenderSystem_GL3Plus.dll
* RenderSystem_GL3Plus_d.dll
* RenderSystem_NULL.dll
* RenderSystem_NULL_d.dll
* OgreMeshLodGenerator.dll
* OgreMeshTool.exe

Step 4. Copy the following Qt (5.x) DLL's to the HLMSEditor/bin directory
* Qt5Core.dll
* Qt5Gui.dll
* Qt5Multimedia.dll
* Qt5Network.dll
* Qt5OpenGL.dll
* Qt5Widgets.dll
* platform/qwindows.dll (if you use windows, of course)  
* imageformats/qgif.dll
* imageformats/qicns.dll
* imageformats/qico.dll
* imageformats/qjpeg.dll
* imageformats/qsvg.dll
* imageformats/qtga.dll
* imageformats/qtiff.dll
* imageformats/qwbmp.dll
* imageformats/qwebp.dll

Step 5a. Run 'Qt Creator'  
Step 6a. Open the 'HLMSEditor.pro' in Qt Creator  
Step 7a. Edit 'HLMSEditor.pro' file; change OGREHOME = "C:/Users/Henry/Documents/Visual Studio 2015/Projects/ogre" to the path that refers to your ogre root/home  
Step 8a. Run the HLMSEditor application  
Step 9a. Note, that you don't need the Qt dll files in your HLMSEditor/bin directory, unless the HLMSEditor application is used as stand-alone (outside Qt Creator).  

Step 5b. Start CMake (download gui version on https://cmake.org/)  
Step 6b. Browse location 'Where is the source code' and 'Where to build the binaries' and click 'Configure'  
Step 7b. Click 'Generate'  
(note, that CMakeLists.txt may need some more attention)
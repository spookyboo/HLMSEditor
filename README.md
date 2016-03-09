# HLMS Editor
HLMS Editor

Editor to create Ogre3D High Level Material Shader materials (Ogre 2.0+)

![HLMS Editor](/HLMSEditor.png)

Installation:<br/>
---
1. Download the zip or clone the 'HLMSEditor' repository from Github.<br/>
2. Compile the sources of Ogre3D version 2.1 (from https://bitbucket.org/sinbad/ogre)<br/>
3. Copy the following DLL's to the HLMSEditor/bin directory<br/>
* OgreHlmsPbs.dll<br/>
* OgreHlmsPbs_d.dll<br/>
* OgreHlmsUnlit.dll<br/>
* OgreHlmsUnlit_d.dll<br/>
* OgreMain.dll<br/>
* OgreMain_d.dll<br/>
* RenderSystem_Direct3D11.dll<br/>
* RenderSystem_Direct3D11_d.dll<br/>
* RenderSystem_GL3Plus.dll<br/>
* RenderSystem_GL3Plus_d.dll<br/>

4. Run 'Qt Creator'<br/>
5. Open the 'HLMSEditor.pro' in Qt Creator<br/>
6. Edit 'HLMSEditor.pro' file; change OGREHOME = "C:/Users/Henry/Documents/Visual Studio 2015/Projects/ogre" to the path that refers to your ogre root/home<br/>
7. Run the HLMSEditor application<br/>
8. Note, that you don't need the Qt dll files in your HLMSEditor/bin directory, unless the HLMSEditor application is used as stand-alone (outside Qt Creator).<br/>

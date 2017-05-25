QT += widgets opengl multimedia

OGREHOME = "C:/Users/Henry/Documents/Visual Studio 2015/Projects/ogre2.1"
win32:RC_ICONS += "common/icons/HLMSEditor.ico"

HEADERS       = \
    source/header/mainwindow.h \
    source/header/renderwindow_dockwidget.h \
    source/header/properties_dockwidget.h \
    source/header/nodeeditor_dockwidget.h \
    framework/header/asset_assetconstants.h \
    framework/header/asset_assetwidget.h \
    framework/header/asset_clickableframe.h \
    framework/header/asset_clickablelabel.h \
    framework/header/asset_containerwidget.h \
    framework/header/asset_curve_dialog.h \
    framework/header/asset_curve_grid.h \
    framework/header/asset_propertywidget.h \
    framework/header/asset_propertywidget_checkbox.h \
    framework/header/asset_propertywidget_color.h \
    framework/header/asset_propertywidget_curve.h \
    framework/header/asset_propertywidget_decimal.h \
    framework/header/asset_propertywidget_quaternion.h \
    framework/header/asset_propertywidget_select.h \
    framework/header/asset_propertywidget_slider.h \
    framework/header/asset_propertywidget_string.h \
    framework/header/asset_propertywidget_texture.h \
    framework/header/asset_propertywidget_xy.h \
    framework/header/asset_propertywidget_xyz.h \
    framework/header/magus_core.h \
    framework/header/node_compound.h \
    framework/header/node_connection.h \
    framework/header/node_constants.h \
    framework/header/node_editorwidget.h \
    framework/header/node_node.h \
    framework/header/node_port.h \
    framework/header/node_porttype.h \
    framework/header/ogre3_cameraman.h \
    framework/header/ogre3_renderman.h \
    framework/header/ogre3_widget.h \
    framework/header/ogre_prereqs.h \
    source/header/hlms_node_pbs_datablock.h \
    source/header/hlms_node_porttypes.h \
    source/header/constants.h \
    source/header/hlms_node_samplerblock.h \
    source/header/hlms_node_macroblock.h \
    source/header/hlms_node_blenddblock.h \
    source/header/hlms_properties_blenddblock.h \
    source/header/hlms_properties_macroblock.h \
    source/header/hlms_properties_pbs_datablock.h \
    source/header/hlms_properties_samplerblock.h \
    source/header/hlms_pbs_builder.h \
    source/header/hlms_builder.h \
    source/header/hlms_properties_unlit_datablock.h \
    source/header/hlms_node_unlit_datablock.h \
    source/header/hlms_unlit_builder.h \
    framework/header/tb_transformationwidget.h \
    framework/header/magus_treewidget.h \
    framework/header/node_scene.h \
    framework/header/tool_default_texturewidget.h \
    framework/header/tool_resourcetree_widget.h \
    source/header/material_browser.h \
    source/header/material_tree.h \
    source/header/material_main.h \
    source/header/material_thumbs.h \
    source/header/material_browser_dialog.h \
    source/header/texture_dockwidget.h \
    source/header/texture_main.h \
    source/header/texture_thumbs.h \
    source/header/texture_tree.h \
    framework/header/node_view.h \
    source/header/hlms_editor_plugin.h \
    source/header/hlms_editor_plugin_action.h \
    source/header/recent_file_action.h \
    source/header/config_dialog.h \
    source/header/config_pages.h \
    source/header/hlms_utils_manager.h \
    framework/header/asset_propertywidget_slider_decimal.h \
    source/header/paintlayer.h \
    source/header/texturelayer.h \
    source/header/paintlayer_manager.h \
    source/header/texturelayer_manager.h \
    source/header/paint_dockwidget.h \
    source/header/central_dockwidget.h \
    source/header/paintlayer_dockwidget.h \
    source/header/paintlayer_widget.h \
    source/header/paintlayer_dialog.h \
    source/header/brush_widget.h \
    source/header/brush_preset_dockwidget.h \
    source/header/preset_widget.h \
    framework/header/tool_generic_assetwidget.h \
    source/header/clipboard_widget.h


SOURCES       = \
    source/src/main.cpp \
    source/src/mainwindow.cpp \
    source/src/renderwindow_dockwidget.cpp \ 
    source/src/properties_dockwidget.cpp \ 
    source/src/nodeeditor_dockwidget.cpp \ 
    framework/src/asset_assetwidget.cpp \
    framework/src/asset_containerwidget.cpp \
    framework/src/asset_curve_dialog.cpp \
    framework/src/asset_curve_grid.cpp \
    framework/src/asset_propertywidget_checkbox.cpp \
    framework/src/asset_propertywidget_color.cpp \
    framework/src/asset_propertywidget_curve.cpp \
    framework/src/asset_propertywidget_decimal.cpp \
    framework/src/asset_propertywidget_quaternion.cpp \
    framework/src/asset_propertywidget_select.cpp \
    framework/src/asset_propertywidget_slider.cpp \
    framework/src/asset_propertywidget_string.cpp \
    framework/src/asset_propertywidget_texture.cpp \
    framework/src/asset_propertywidget_xy.cpp \
    framework/src/asset_propertywidget_xyz.cpp \
    framework/src/node_compound.cpp \
    framework/src/node_connection.cpp \
    framework/src/node_editorwidget.cpp \
    framework/src/node_node.cpp \
    framework/src/node_port.cpp \
    framework/src/node_porttype.cpp \
    framework/src/ogre3_renderman.cpp \
    framework/src/ogre3_widget.cpp \
    source/src/hlms_node_pbs_datablock.cpp \
    source/src/hlms_node_porttypes.cpp \
    source/src/hlms_node_samplerblock.cpp \
    source/src/hlms_node_macroblock.cpp \
    source/src/hlms_node_blenddblock.cpp \
    source/src/hlms_properties_blenddblock.cpp \
    source/src/hlms_properties_macroblock.cpp \
    source/src/hlms_properties_pbs_datablock.cpp \
    source/src/hlms_properties_samplerblock.cpp \
    source/src/hlms_pbs_builder.cpp \
    source/src/hlms_builder.cpp \
    source/src/hlms_properties_unlit_datablock.cpp \
    source/src/hlms_node_unlit_datablock.cpp \
    source/src/hlms_unlit_builder.cpp \
    framework/src/tb_transformationwidget.cpp \
    framework/src/magus_treewidget.cpp \
    framework/src/tool_default_texturewidget.cpp \
    framework/src/tool_resourcetree_widget.cpp \
    source/src/material_browser.cpp \
    source/src/material_main.cpp \
    source/src/material_tree.cpp \
    source/src/material_thumbs.cpp \
    source/src/material_browser_dialog.cpp \
    source/src/texture_dockwidget.cpp \
    source/src/texture_main.cpp \
    source/src/texture_thumbs.cpp \
    source/src/texture_tree.cpp \
    source/src/hlms_editor_plugin_action.cpp \
    source/src/recent_file_action.cpp \
    source/src/config_dialog.cpp \
    source/src/config_pages.cpp \
    source/src/hlms_utils_manager.cpp \
    framework/src/asset_propertywidget_slider_decimal.cpp \
    source/src/paintlayer.cpp \
    source/src/texturelayer.cpp \
    source/src/paintlayer_manager.cpp \
    source/src/texturelayer_manager.cpp \
    source/src/paint_dockwidget.cpp \
    source/src/central_dockwidget.cpp \
    source/src/paintlayer_dockwidget.cpp \
    source/src/paintlayer_widget.cpp \
    source/src/paintlayer_dialog.cpp \
    source/src/brush_widget.cpp \
    source/src/brush_preset_dockwidget.cpp \
    source/src/preset_widget.cpp \
    framework/src/tool_generic_assetwidget.cpp \
    source/src/clipboard_widget.cpp


INCLUDEPATH += "../HLMSEditor/source/header/"
INCLUDEPATH += "../HLMSEditor/framework/header"
INCLUDEPATH += .
win32 {
    INCLUDEPATH += "$$OGREHOME/OgreMain/include"
    INCLUDEPATH += "$$OGREHOME/Components/Hlms/Common/include"
    INCLUDEPATH += "$$OGREHOME/Components/Hlms/Pbs/include"
    INCLUDEPATH += "$$OGREHOME/Components/Hlms/Unlit/include"
    INCLUDEPATH += "$$OGREHOME/VCBuild/include"
    INCLUDEPATH += "$$OGREHOME/Dependencies"
    INCLUDEPATH += "$$OGREHOME/Dependencies/src/rapidjson"
} unix {
    exists( "/usr/local/include/OGRE" ) {
        INCLUDEPATH += "/usr/local/include/OGRE"
        INCLUDEPATH += "/usr/local/include/OGRE/Hlms/Pbs"
        INCLUDEPATH += "/usr/local/include/OGRE/Hlms/Unlit"
    } else:exists( "/usr/include/OGRE" ) {
        INCLUDEPATH += "/usr/include/OGRE"
        INCLUDEPATH += "/usr/include/OGRE/Hlms/Pbs"
        INCLUDEPATH += "/usr/include/OGRE/Hlms/Unlit"
    } else {
        error("Could not find Ogre.")
    }
}

win32 {
    Debug:LIBS += -L"$$OGREHOME/VCBuild/lib/Debug"
    Release:LIBS += -L"$$OGREHOME/VCBuild/lib/Release"
}

win32:LIBS += -lopengl32
else:unix:LIBS += -lGL -lGLU

CONFIG(debug, debug|release):LIBS += \
    -lOgreMain_d \
    -lOgreHlmsPbs_d \
    -lOgreHlmsUnlit_d

CONFIG(release, debug|release):LIBS += \
    -lOgreMain \
    -lOgreHlmsPbs \
    -lOgreHlmsUnlit



Release:DESTDIR = ../HLMSEditor/bin
Debug:DESTDIR = ../HLMSEditor/bin
target.path = $$[QTDIR]/
INSTALLS += target

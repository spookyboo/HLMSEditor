#-------------------------------------------------------------------
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0)

# Use relative paths
if(WIN32)
	set(CMAKE_USE_RELATIVE_PATHS true)
	set(CMAKE_SUPPRESS_REGENERATION true)
	if(EXISTS "$ENV{OGRE_HOME}/CMake")
		file(TO_CMAKE_PATH $ENV{OGRE_HOME} OGRE_HOME)
		set(CMAKE_MODULE_PATH "${OGRE_HOME}/CMake/;${CMAKE_MODULE_PATH}")
	elseif(EXISTS "C:/Program Files (x86)/OGRE/CMake")
		set(CMAKE_MODULE_PATH "C:/Program Files (x86)/CMake/;${CMAKE_MODULE_PATH}")
	elseif(EXISTS "C:/Program Files/OGRE/CMake")
		set(CMAKE_MODULE_PATH "C:/Program Files/CMake/;${CMAKE_MODULE_PATH}")
	endif()
	if(EXISTS "$ENV{QTDIR}/lib/cmake")
		file(TO_CMAKE_PATH $ENV{QTDIR} QTDIR)
		set(Qt5_DIR "${QTDIR}/lib/cmake/Qt5")
		set(CMAKE_MODULE_PATH "${QTDIR}/lib/cmake/Qt5;${QTDIR}/lib/cmake/Qt5Core;${QTDIR}/lib/cmake/Qt5Gui;${QTDIR}/lib/cmake/Qt5Multimedia;${QTDIR}/lib/cmake/Qt5OpenGL;${QTDIR}/lib/cmake/Qt5Widgets;${CMAKE_MODULE_PATH}")
	endif()
endif()

if(UNIX)
	if(EXISTS "/usr/local/lib/OGRE/cmake")
		set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	elseif(EXISTS "/usr/lib/OGRE/cmake")
		set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
	elseif(EXISTS "/usr/share/OGRE/cmake/modules")
		set(CMAKE_MODULE_PATH "/usr/share/OGRE/cmake/modules;${CMAKE_MODULE_PATH}")
	else ()
		message(SEND_ERROR "Failed to find module path.")
	endif()
endif()

if (CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None (CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

set(CMAKE_DEBUG_POSTFIX "_d")

project(HLMSEditor)

if(WIN32 OR APPLE)
	set(Boost_USE_STATIC_LIBS TRUE)
endif()
if(APPLE AND OGRE_BUILD_PLATFORM_APPLE_IOS)
    set(Boost_USE_MULTITHREADED OFF)
endif()
if(ANDROID)
    set(Boost_COMPILER -gcc)
endif()

find_package(OGRE COMPONENTS HlmsPbs HlmsUnlit REQUIRED)
if(OGRE_FOUND)
	if(${OGRE_VERSION} VERSION_LESS "2.1.0")
		message(FATAL_ERROR "Ogre 2.1.0 is required!")
	endif()
endif()
include_directories( ${OGRE_INCLUDE_DIRS} )
include_directories( ${OGRE_HlmsPbs_INCLUDE_DIRS} )
include_directories( ${OGRE_HlmsUnlit_INCLUDE_DIRS} )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/source/header/" )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/framework/header" )

set(CMAKE_AUTOMOC ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL Multimedia)
find_package(OpenGL REQUIRED)

set (HEADER_FILES
    framework/header/asset_assetconstants.h
    framework/header/asset_assetwidget.h
    framework/header/asset_clickableframe.h
    framework/header/asset_clickablelabel.h
    framework/header/asset_containerwidget.h
    framework/header/asset_curve_dialog.h
    framework/header/asset_curve_grid.h
    framework/header/asset_propertywidget.h
    framework/header/asset_propertywidget_checkbox.h
    framework/header/asset_propertywidget_color.h
    framework/header/asset_propertywidget_curve.h
    framework/header/asset_propertywidget_decimal.h
    framework/header/asset_propertywidget_quaternion.h
    framework/header/asset_propertywidget_select.h
    framework/header/asset_propertywidget_slider.h
    framework/header/asset_propertywidget_string.h
    framework/header/asset_propertywidget_texture.h
    framework/header/asset_propertywidget_xy.h
    framework/header/asset_propertywidget_xyz.h
    framework/header/magus_core.h
    framework/header/node_compound.h
    framework/header/node_connection.h
    framework/header/node_constants.h
    framework/header/node_editorwidget.h
    framework/header/node_node.h
    framework/header/node_port.h
    framework/header/node_porttype.h
    framework/header/ogre3_cameraman.h
    framework/header/ogre3_renderman.h
    framework/header/ogre3_widget.h
    framework/header/ogre_prereqs.h
    framework/header/node_view.h
    framework/header/tb_transformationwidget.h
    framework/header/magus_treewidget.h
    framework/header/node_scene.h
    framework/header/tool_default_texturewidget.h
    framework/header/tool_resourcetree_widget.h
    framework/header/asset_propertywidget_slider_decimal.h
    source/header/mainwindow.h
    source/header/renderwindow_dockwidget.h
    source/header/properties_dockwidget.h
    source/header/nodeeditor_dockwidget.h
    source/header/hlms_node_pbs_datablock.h
    source/header/hlms_node_porttypes.h
    source/header/constants.h
    source/header/hlms_node_samplerblock.h
    source/header/hlms_node_macroblock.h
    source/header/hlms_node_blenddblock.h
    source/header/hlms_properties_blenddblock.h
    source/header/hlms_properties_macroblock.h
    source/header/hlms_properties_pbs_datablock.h
    source/header/hlms_properties_samplerblock.h
    source/header/hlms_pbs_builder.h
    source/header/hlms_builder.h
    source/header/hlms_properties_unlit_datablock.h
    source/header/hlms_node_unlit_datablock.h
    source/header/hlms_unlit_builder.h
    source/header/material_browser.h
    source/header/material_tree.h
    source/header/material_main.h
    source/header/material_thumbs.h
    source/header/material_browser_dialog.h
    source/header/texture_dockwidget.h
    source/header/texture_main.h
    source/header/texture_thumbs.h
    source/header/texture_tree.h
    source/header/hlms_editor_plugin.h
    source/header/hlms_editor_plugin_action.h
    source/header/recent_file_action.h
    source/header/hlms_utils_manager.h
    source/header/config_dialog.h
    source/header/config_pages.h
    source/header/texturelayer.h
    source/header/texturelayer_manager.h
    source/header/paintlayer.h
    source/header/paintlayer_manager.h
)

set (SOURCE_FILES
    framework/src/asset_assetwidget.cpp
    framework/src/asset_containerwidget.cpp
    framework/src/asset_curve_dialog.cpp
    framework/src/asset_curve_grid.cpp
    framework/src/asset_propertywidget_checkbox.cpp
    framework/src/asset_propertywidget_color.cpp
    framework/src/asset_propertywidget_curve.cpp
    framework/src/asset_propertywidget_decimal.cpp
    framework/src/asset_propertywidget_quaternion.cpp
    framework/src/asset_propertywidget_select.cpp
    framework/src/asset_propertywidget_slider.cpp
    framework/src/asset_propertywidget_slider_decimal.cpp
    framework/src/asset_propertywidget_string.cpp
    framework/src/asset_propertywidget_texture.cpp
    framework/src/asset_propertywidget_xy.cpp
    framework/src/asset_propertywidget_xyz.cpp
    framework/src/magus_treewidget.cpp
    framework/src/node_compound.cpp
    framework/src/node_connection.cpp
    framework/src/node_editorwidget.cpp
    framework/src/node_node.cpp
    framework/src/node_port.cpp
    framework/src/node_porttype.cpp
    framework/src/ogre3_renderman.cpp
    framework/src/ogre3_widget.cpp
    framework/src/tb_transformationwidget.cpp
    framework/src/magus_treewidget.cpp
    framework/src/tool_resourcetree_widget.cpp
    framework/src/tool_default_texturewidget.cpp
    source/src/main.cpp
    source/src/mainwindow.cpp
    source/src/renderwindow_dockwidget.cpp 
    source/src/properties_dockwidget.cpp 
    source/src/nodeeditor_dockwidget.cpp 
    source/src/hlms_node_pbs_datablock.cpp
    source/src/hlms_node_porttypes.cpp
    source/src/hlms_node_samplerblock.cpp
    source/src/hlms_node_macroblock.cpp
    source/src/hlms_node_blenddblock.cpp
    source/src/hlms_properties_blenddblock.cpp
    source/src/hlms_properties_macroblock.cpp
    source/src/hlms_properties_pbs_datablock.cpp
    source/src/hlms_properties_samplerblock.cpp
    source/src/hlms_pbs_builder.cpp
    source/src/hlms_builder.cpp
    source/src/hlms_properties_unlit_datablock.cpp
    source/src/hlms_node_unlit_datablock.cpp
    source/src/hlms_unlit_builder.cpp
    source/src/material_browser.cpp
    source/src/material_main.cpp
    source/src/material_tree.cpp
    source/src/material_thumbs.cpp
    source/src/material_browser_dialog.cpp
    source/src/texture_dockwidget.cpp
    source/src/texture_main.cpp
    source/src/texture_thumbs.cpp
    source/src/texture_tree.cpp
    source/src/hlms_editor_plugin_action.cpp
    source/src/recent_file_action.cpp
    source/src/hlms_utils_manager.cpp
    source/src/config_dialog.cpp
    source/src/config_pages.cpp
    source/src/texturelayer.cpp
    source/src/texturelayer_manager.cpp
    source/src/paintlayer.cpp
    source/src/paintlayer_manager.cpp
)

if(Boost_FOUND)
	link_directories(${Boost_LIBRARY_DIRS})
endif()
add_executable( HLMSEditor ${HEADER_FILES} ${SOURCE_FILES} )
target_link_libraries( HLMSEditor ${OGRE_LIBRARIES} ${OGRE_HlmsPbs_LIBRARIES} ${OGRE_HlmsUnlit_LIBRARIES} ${OPENGL_LIBRARIES} )
if(Boost_FOUND)
	target_link_libraries( HLMSEditor ${Boost_LIBRARIES} )
endif()
qt5_use_modules( HLMSEditor Widgets)
qt5_use_modules( HLMSEditor OpenGL)
qt5_use_modules( HLMSEditor Multimedia)

if(UNIX)
	target_link_libraries( HLMSEditor ${GTK2_LIBRARIES} )
	target_link_libraries( HLMSEditor ${OPENGL_LIBRARIES} )
endif()

# MSVC debugging
if(MSVC)
	# Create file for msvc debugging environment
	set(OUTPUT_DEBUG_FILE "${CMAKE_CURRENT_BINARY_DIR}/HLMSEditor.vcxproj.user")
	file(WRITE ${OUTPUT_DEBUG_FILE} "<?xml version='1.0' encoding='utf-8'?>\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "<Project ToolsVersion='12.0' xmlns='http://schemas.microsoft.com/developer/msbuild/2003'>\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "  <PropertyGroup>\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "    <QTDIR>${QTDIR}</QTDIR>\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "    <OGRE_HOME>${OGRE_HOME}</OGRE_HOME>\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "    <LocalDebuggerEnvironment>PATH=$(QTDIR)\\bin%3b$(OGRE_HOME)\\bin\\$(Configuration)%3b$(PATH)</LocalDebuggerEnvironment>\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "    <LocalDebuggerWorkingDirectory>$(TargetDir)</LocalDebuggerWorkingDirectory>\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "  </PropertyGroup>\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "</Project>\n")
	set(OUTPUT_DEBUG_FILE "${CMAKE_CURRENT_BINARY_DIR}/Debug/resources_d.cfg")
	file(WRITE ${OUTPUT_DEBUG_FILE} "[Autodetect]\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "[General]\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "FileSystem = ../common/ogre3\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "FileSystem = ${OGRE_MEDIA_DIR}/materials/textures\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "FileSystem = ${OGRE_MEDIA_DIR}/materials/textures/Cubemaps\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "[Internal]\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "[Hlms]\n")
	file(APPEND ${OUTPUT_DEBUG_FILE} "DoNotUseAsResource = ../common/ogre3\n")
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/bin/models.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/bin/plugins_d.cfg" "${CMAKE_CURRENT_SOURCE_DIR}/bin/dark.qss" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/Debug")
	add_custom_command(TARGET HLMSEditor POST_BUILD COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/common" "${CMAKE_CURRENT_BINARY_DIR}/common")
	
endif()

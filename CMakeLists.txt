#-------------------------------------------------------------------
# The contents of this file are placed in the public domain. Feel
# free to make use of it in any way you like.
#-------------------------------------------------------------------

cmake_minimum_required(VERSION 3.0.2)

# Use relative paths
if(WIN32)
	set(CMAKE_USE_RELATIVE_PATHS true)
	set(CMAKE_SUPPRESS_REGENERATION false)
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
    elseif(IS_DIRECTORY ${QTDIR})
        set(Qt5_DIR ${QTDIR}/lib/cmake/Qt5/)
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

if(WIN32)
    set(CMAKE_MODULE_PATH ${OGRE_HOME}/CMake)
endif()
    

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

find_package(Rapidjson REQUIRED)

include_directories( ${Rapidjson_INCLUDE_DIR} )
include_directories( ${OGRE_INCLUDE_DIRS} )
include_directories( ${OGRE_HlmsPbs_INCLUDE_DIRS} )
include_directories( ${OGRE_HlmsUnlit_INCLUDE_DIRS} )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/source/header/" )
include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/framework/header" )

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL Multimedia)
find_package(OpenGL REQUIRED)

file(GLOB HEADER_FILES source/header/* framework/header/*)

file(GLOB SOURCE_FILES source/src/* framework/src/*)


if(Boost_FOUND)
	link_directories(${Boost_LIBRARY_DIRS})
endif()

add_executable( HLMSEditor ${HEADER_FILES} ${SOURCE_FILES} )

if(MSVC)
    #multi processor compilation 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif(MSVC)

#valid for "single build type generator" like "Unix Makefiles" for Linux 
if(CMAKE_BUILD_TYPE MATCHES Debug)
	target_compile_definitions(HLMSEditor PUBLIC DEBUG=1)
endif()

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
	file(COPY ${OGRE_MEDIA_DIR}/../plugins.cfg DESTINATION ${CMAKE_BINARY_DIR})
	file(COPY ${OGRE_MEDIA_DIR}/../plugins_d.cfg DESTINATION ${CMAKE_BINARY_DIR})

	#install resources from /bin
	file(COPY ${PROJECT_SOURCE_DIR}/bin/resources.cfg ${PROJECT_SOURCE_DIR}/bin/resources_d.cfg
			${PROJECT_SOURCE_DIR}/bin/resources_default.cfg ${PROJECT_SOURCE_DIR}/bin/settings.cfg
			${PROJECT_SOURCE_DIR}/bin/settings_default.cfg ${PROJECT_SOURCE_DIR}/bin/models.cfg
			#QSS
			${PROJECT_SOURCE_DIR}/bin/dark.qss
			DESTINATION ${CMAKE_BINARY_DIR})
endif()

# MSVC debugging
if(MSVC)
	# Create file for msvc debugging environment
    #set(OUTPUT_DEBUG_FILE "${CMAKE_CURRENT_BINARY_DIR}/HLMSEditor.vcxproj.user")
    #file(WRITE ${OUTPUT_DEBUG_FILE} "<?xml version='1.0' encoding='utf-8'?>\n")
    #file(APPEND ${OUTPUT_DEBUG_FILE} "<Project ToolsVersion='12.0' xmlns='http://schemas.microsoft.com/developer/msbuild/2003'>\n")
    #file(APPEND ${OUTPUT_DEBUG_FILE} "  <PropertyGroup>\n")
    #file(APPEND ${OUTPUT_DEBUG_FILE} "    <QTDIR>${QTDIR}</QTDIR>\n")
    #file(APPEND ${OUTPUT_DEBUG_FILE} "    <OGRE_HOME>${OGRE_HOME}</OGRE_HOME>\n")
    #file(APPEND ${OUTPUT_DEBUG_FILE} "    <LocalDebuggerEnvironment>PATH=$(QTDIR)\\bin%3b$(OGRE_HOME)\\bin\\$(Configuration)%3b$(PATH)</LocalDebuggerEnvironment>\n")
    #file(APPEND ${OUTPUT_DEBUG_FILE} "    <LocalDebuggerWorkingDirectory>${CMAKE_BINARY_DIR}</LocalDebuggerWorkingDirectory>\n")
    #file(APPEND ${OUTPUT_DEBUG_FILE} "  </PropertyGroup>\n")
    #file(APPEND ${OUTPUT_DEBUG_FILE} "</Project>\n")
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

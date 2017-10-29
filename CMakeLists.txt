cmake_minimum_required(VERSION 2.8)

project(LeDernierMorkid)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

file(GLOB SRC "src/*.cpp")
file(GLOB GRAPHICSSRC "src/Graphics/*.cpp")
file(GLOB LOGICSRC "src/Logic/*.cpp")
file(GLOB TERRAINSRC "src/Terrain/*.cpp" "src/Terrain/Hlms/*.cpp" "src/Terrain/Hlms/PbsListener/*.cpp")
file(GLOB COLLISIONSRC "Collision/src/*.cpp")
file(GLOB COMMONSRC "Common/src/*.cpp" "Common/src/Threading/*.cpp" "Common/src/System/*.cpp" "Common/src/State/*.cpp")
add_executable(LeDernierMorkid ${SRC} ${GRAPHICSSRC} ${LOGICSRC} ${TERRAINSRC} ${COLLISIONSRC} ${COMMONSRC})

add_custom_target(run COMMAND LeDernierMorkid DEPENDS LeDernierMorkid WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

include_directories (include Common/include Collision/include)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(LEDERNIERMORKID_DEPENDENCIES_DIR "" CACHE PATH "Path to used dependencies")
set(CMAKE_FIND_ROOT_PATH ${LEDERNIERMORKID_DEPENDENCIES_DIR} "${CMAKE_FIND_ROOT_PATH}")
set(OGRE_HOME ${LEDERNIERMORKID_DEPENDENCIES_DIR})
set(ENV${SDL2DIR} ${LEDERNIERMORKID_DEPENDENCIES_DIR})

find_package(Bullet REQUIRED)
include_directories(${BULLET_INCLUDE_DIRS})
target_link_libraries(LeDernierMorkid ${BULLET_LIBRARIES})

find_package(OGRE REQUIRED)
unset(OGRE_FRAMEWORK_PATH CACHE)
include_directories(${OGRE_INCLUDE_DIR})
include_directories(${OGRE_HlmsUnlit_INCLUDE_DIR})
include_directories(${OGRE_HlmsPbs_INCLUDE_DIR})
target_link_libraries(LeDernierMorkid ${OGRE_LIBRARY_REL} ${OGRE_HlmsPbs_LIBRARY_REL} ${OGRE_HlmsUnlit_LIBRARY_REL})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})
target_link_libraries(LeDernierMorkid ${SDL2_LIBRARY})

find_package(CEGUI REQUIRED)
# On default the CMake script return a string ending with CEGUI/ using .. is a simple workaround to avoid breaking the includes
include_directories(${CEGUI_INCLUDE_DIR}/..)
target_link_libraries(LeDernierMorkid ${CEGUI_LIBRARIES} ${CEGUI_OgreRenderer_LIBRARIES})

file(GLOB CONFIGFILES "config/*.cfg" )
file(COPY ${CONFIGFILES} DESTINATION ${EXECUTABLE_OUTPUT_PATH})
file(COPY Media DESTINATION ${PROJECT_BINARY_DIR})

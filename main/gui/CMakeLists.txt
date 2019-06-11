cmake_minimum_required(VERSION 3.12)

project(gui)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCE_FILES "gui.cpp" "elements.cpp")
set(HEADER_FILES "elements.h")
set(RESOURCE_FILES  "resources/AppWindow.qml" 
                    "resources/location.qml" 
                    "resources/componentCreation.js"
                    "resources/fontello.ttf")
    
set(CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.12.2/gcc_64)

find_package( Qt5 COMPONENTS Widgets Core Qml Quick REQUIRED)

set_property(SOURCE source/main.cpp PROPERTY SKIP_AUTOMOC ON)

include_directories(${STRIDE_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${RESOURCES})

target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Widgets Qt5::Core Qt5::Qml Qt5::Quick libstride)

install(TARGETS gui DESTINATION ${BIN_INSTALL_LOCATION}/gui)
install(FILES ${RESOURCE_FILES} DESTINATION ${BIN_INSTALL_LOCATION}/gui/resources)

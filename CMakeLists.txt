cmake_minimum_required(VERSION 3.16)
project(ImageCropper LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
)
qt_standard_project_setup()

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)

file(GLOB PROJECT_SOURCES ${SOURCE_DIR}/*)

set(GLOB FORMS ${SOURCE_DIR}/*.ui)

source_group(FORMS FILES ${FORMS})

qt_add_executable(ImageCropper ${PROJECT_SOURCES})

### OpenCV

find_package(OpenCV REQUIRED)
target_include_directories(ImageCropper
	PRIVATE
		${OpenCV_INCLUDE_DIRS}
)
target_link_directories(ImageCropper
	PRIVATE
		${OpenCV_LIB_DIR}
)
target_link_libraries(ImageCropper
	PRIVATE
		${OpenCV_LIBS}
)

target_include_directories(ImageCropper
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(ImageCropper
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(ImageCropper
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

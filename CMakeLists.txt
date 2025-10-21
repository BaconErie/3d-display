cmake_minimum_required(VERSION 3.10)

project(3d_display_program)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find GTK packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK REQUIRED gtk4)

# Find GLib packages
pkg_check_modules(GLIBMM REQUIRED glibmm-2.4)

# Find OpenCV packages
find_package(OpenCV REQUIRED)

# Find Boost packages
find_package(Boost REQUIRED)

# Include GTK directories
include_directories(${GTK_INCLUDE_DIRS}  ${GLIBMM_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})

# Add compiler flags
add_compile_options(${GTK_CFLAGS_OTHER})

# Create executables
add_executable(3d_display_program main.cpp)
add_executable(cv cv.cpp)


# Check if Blueprint compiler is installed.

find_program(BLUEPRINT_COMPILER blueprint-compiler)

if(NOT BLUEPRINT_COMPILER)
    message(FATAL_ERROR "Blueprint compiler not found")
endif()

# Compile Blueprint UI files and copy to UI directory.
if (UNIX)
    add_custom_target(compile_ui ALL
        COMMAND ${BLUEPRINT_COMPILER} batch-compile ${CMAKE_BINARY_DIR}/ui ${CMAKE_SOURCE_DIR}/ui ${CMAKE_SOURCE_DIR}/ui/*.blp
        COMMENT "Compiling Blueprint UI files"
    )
ELSEIF (WIN32)
    add_custom_target(compile_ui ALL
        COMMAND powershell -Command "Get-ChildItem '${CMAKE_SOURCE_DIR}/ui/*.blp' | ForEach-Object { & '${BLUEPRINT_COMPILER}' $_.FullName -o '${CMAKE_BINARY_DIR}/ui/$($_.BaseName).xml' }"
        COMMENT "Compiling Blueprint UI files"
    )
ELSE()
    message(FATAL_ERROR "Unsupported platform")
ENDIF()

# Copy the CSS file

add_custom_target(copy_css ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/ui/style.css ${CMAKE_BINARY_DIR}/ui/style.css
    COMMENT "Copying CSS file"
)

# Copy model folder

add_custom_target(copy_models ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/models
    COMMENT "Copying model folder"
)

# Make sure UI files are copied before building the main executable
add_dependencies(3d_display_program compile_ui copy_css copy_models)

# Link GTK libraries to your executables
target_link_libraries(3d_display_program ${GTK_LIBRARIES} ${Boost_LIBRARIES} ${OpenCV_LIBS} ${GLIBMM_LIBRARIES})
target_link_libraries(cv ${GTK_LIBRARIES} ${OpenCV_LIBS} ${Boost_LIBRARIES} ${GLIBMM_LIBRARIES})
cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    # This block of code will only be executed for a debug build.
    add_definitions(-DDEBUG_MODE)
endif()

# Set the project name
set(CMAKE_PROJECT_NAME Co2-Sensor-PoC)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Add lvgl
# Disable the building of the separate lvgl_demos target
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE) 

# Disable the building of the separate lvgl_examples target
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(LV_BUILD_CONF_PATH "${PROJECT_SOURCE_DIR}/Core/Inc/lv_conf.h" CACHE PATH "Path to lv_conf.h" FORCE)
add_subdirectory(Middlewares/Third_Party/lvgl)

target_include_directories(lvgl PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/FreeRTOS/Source/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/RTOS2/Include/
)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
    Core/Src/display.c
)


# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/Third_Party/lvgl/
    # Add user defined include paths'
    Core/Inc/
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    lvgl

    # Add user defined libraries
)

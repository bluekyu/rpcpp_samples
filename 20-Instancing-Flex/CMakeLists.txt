# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

cmake_minimum_required(VERSION 3.5)
project(20-Instancing-Flex)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# === project specific packages ===
# find flex
set(FLEX_ROOT "" CACHE PATH "Hint for finding NVIDIA flex root directory")
set(FLEX_INCLUDEDIR "" CACHE PATH "Hint for finding NVIDIA flex include directory")
set(FLEX_LIBRARYDIR "" CACHE PATH "Hint for finding NVIDIA flex library directory")
find_package(Flex REQUIRED)
# =================================

# platform specific configure
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")

    # build type
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: None Debug RelWithDebInfo Release."
            FORCE)
    endif()
    message("${PROJECT_NAME} Build Type: ${CMAKE_BUILD_TYPE}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
    endif()
endif()
# ==================================================================================================

# === sources ======================================================================================
# set input files
set(sources_files
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

# grouping
#source_group("src" FILES ${source_})
# ==================================================================================================

# === build & install===============================================================================
# create exe
add_executable(${PROJECT_NAME} ${sources_files})

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_SOURCE_DIR}/include" "${PROJECT_BINARY_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    ${Panda3d_INCLUDE_DIRS}
    ${Flex_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE $<$<NOT:$<BOOL:Boost_USE_STATIC_LIBS>>:Boost::dynamic_linking>
    Boost::boost
    ${Panda3d_LIBRARIES} render_pipeline
    ${Flex_LIBRARIES}
)

# library
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib")
# ==================================================================================================

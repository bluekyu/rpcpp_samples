# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

cmake_minimum_required(VERSION 3.6)
project(20-Instancing-Flex)

# === configure ====================================================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)    # Project Grouping

# === project specific packages ===
# find flex
set(FLEX_ROOT "" CACHE PATH "Hint for finding NVIDIA flex root directory")
set(FLEX_INCLUDEDIR "" CACHE PATH "Hint for finding NVIDIA flex include directory")
set(FLEX_LIBRARYDIR "" CACHE PATH "Hint for finding NVIDIA flex library directory")
find_package(Flex REQUIRED)

render_pipeline_FIND_PLUGINS("flex")
# ==================================================================================================

# === sources ======================================================================================
# set input files
set(sources_files
    "${PROJECT_SOURCE_DIR}/src/main.cpp"
)

# grouping
#source_group("src" FILES ${source_})
# ==================================================================================================

# === target =======================================================================================
add_executable(${PROJECT_NAME} ${sources_files})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_auto_type)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /MP /wd4251)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall)
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/src"

    ${Flex_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE render_pipeline

    rpplugin::flex
    ${Flex_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "rpcpp_samples")
# ==================================================================================================

# === install ======================================================================================
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib")
# ==================================================================================================

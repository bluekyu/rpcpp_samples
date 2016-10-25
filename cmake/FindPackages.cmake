# Author: Younguk Kim (bluekyu)
# Date  : 2016-08-02

# find boost
option(Boost_USE_STATIC_LIBS    "Boost uses static libraries" OFF)
option(Boost_USE_MULTITHREADED  "Boost uses multithreaded"  ON)
option(Boost_USE_STATIC_RUNTIME "Boost uses static runtime" OFF)

set(BOOST_ROOT "" CACHE PATH "Hint for finding boost root directory")
set(BOOST_INCLUDEDIR "" CACHE PATH "Hint for finding boost include directory")
set(BOOST_LIBRARYDIR "" CACHE PATH "Hint for finding boost library directory")

find_package(Boost 1.59.0 REQUIRED COMPONENTS filesystem system)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    message(STATUS "Boost include path: ${Boost_INCLUDE_DIRS}")
endif()

# find panda3d
set(PANDA3D_ROOT "" CACHE PATH "Hint for finding panda3d root directory")

find_package(Panda3d REQUIRED COMPONENTS libp3framework libpanda libpandaexpress libp3dtool libp3dtoolconfig libp3direct libp3interrogatedb)
if(Panda3d_FOUND)
    include_directories(${Panda3d_INCLUDE_DIRS})
    message(STATUS "Panda3D include path: ${Panda3d_INCLUDE_DIRS}")
endif()

# find yaml-cpp
set(YAMLCPP_ROOT "" CACHE PATH "Hint for finding yaml-cpp root directory")
set(YAMLCPP_INCLUDEDIR "" CACHE PATH "Hint for finding yaml-cpp include directory")
set(YAMLCPP_LIBRARYDIR "" CACHE PATH "Hint for finding yaml-cpp library directory")
option(YAMLCPP_USE_STATIC_LIBS "Yaml-cpp uses static libraries" OFF)
option(YAMLCPP_USE_MULTITHREADED "Yaml-cpp uses non-multithreaded libraries ('mt' tag)" ON)

find_package(YamlCpp REQUIRED)
if(YamlCpp_FOUND)
    include_directories(${YamlCpp_INCLUDE_DIRS})
    message(STATUS "yaml-cpp include path: ${YamlCpp_INCLUDE_DIRS}")
endif()

# find freetype
find_package(FreeType REQUIRED)
if(FREETYPE_FOUND)
    include_directories(${FREETYPE_INCLUDE_DIRS})
    message(STATUS "freetype include path: ${FREETYPE_INCLUDE_DIRS}")
endif()

# find render_pipeline
set(RenderPipeline_ROOT "" CACHE PATH "Hint for finding render pipeline root directory")
set(RenderPipeline_INCLUDEDIR "" CACHE PATH "Hint for finding render pipeline include directory")
set(RenderPipeline_LIBRARYDIR "" CACHE PATH "Hint for finding render pipeline library directory")
find_package(RenderPipeline REQUIRED)
if(RenderPipeline_FOUND)
    include_directories(${RenderPipeline_INCLUDE_DIRS})
    message(STATUS "render pipeline include path: ${RenderPipeline_INCLUDE_DIRS}")
endif()

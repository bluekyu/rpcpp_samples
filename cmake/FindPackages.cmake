# Author: Younguk Kim (bluekyu)
# Date  : 2017-05-12

# find boost
option(Boost_USE_STATIC_LIBS    "Boost uses static libraries" OFF)
option(Boost_USE_MULTITHREADED  "Boost uses multithreaded"  ON)
option(Boost_USE_STATIC_RUNTIME "Boost uses static runtime" OFF)

set(BOOST_ROOT "" CACHE PATH "Hint for finding boost root directory")
set(BOOST_INCLUDEDIR "" CACHE PATH "Hint for finding boost include directory")
set(BOOST_LIBRARYDIR "" CACHE PATH "Hint for finding boost library directory")

find_package(Boost 1.62.0 REQUIRED)

# find panda3d
set(PANDA3D_ROOT "" CACHE PATH "Hint for finding panda3d root directory")

find_package(Panda3d REQUIRED)

# find render_pipeline
find_package(render_pipeline REQUIRED)

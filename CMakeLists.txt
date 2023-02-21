cmake_minimum_required(VERSION 3.1)

project(BookShelfParser VERSION 1
	LANGUAGES CXX
)

set(CMAKE_C_COMPILER "g++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${BookShelfParser_HOME}/cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb -O3 -DIL_STD -ldl")
set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -g -ggdb -O3 -DIL_STD -ldl")

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON) # For CImg

# Home Directory
set(BookShelfParser_HOME ${PROJECT_SOURCE_DIR})
set(CIMG_HOME ${PROJECT_SOURCE_DIR}/module/CImg)

set(GOOGLEHASH_HOME "/tool/cpplib/sparsehash/install")

# Package Settings
# 1. For Boost and Google Hash Map
find_package(Boost REQUIRED)

# 2. For CImg
find_package(X11 REQUIRED)
find_package(JPEG REQUIRED)
find_package(Threads REQUIRED)

# Source Code
set(BookShelfParser_SRC
	src/main.cpp
	src/BookShelfDB.cpp
	src/BookShelfParser.cpp
)

# Include Directory

include_directories(
  ${X11_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}

  ${CIMG_HOME}

  ${GOOGLEHASH_HOME}/include
)

# Executable

add_executable( BookShelfParser ${BookShelfParser_SRC})

# Link Library

target_link_libraries(${PROJECT_NAME} 
	PUBLIC
  	${X11_LIBRARIES}
	Threads::Threads
	X11
	${LIBGOOGLEHASH}
)


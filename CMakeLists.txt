cmake_minimum_required(VERSION 3.10)
project(MyProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(UNIX)
  set(ADDITIONAL_LIBRARIES X11 m pthread) # X11 is needed for CImg (textures) in the builder
else()
  set(ADDITIONAL_LIBRARIES "")
endif()

# Add your source files
add_executable(MyApp src/main.cpp)

# Optionally include the headers if needed
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link the shared library directly
target_link_libraries(MyApp ${ADDITIONAL_LIBRARIES} ${CMAKE_SOURCE_DIR}/lib/liboasis.so)


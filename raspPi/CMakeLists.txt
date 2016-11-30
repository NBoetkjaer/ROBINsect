cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(servo_test)
 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

#Bring the headers into the project
include_directories(include  /usr/include/eigen3/)
 
#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES_top "src/*.cpp")
file(GLOB SOURCES_comm "src/communication/*.cpp")
file(GLOB SOURCES_modules "src/modules/*.cpp")
file(GLOB SOURCES_nodes "src/nodes/*.cpp")
set(SOURCES 
    ${SOURCES_top}
    ${SOURCES_comm}
    ${SOURCES_modules}
    ${SOURCES_nodes}
) 
 
add_executable(ROBINsect ${SOURCES} )

ADD_DEFINITIONS(
    -std=c++14 # Or -std=c++0x
    # Other flags
)

#install(TARGETS ROBINsect DESTINATION bin)

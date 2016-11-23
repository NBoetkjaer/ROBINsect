cmake_minimum_required(VERSION 2.8.0 FATAL_ERROR)
project(servo_test)
 
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

#Bring the headers into the project
include_directories(include  /usr/include/eigen3/)
 
#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")
file(GLOB SOURCES_Comm "src/communication/*.cpp")
 
add_executable(servo_test ${SOURCES} ${SOURCES_Comm})

ADD_DEFINITIONS(
    #-std=c++11 # Or 
    -std=c++0x
    # Other flags
)

#install(TARGETS servo_test DESTINATION bin)

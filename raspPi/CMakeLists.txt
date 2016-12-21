cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(ROBINsect)
 

#set(SOURCES src/mainapp.cpp src/Student.cpp)

#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES_top "src/*.cpp")
file(GLOB SOURCES_comm "src/communication/*.cpp")
file(GLOB SOURCES_modules "src/modules/*.cpp")
file(GLOB SOURCES_nodes "src/nodes/*.cpp")
file(GLOB SOURCES_test "src/test/*.cpp")

 IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	#Set additional compiler flags.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
	 #Bring the headers into the project
	include_directories(include  /usr/include/eigen3/)
	
	set(SOURCES 
		${SOURCES_top}
		${SOURCES_comm}
		${SOURCES_modules}
		${SOURCES_nodes}
	) 
ELSE (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(SOURCES 
		#${SOURCES_top}
		#${SOURCES_comm}
		#${SOURCES_modules}
		${SOURCES_nodes}
		${SOURCES_test}
	) 
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_executable(ROBINsect ${SOURCES} )

ADD_DEFINITIONS(
    -std=c++14 # Or -std=c++0x
    # Other flags
)

#install(TARGETS ROBINsect DESTINATION bin)

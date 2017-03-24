cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(ROBINsect)
 
#The file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES_top "src/*.cpp")
file(GLOB SOURCES_comm "src/communication/*.cpp")
file(GLOB SOURCES_modules "src/modules/*.cpp")
file(GLOB SOURCES_modules_hw "src/modules/hardware/*.cpp")
file(GLOB SOURCES_nodes "src/nodes/*.cpp")
file(GLOB SOURCES_test "src/test/*.cpp")

 IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	#Set additional compiler flags.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")
	 #Bring the headers into the project
	include_directories(include  /usr/include/eigen3/)
	
	set(SOURCES 
		#${SOURCES_top}		
		${SOURCES_comm}
		${SOURCES_modules}
		${SOURCES_modules_hw}		
		${SOURCES_nodes}
		${SOURCES_test}
	)

    ADD_DEFINITIONS(
        -std=c++14 # Or -std=c++0x
        # Other flags
    )

ELSE (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(SOURCES 
		${SOURCES_top}
		${SOURCES_comm}
		${SOURCES_modules}
		${SOURCES_modules_hw}
		${SOURCES_nodes}
		${SOURCES_test}
	)
	set_source_files_properties(${SOURCES_modules_hw} PROPERTIES HEADER_FILE_ONLY true)
	set_source_files_properties(${SOURCES_top} PROPERTIES HEADER_FILE_ONLY true)
	source_group("Modules HW"    	FILES ${SOURCES_top})
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_executable(ROBINsect ${SOURCES} )

source_group("Modules"	FILES ${SOURCES_modules})
source_group("Modules/hardware"	FILES ${SOURCES_modules_hw})
source_group("Nodes"    FILES ${SOURCES_nodes})
source_group("Communication"    FILES ${SOURCES_comm})
source_group("Test"    	FILES ${SOURCES_test})
source_group("Test\\Top"    	FILES ${SOURCES_top})

#install(TARGETS ROBINsect DESTINATION bin)

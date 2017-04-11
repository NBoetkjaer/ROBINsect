cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(ROBINsect)
 
#The file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES_top "src/*.cpp")
file(GLOB SOURCES_comm "src/communication/*.*pp")
file(GLOB SOURCES_modules "src/modules/*.*pp")
file(GLOB SOURCES_modules_hw "src/modules/hardware/*.*pp")
file(GLOB SOURCES_modules_hw_stub "src/modules/hardware/stub/*.*pp")
file(GLOB SOURCES_nodes "src/nodes/*.*pp")
file(GLOB SOURCES_xml "src/nodes/xml/*.*pp")
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
		${SOURCES_modules_hw}
		${SOURCES_nodes}
		${SOURCES_xml}
		${SOURCES_test}
	)
    ADD_DEFINITIONS(
        -std=c++14 # Or -std=c++0x
        # Other flags
    )

ELSE (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	
	include_directories(include  ../../eigen3/)
	set(SOURCES
		${SOURCES_top}
		${SOURCES_comm}
		${SOURCES_modules}
		${SOURCES_modules_hw}
		${SOURCES_modules_hw_stub}
		${SOURCES_nodes}
		${SOURCES_xml}
		${SOURCES_test}
	)
	ADD_DEFINITIONS(	
		-D_CRT_SECURE_NO_WARNINGS
		-DNOMINMAX
	)	
	set_source_files_properties(${SOURCES_modules_hw} PROPERTIES HEADER_FILE_ONLY true)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

add_executable(ROBINsect ${SOURCES} )
				
source_group("Modules"				FILES ${SOURCES_modules})
source_group("Modules\\hardware"	FILES ${SOURCES_modules_hw} ${SOURCES_modules_hw_stub})
source_group("Nodes"				FILES ${SOURCES_nodes})
source_group("Nodes\\xml"			FILES ${SOURCES_xml})
source_group("Communication"		FILES ${SOURCES_comm})
source_group("Test"					FILES ${SOURCES_test})

#install(TARGETS ROBINsect DESTINATION bin)
#configure_file( ${CMAKE_SOURCE_DIR}/config/ROBINsect.xml
#                ${CMAKE_CURRENT_BINARY_DIR}/ROBINsect.xml COPYONLY)
#add_custom_command(
#        TARGET ROBINsect POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#                ${CMAKE_SOURCE_DIR}/config/ROBINsect.xml
#                ${CMAKE_CURRENT_BINARY_DIR}/ROBINsect.xml)
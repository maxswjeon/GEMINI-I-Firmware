cmake_minimum_required(VERSION 3.25)

set(PROJECT rtc-setup)
project(${PROJECT} C CXX)
add_executable(${PROJECT})

file(GLOB_RECURSE SOURCES src/*.cpp)
target_sources(${PROJECT} PRIVATE 
	${SOURCES}
)

target_include_directories(${PROJECT} PRIVATE 
	src
)

set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 17)

if (WIN32)
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/cmake/")
	find_package(ATL REQUIRED)

	file(GLOB_RECURSE ENUMERATE_SERIAL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib/CEnumerateSerial/*.cpp)
	list(REMOVE_ITEM ENUMERATE_SERIAL_SOURCES ${CMAKE_CURRENT_LIST_DIR}/lib/CEnumerateSerial/main.cpp)
	target_sources(${PROJECT} PRIVATE 
		${ENUMERATE_SERIAL_SOURCES}
	)

	target_include_directories(${PROJECT} PRIVATE 
		${CMAKE_CURRENT_LIST_DIR}/lib/CEnumerateSerial
	)
endif()

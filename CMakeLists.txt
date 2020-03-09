cmake_minimum_required(VERSION 3.8)
project(WebSocketServer)

add_definitions("-std=c++11")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(wsserver
	main.cpp
	Listener.h
	Session.h
	WSServer.h
)

target_link_libraries(wsserver ${CONAN_LIBS})
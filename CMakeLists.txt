
cmake_minimum_required(VERSION 3.15)
project(cairo C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CAIRO_FLAGS -Wall -Wextra -Werror -Wpedantic
	-Wshadow -Wundef -Wfloat-equal -Wdouble-promotion -Wformat=2
	-Wconversion -Wsign-conversion -Wcast-qual -Wcast-align=strict
	-Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes
	-Wwrite-strings -Wswitch-enum -Wswitch-default
	-Wnull-dereference -Winit-self -Wjump-misses-init -Wvla -Walloca
	-Wduplicated-cond -Wduplicated-branches -Wlogical-op
)

file(GLOB entries RELATIVE ${CMAKE_SOURCE_DIR}/examples ${CMAKE_SOURCE_DIR}/examples/*)

foreach(entry IN LISTS entries)
	if(NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/examples/${entry})
		continue()
	endif()

	file(GLOB sources ${CMAKE_SOURCE_DIR}/examples/${entry}/*.c)
	add_executable(${entry}_tests ${sources})
	target_include_directories(${entry}_tests PRIVATE ${CMAKE_SOURCE_DIR})
	target_compile_options(${entry}_tests PRIVATE ${CAIRO_FLAGS})
	target_link_libraries(${entry}_tests PRIVATE m)
	add_custom_target(run_${entry}
		COMMAND ${entry}_tests
		DEPENDS ${entry}_tests
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/examples/${entry}
		USES_TERMINAL
	)

	list(APPEND run_targets run_${entry})
endforeach()

add_custom_target(run DEPENDS ${run_targets})

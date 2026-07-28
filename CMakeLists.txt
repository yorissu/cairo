
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

set(run_targets "")

# Iterate over both target directories
foreach(subdir IN ITEMS "examples" "tests")
	set(subdir_path "${CMAKE_SOURCE_DIR}/${subdir}")
	
	if(NOT IS_DIRECTORY ${subdir_path})
		continue()
	endif()

	file(GLOB entries RELATIVE ${subdir_path} ${subdir_path}/*)

	foreach(entry IN LISTS entries)
		set(entry_path "${subdir_path}/${entry}")
		
		if(NOT IS_DIRECTORY ${entry_path})
			continue()
		endif()

		file(GLOB sources ${entry_path}/*.c)
		if(NOT sources)
			continue()
		endif()

		# Prepend the subdirectory to avoid target collisions (e.g. examples_demo_tests vs tests_demo_tests)
		set(target_name "${subdir}_${entry}_tests")
		set(run_target "${subdir}_run_${entry}")

		add_executable(${target_name} ${sources})
		target_include_directories(${target_name} PRIVATE ${CMAKE_SOURCE_DIR})
		target_compile_options(${target_name} PRIVATE ${CAIRO_FLAGS})
		target_link_libraries(${target_name} PRIVATE m)
		
		add_custom_target(${run_target}
			COMMAND ${target_name}
			DEPENDS ${target_name}
			WORKING_DIRECTORY ${entry_path}
			USES_TERMINAL
		)

		list(APPEND run_targets ${run_target})
	endforeach()
endforeach()

if(run_targets)
	add_custom_target(run DEPENDS ${run_targets})
endif()

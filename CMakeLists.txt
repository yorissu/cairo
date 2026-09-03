
cmake_minimum_required(VERSION 3.15)
project(cairo_examples C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CAIRO_FLAGS -Wall -Wextra -Werror -Wpedantic
	-Wshadow -Wundef -Wfloat-equal -Wdouble-promotion -Wformat=2
	-Wconversion -Wsign-conversion -Wcast-qual -Wcast-align=strict
	-Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes
	-Wwrite-strings -Wswitch-enum -Wswitch-default
	-Wnull-dereference -Winit-self -Wjump-misses-init -Wvla -Walloca
	-Wduplicated-cond -Wduplicated-branches -Wlogical-op
	-Wno-error=deprecated-declarations
)

set(run_targets "")
foreach(subdir IN ITEMS "examples")
	set(subdir_path "${CMAKE_SOURCE_DIR}/${subdir}")
	if(NOT IS_DIRECTORY ${subdir_path})
		continue()
	endif()

	file(GLOB entries RELATIVE ${subdir_path} CONFIGURE_DEPENDS ${subdir_path}/*)

	foreach(entry IN LISTS entries)
		set(entry_path "${subdir_path}/${entry}")
		if(NOT IS_DIRECTORY ${entry_path})
			continue()
		endif()

		file(GLOB sources CONFIGURE_DEPENDS ${entry_path}/*.c)
		if(NOT sources)
			continue()
		endif()

		set(name ${entry})
		if(TARGET ${name})
			message(WARNING
				"Skipping ${entry_path}: target '${name}' already exists "
				"(name collision across examples/ and tests/)."
			)
			continue()
		endif()

		add_executable(${name} ${sources})
		target_include_directories(${name} PRIVATE ${CMAKE_SOURCE_DIR})
		target_compile_options(${name} PRIVATE ${CAIRO_FLAGS})
		target_link_libraries(${name} PRIVATE m)

		# run_<name>: build if needed, then run with args from $ARGS in the
		# environment (read at run time, no reconfigure):
		#   ARGS="--verbose --seed 7" cmake --build build --target run_demo1
		add_custom_target(run_${name}
			COMMAND sh -c "exec \"$0\" $ARGS" $<TARGET_FILE:${name}>
			WORKING_DIRECTORY ${entry_path}
			USES_TERMINAL
			VERBATIM
		)
		add_dependencies(run_${name} ${name})
		list(APPEND run_targets run_${name})
	endforeach()
endforeach()

if(run_targets)
	add_custom_target(run DEPENDS ${run_targets})
endif()

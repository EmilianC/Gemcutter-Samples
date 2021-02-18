list(APPEND input_files
	"Game.cpp"
	"Game.h"
	"main.cpp"
	"Settings.cfg"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${input_files})

add_executable(input WIN32 ${input_files})
add_dependencies(input ASSETS)
sf_target_compile_warnings(input)
sf_target_compile_warnings_as_errors(input OPTIONAL)

target_include_directories(input PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(input PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(input PUBLIC gemcutter)

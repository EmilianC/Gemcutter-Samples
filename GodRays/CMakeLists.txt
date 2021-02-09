list(APPEND godrays_files
	"Game.cpp"
	"Game.h"
	"main.cpp"
	"Settings.cfg"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${godrays_files})

add_executable(godrays WIN32 ${godrays_files})
sf_target_compile_warnings(godrays)
sf_target_compile_warnings_as_errors(godrays OPTIONAL)

target_include_directories(godrays PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(godrays PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(godrays PUBLIC gemcutter)

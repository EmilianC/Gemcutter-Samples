list(APPEND shadows_files
	"Game.cpp"
	"Game.h"
	"main.cpp"
	"Settings.cfg"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${shadows_files})

add_executable(shadows WIN32 ${shadows_files})
sf_target_compile_warnings(shadows)
sf_target_compile_warnings_as_errors(shadows OPTIONAL)

target_include_directories(shadows PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(shadows PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(shadows PUBLIC gemcutter)

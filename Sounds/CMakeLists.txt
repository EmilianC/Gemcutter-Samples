list(APPEND sounds_files
	"Game.cpp"
	"Game.h"
	"main.cpp"
	"Settings.cfg"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sounds_files})

add_executable(sounds WIN32 ${sounds_files})
add_dependencies(sounds ASSETS)
sf_target_compile_warnings(sounds)
sf_target_compile_warnings_as_errors(sounds OPTIONAL)

target_include_directories(sounds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(sounds PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(sounds PUBLIC gemcutter)

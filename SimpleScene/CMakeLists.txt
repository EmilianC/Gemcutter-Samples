list(APPEND simple_scene_files
	"Game.cpp"
	"Game.h"
	"main.cpp"
	"Settings.cfg"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${simple_scene_files})

add_executable(simple_scene WIN32 ${simple_scene_files})
add_dependencies(simple_scene ASSETS)
sf_target_compile_warnings(simple_scene)
sf_target_compile_warnings_as_errors(simple_scene OPTIONAL)

target_include_directories(simple_scene PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(simple_scene PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(simple_scene PUBLIC gemcutter)

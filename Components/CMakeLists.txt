list(APPEND components_files
	"Boid.cpp"
	"Boid.h"
	"Flock.cpp"
	"Flock.h"
	"Game.cpp"
	"Game.h"
	"main.cpp"
	"Settings.cfg"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${components_files})

add_executable(components WIN32 ${components_files})
sf_target_compile_warnings(components)
sf_target_compile_warnings_as_errors(components OPTIONAL)

target_include_directories(components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(components PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(components PUBLIC gemcutter)

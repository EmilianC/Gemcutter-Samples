list(APPEND lighting_files
	"Game.cpp"
	"Game.h"
	"main.cpp"
	"Settings.cfg"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${lighting_files})

add_executable(lighting WIN32 ${lighting_files})
sf_target_compile_warnings(lighting)
sf_target_compile_warnings_as_errors(lighting OPTIONAL)

target_include_directories(lighting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(lighting PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(lighting PUBLIC gemcutter)

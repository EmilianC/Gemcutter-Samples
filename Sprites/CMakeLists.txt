list(APPEND sprites_files
	"Game.cpp"
	"Game.h"
	"main.cpp"
	"Settings.cfg"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${sprites_files})

add_executable(sprites WIN32 ${sprites_files})
sf_target_compile_warnings(sprites)
sf_target_compile_warnings_as_errors(sprites OPTIONAL)

target_include_directories(sprites PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(sprites PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(sprites PUBLIC gemcutter)

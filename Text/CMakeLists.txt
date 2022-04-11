list(APPEND text_files
	"Game.cpp"
	"Game.h"
	"main.cpp"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${text_files})

add_executable(text WIN32 ${text_files})
add_dependencies(text ASSETS)
sf_target_compile_warnings(text)
sf_target_compile_warnings_as_errors(text OPTIONAL)

target_include_directories(text PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(text PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_CURRENT_LIST_DIR}/../"
)

target_link_libraries(text PUBLIC gemcutter)

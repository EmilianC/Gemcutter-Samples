list(APPEND particles_files
	"Game.cpp"
	"Game.h"
	"main.cpp"
	"VelocityFunc.cpp"
	"VelocityFunc.h"
	"WaveFunc.cpp"
	"WaveFunc.h"
	"Settings.cfg"
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${particles_files})

add_executable(particles WIN32 ${particles_files})
add_dependencies(particles ASSETS)
sf_target_compile_warnings(particles)
sf_target_compile_warnings_as_errors(particles OPTIONAL)

target_include_directories(particles PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(particles PROPERTIES
	VS_DEBUGGER_WORKING_DIRECTORY
		"${CMAKE_CURRENT_LIST_DIR}"
)

target_link_libraries(particles PUBLIC gemcutter)

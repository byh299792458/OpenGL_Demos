cmake_minimum_required(VERSION 3.7)

project("OpenGL_Demos")

find_package(OpenGL REQUIRED)
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

configure_file(configuration/root_directory.h.in configuration/root_directory.h)
include_directories(${CMAKE_BINARY_DIR}/configuration)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_DEBUG_POSTFIX "_d")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

include_directories(external)
include_directories(external/glfw/include)
include_directories(external/gl3w/include)
include_directories(external/assimp/include)
include_directories(external/stb_image)
include_directories(util)

add_subdirectory(external/glm)
add_subdirectory(external/glfw)

aux_source_directory(util UTIL_SRC_LIST)
file(GLOB_RECURSE UTIL_HEADER_LIST util/*.hpp util/*.h)
set(UTIL_LIB_SRC_LIST 
	${UTIL_SRC_LIST} 
	${UTIL_HEADER_LIST}
	external/gl3w/include/GL/glcorearb.h
	external/gl3w/include/GL/gl3w.h
	external/gl3w/include/KHR/khrplatform.h
	external/gl3w/src/gl3w.c
	external/stb_image/stb_image.h
	external/stb_image/stb_image.cpp
	)
add_library(util_libs STATIC ${UTIL_LIB_SRC_LIST})

set(ALL_LIBS ${OPENGL_LIBRARIES}
	optimized util_libs debug util_libs_d 
	optimized glfw3 debug glfw3_d
	optimized glm_static debug glm_static_d 
	optimized ${PROJECT_SOURCE_DIR}/external/assimp/lib/IrrXML.lib
	debug ${PROJECT_SOURCE_DIR}/external/assimp/lib/IrrXML.lib
	optimized ${PROJECT_SOURCE_DIR}/external/assimp/lib/assimp-vc140-mt.lib
	debug ${PROJECT_SOURCE_DIR}/external/assimp/lib/assimp-vc140-mt.lib
)
link_directories(${CMAKE_SOURCE_DIR}/lib)

add_custom_target(CopyDLL ALL)
add_custom_command(TARGET CopyDLL
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
	COMMAND ${CMAKE_COMMAND} -E copy_if_different  
		${PROJECT_SOURCE_DIR}/external/assimp/bin/assimp-vc140-mt.dll 
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug
	COMMAND ${CMAKE_COMMAND} -E copy_if_different  
		${PROJECT_SOURCE_DIR}/external/assimp/bin/assimp-vc140-mt.dll 
		${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release
)

set(SOLUTION
	01-elastic_band
	02-shade_frequency
	03-mapping
	04-deferred_shading
	05-ssao
)

foreach(PROJECT ${SOLUTION})
	aux_source_directory(src/${PROJECT} SRC_LIST)
	file(GLOB_RECURSE HEADER_LIST src/${PROJECT}/*.hpp)
	file(GLOB_RECURSE VERT_SHADER_SRC_LIST src/${PROJECT}/*.vert)
	file(GLOB_RECURSE FRAG_SHADER_SRC_LIST src/${PROJECT}/*.frag)
	file(GLOB_RECURSE GEOM_SHADER_SRC_LIST src/${PROJECT}/*.geom)
	add_executable(${PROJECT} ${SRC_LIST} ${HEADER_LIST} 
		${VERT_SHADER_SRC_LIST}
		${FRAG_SHADER_SRC_LIST}
		${GEOM_SHADER_SRC_LIST})
	set_property(TARGET ${PROJECT} PROPERTY DEBUG_POSTFIX _d)
	target_link_libraries(${PROJECT} ${ALL_LIBS} ${ASSIMP_LIB})
	set(SRC_LIST)
endforeach(PROJECT)

cmake_minimum_required(VERSION 2.8.12)
project(libresample)

include(CheckIncludeFiles)
include(CheckTypeSize)

check_type_size("void *" POINTER_SIZE)

check_include_files(inttypes.h HAVE_INTTYPES_H)

find_library(HAVE_LIBM m)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/configtemplate.h ${CMAKE_CURRENT_BINARY_DIR}/src/config.h)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/libresample.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libresample.pc)

if( BUILD_SHARED_RESAMPLE )
	add_library(resample SHARED src/filterkit.c src/resample.c src/resamplesubs.c)

	set_target_properties(resample PROPERTIES VERSION 1.0 SOVERSION 1)
	set_target_properties(resample PROPERTIES CLEAN_DIRECT_OUTPUT 1)

	install(TARGETS resample LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)
else()
	add_library(resample STATIC src/filterkit.c src/resample.c src/resamplesubs.c)
	set_target_properties(resample PROPERTIES OUTPUT_NAME "resample")
	set_target_properties(resample PROPERTIES CLEAN_DIRECT_OUTPUT 1)
endif()


target_include_directories(resample
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/libresample
	PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src
	)


#install(FILES include/libresample.h DESTINATION include)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libresample.pc DESTINATION lib/pkgconfig)

if( ENABLE_RESAMPLE_TEST )
	enable_testing()
	add_subdirectory(tests)
endif()

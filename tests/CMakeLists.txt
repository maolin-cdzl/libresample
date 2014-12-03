enable_testing()

include(FindPkgConfig)

pkg_check_modules(SNDFILE sndfile)
pkg_check_modules(SAMPLERATE samplerate)

add_executable(testresample testresample.c)
target_include_directories(testresample 
	PRIVATE $<TARGET_PROPERTY:resample,INTERFACE_INCLUDE_DIRECTORIES>
	)
target_link_libraries(testresample resample m)
add_test(testresample testresample DEPENDS resample)

if(SNDFILE_FOUND)
	add_executable(resample-sndfile resample-sndfile.c)
	target_include_directories(resample-sndfile
		PRIVATE $<TARGET_PROPERTY:resample,INTERFACE_INCLUDE_DIRECTORIES>
		)
	target_link_libraries(resample-sndfile resample sndfile m)
	set_target_properties(resample-sndfile PROPERTIES COMPILE_FLAGS "${SNDFILE_CFLAGS}" LINK_FLAGS "${SNDFILE_LDFLAGS}")
	install(TARGETS resample-sndfile RUNTIME DESTINATION bin)
endif(SNDFILE_FOUND)

if(SAMPLERATE_FOUND)
	add_executable(compareresample compareresample.c)
	target_include_directories(compareresample 
		PRIVATE $<TARGET_PROPERTY:resample,INTERFACE_INCLUDE_DIRECTORIES>
		)
	target_link_libraries(compareresample resample samplerate m)
	set_target_properties(compareresample PROPERTIES COMPILE_FLAGS "${SAMPLERATE_CFLAGS}" LINK_FLAGS "${SAMPLERATE_LDFLAGS}")
	add_test(compareresample compareresample DEPENDS resample)
endif(SAMPLERATE_FOUND)


add_executable(resampler resampler.cc)
target_include_directories(resampler
	PRIVATE $<TARGET_PROPERTY:resample,INTERFACE_INCLUDE_DIRECTORIES>
	)
target_link_libraries(resampler PRIVATE resample)


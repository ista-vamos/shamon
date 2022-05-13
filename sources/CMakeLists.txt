if (DYNAMORIO_SOURCES)
       add_subdirectory(drfun)
       #add_subdirectory(drsyscalls)
endif()

add_executable(regex regex.c)
target_compile_definitions(regex PRIVATE -D_POSIX_C_SOURCE=200809L)
target_include_directories(regex PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(regex PRIVATE shamon-shmbuf shamon-signature)


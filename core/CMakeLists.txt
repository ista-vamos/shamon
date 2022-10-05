include_directories(${CMAKE_SOURCE_DIR})

add_library(shamon-list OBJECT list.c)
add_library(shamon-vector OBJECT vector.c)
add_library(shamon-queue-spsc OBJECT queue_spsc.c)

add_library(shamon-source OBJECT source.c)
add_library(shamon-monitor STATIC monitor.c)

target_compile_options(shamon-source PRIVATE -fPIC)
target_link_libraries(shamon-source PRIVATE shamon-signature)

add_library(shamon-utils STATIC utils.c)
target_compile_definitions(shamon-utils PUBLIC -D_POSIX_C_SOURCE=200809L)
target_compile_options(shamon-utils PRIVATE -fPIC)

add_library(shamon-arbiter STATIC shamon.c arbiter.c event.c stream.c)
target_compile_options(shamon-arbiter PRIVATE -fPIC)
target_compile_definitions(shamon-arbiter PUBLIC -D_POSIX_C_SOURCE=200809L)

add_library(shamon-signature STATIC signatures.c)
target_compile_options(shamon-signature PRIVATE -fPIC)

add_library(shamon-ringbuf OBJECT spsc_ringbuf.c)
add_library(shamon-parallel-queue STATIC par_queue.c)
target_link_libraries(shamon-parallel-queue PUBLIC shamon-ringbuf)

add_library(shamon STATIC shamon.c)
add_library(shamon-lib SHARED arbiter.c event.c stream.c)
target_compile_definitions(shamon-lib PUBLIC -D_POSIX_C_SOURCE=200809L)
target_link_libraries(shamon-lib PUBLIC shamon-parallel-queue
		                 PUBLIC shamon-signature
		                 PUBLIC shamon-shmbuf)

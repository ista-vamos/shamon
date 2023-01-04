add_library(drregex SHARED regex.c)
target_link_libraries(drregex shamon-shmbuf shamon-list shamon-signature)
target_compile_options(drregex PUBLIC -Wno-pedantic -Wno-missing-field-initializers)

use_DynamoRIO_extension(drregex drmgr)
configure_DynamoRIO_client(drregex)

include_directories("${CMAKE_SOURCE_DIR}")
include_directories("${CMAKE_SOURCE_DIR}/shmbuf")
include_directories("${CMAKE_SOURCE_DIR}/streams")


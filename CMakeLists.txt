cmake_minimum_required(VERSION 3.10.2)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(cr2-decode CXX C)

SET (SRC_DIR ./src)
SET (INCLUDE_DIR ./include)

SET (SRC_FILES
	${SRC_DIR}/CR2Decoder.cpp
	${SRC_DIR}/CR2File.cpp
	)

add_library(libcr2-decode ${SRC_FILES})

target_include_directories(libcr2-decode PUBLIC ${INCLUDE_DIR})

add_executable(cr2-decode ${SRC_DIR}/main.cpp)

target_link_libraries(cr2-decode libcr2-decode)


# use this command to enable debugging symbols
# cmake -DCMAKE_BUILD_TYPE=Debug ..

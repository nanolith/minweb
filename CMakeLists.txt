PROJECT(minweb)

cmake_minimum_required(VERSION 3.10)

INCLUDE(CheckSymbolExists)

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

find_package(minunit REQUIRED)

INCLUDE_DIRECTORIES(include)
AUX_SOURCE_DIRECTORY(src/minweave MINWEB_MINWEAVE_SOURCES)
AUX_SOURCE_DIRECTORY(src/mintangle MINWEB_MINTANGLE_SOURCES)
AUX_SOURCE_DIRECTORY(src/lexer MINWEB_LEXER_SOURCES)
AUX_SOURCE_DIRECTORY(src/processor MINWEB_PROCESSOR_SOURCES)
AUX_SOURCE_DIRECTORY(src/util MINWEB_UTIL_SOURCES)
AUX_SOURCE_DIRECTORY(test MINWEB_TEST_SOURCES)

ADD_LIBRARY(minweb_lexer STATIC ${MINWEB_LEXER_SOURCES})
TARGET_COMPILE_OPTIONS(minweb_lexer PRIVATE -O2)
ADD_LIBRARY(minweb_processor STATIC ${MINWEB_PROCESSOR_SOURCES})
TARGET_COMPILE_OPTIONS(minweb_processor PRIVATE -O2)
ADD_LIBRARY(minweb_util STATIC ${MINWEB_UTIL_SOURCES})
TARGET_COMPILE_OPTIONS(minweb_util PRIVATE -O2)

ADD_EXECUTABLE(testminweb
    ${MINWEB_LEXER_SOURCES} ${MINWEB_PROCESSOR_SOURCES} ${MINWEB_TEST_SOURCES}
    ${MINWEB_UTIL_SOURCES})
TARGET_COMPILE_OPTIONS(testminweb PRIVATE --coverage ${MINUNIT_CFLAGS})
TARGET_LINK_LIBRARIES(testminweb PRIVATE --coverage ${MINUNIT_LDFLAGS})

ADD_CUSTOM_COMMAND(TARGET testminweb
    POST_BUILD
    COMMAND testminweb)

ADD_EXECUTABLE(minweave
    ${MINWEB_MINWEAVE_SOURCES})
TARGET_COMPILE_OPTIONS(minweave PRIVATE -O2)
TARGET_LINK_LIBRARIES(minweave PRIVATE minweb_processor minweb_lexer)

ADD_EXECUTABLE(mintangle
    ${MINWEB_MINTANGLE_SOURCES})
TARGET_COMPILE_OPTIONS(mintangle PRIVATE -O2)
TARGET_LINK_LIBRARIES(mintangle
    PRIVATE minweb_processor minweb_lexer minweb_util)

#Build a pkg-config file
SET(MINWEB_PC "${CMAKE_BINARY_DIR}/minweb.pc")
FILE(WRITE  ${MINWEB_PC} "Name: minweb")
FILE(APPEND ${MINWEB_PC} "\nDescription: minweb literate programming system")
FILE(APPEND ${MINWEB_PC} "\nVersion: 0.1")
FILE(APPEND ${MINWEB_PC} "\nURL: https://github.com/nanolith/minweb")
FILE(APPEND ${MINWEB_PC} "\nprefix=${CMAKE_INSTALL_PREFIX}")
FILE(APPEND ${MINWEB_PC} "\nlibdir=\${prefix}/lib")
FILE(APPEND ${MINWEB_PC} "\nincludedir=\${prefix}/include")
FILE(APPEND ${MINWEB_PC} "\nLibs: -L\${libdir} -lminweb_lexer")
FILE(APPEND ${MINWEB_PC} " -lminweb_processor")
FILE(APPEND ${MINWEB_PC} "\nCflags: -I\${includedir}")
INSTALL(FILES ${MINWEB_PC} DESTINATION lib/pkgconfig)

#Install headers
FILE(GLOB MINWEB_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/include/minweb/*.h")
INSTALL(FILES ${MINWEB_INCLUDES} DESTINATION include/minweb)

#Install libraries
INSTALL(TARGETS minweb_lexer minweb_processor
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

#Install binaries
INSTALL(TARGETS minweave
        RUNTIME DESTINATION bin)

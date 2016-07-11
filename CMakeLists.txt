cmake_minimum_required(VERSION 2.8)
include(ExternalProject)
project(whisper-core)

# ------------------------------------------------------------------------------
# Add whisper-protocol as external project -------------------------------------
set(jnxlibc_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/jnxlibc")
set(jnxlibc_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
set(jnxlibc_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${jnxlibc_INSTALL_DIR} -DIGNORE_VERSION=1)

ExternalProject_Add(jnxlibc-project
  PREFIX ${jnxlibc_PREFIX}
  GIT_REPOSITORY https://github.com/AlexsJones/jnxlibc.git
  INSTALL_DIR ${jnxlibc_INSTALL_DIR}
  CMAKE_ARGS ${jnxlibc_CMAKE_ARGS}
  BUILD_COMMAND make
  INSTALL_COMMAND sudo make install
  )

set(whisper-protocol_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/whisper-protocol")
set(whisper-protocol_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
set(whisper-protocol_CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${whisper-protocol_INSTALL_DIR})

ExternalProject_Add(whisper-protocol-project
  PREFIX ${whisper-protocol_PREFIX}
  GIT_REPOSITORY https://github.com/AlexsJones/whisper-protocol.git
  INSTALL_DIR ${whisper-protocol_INSTALL_DIR}
  CMAKE_ARGS ${whisper-protocol_CMAKE_ARGS}
  BUILD_COMMAND make
  INSTALL_COMMAND sudo make install
  )
# -------------------------------------------------------------------------------

add_subdirectory(src/crypto)
add_subdirectory(src/data)
add_subdirectory(src/integrity)
add_subdirectory(src/net)
add_subdirectory(src/session)
add_subdirectory(src/util)

include_directories(${PROJECT_INC_DIR}src/crypto)
include_directories(${PROJECT_INC_DIR}src/data)
include_directories(${PROJECT_INC_DIR}src/integrity)
include_directories(${PROJECT_INC_DIR}src/net)
include_directories(${PROJECT_INC_DIR}src/util)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set (CMAKE_C_FLAGS "-Wno-deprecated-declarations -Wno-pointer-sign -Wno-switch -Wno-implicit-function-declaration")
endif()

execute_process(COMMAND ./tools/generate_protobuf)
file(COPY ${HEADERS} DESTINATION whisper-core)

add_library(whisper-protocol SHARED IMPORTED)
add_library(whisper-protocols SHARED IMPORTED)
set_property(TARGET whisper-protocol PROPERTY IMPORTED_LOCATION ${whisper-protocol_INSTALL_DIR}/lib/libwhisper-protocol.so)
set_property(TARGET whisper-protocols PROPERTY IMPORTED_LOCATION ${whisper-protocol_INSTALL_DIR}/lib/libwhisper-protocols.so)

add_library(jnxlibc SHARED IMPORTED)
add_library(jnxlibcs STATIC IMPORTED)

set_property(TARGET jnxlibc PROPERTY IMPORTED_LOCATION ${jnxlibc_INSTALL_DIR}/lib/libjnxc.so)
set_property(TARGET jnxlibcs PROPERTY IMPORTED_LOCATION ${jnxlibc_INSTALL_DIR}/lib/libjnxcs.a)


add_library(whisper-core-static STATIC ${SOURCE})
add_library(whisper-core SHARED ${SOURCE})

add_dependencies(whisper-core whisper-protocol-project)

target_link_libraries(whisper-core jnxlibc -lcrypto -lssl -g -lprotobuf-c whisper-protocol -fPIC -lpthread ${LINKER_LIBRARIES})
target_link_libraries(whisper-core-static jnxlibcs -lcrypto -lssl -g -lprotobuf-c  whisper-protocols -fPIC -lpthread ${LINKER_LIBRARIES})
install(TARGETS whisper-core-static DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS whisper-core DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(DIRECTORY whisper-core DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

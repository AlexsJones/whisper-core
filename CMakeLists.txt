cmake_minimum_required(VERSION 2.8)
set(INSTALL_PREFIX /usr)
include(ExternalProject)
project(whisper-core)

# ------------------------------------------------------------------------------
# Add whisper-protocol as external project -------------------------------------
set(whisper-protocol_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/whisper-protocol")
set(whisper-protocol_INSTALL_DIR "${CMAKE_CURRENT_BINARY_DIR}/whisper-protocol")
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

add_library(whisper-core-static STATIC ${SOURCE})
add_library(whisper-core SHARED ${SOURCE})
target_link_libraries(whisper-core -ljnxc -lcrypto -lssl -g -lprotobuf-c -lwhisper-protocol -fPIC -lpthread ${LINKER_LIBRARIES})
target_link_libraries(whisper-core-static -ljnxc -lcrypto -lssl -g -lprotobuf-c  -lwhisper-protocol -fPIC -lpthread ${LINKER_LIBRARIES})
install(TARGETS whisper-core-static DESTINATION ${INSTALL_PREFIX}/lib)
install(TARGETS whisper-core DESTINATION ${INSTALL_PREFIX}/lib)
install(DIRECTORY whisper-core DESTINATION ${INSTALL_PREFIX}/include)

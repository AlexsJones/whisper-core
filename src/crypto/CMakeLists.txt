set(SOURCE ${SOURCE}
${CMAKE_CURRENT_SOURCE_DIR}/cryptography.c
PARENT_SCOPE)

set(HEADERS
${HEADERS}
${CMAKE_CURRENT_SOURCE_DIR}/cryptography.h
PARENT_SCOPE)

set(LINKER_LIBRARIES
  ${LINKER_LIBRARIES}
  PARENT_SCOPE)

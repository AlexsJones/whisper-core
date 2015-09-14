set(SOURCE ${SOURCE}
${CMAKE_CURRENT_SOURCE_DIR}/auth_initiator.pb-c.c
${CMAKE_CURRENT_SOURCE_DIR}/auth_invite.pb-c.c
${CMAKE_CURRENT_SOURCE_DIR}/auth_joiner.pb-c.c
${CMAKE_CURRENT_SOURCE_DIR}/auth_receiver.pb-c.c
${CMAKE_CURRENT_SOURCE_DIR}/secure_comms_object.pb-c.c
${CMAKE_CURRENT_SOURCE_DIR}/protobuf_construction.c
${CMAKE_CURRENT_SOURCE_DIR}/session.c
${CMAKE_CURRENT_SOURCE_DIR}/session_service.c
PARENT_SCOPE)

set(HEADERS
${HEADERS}
${CMAKE_CURRENT_SOURCE_DIR}/auth_initiator.pb-c.h
${CMAKE_CURRENT_SOURCE_DIR}/auth_invite.pb-c.h
${CMAKE_CURRENT_SOURCE_DIR}/auth_joiner.pb-c.h
${CMAKE_CURRENT_SOURCE_DIR}/auth_receiver.pb-c.h
${CMAKE_CURRENT_SOURCE_DIR}/secure_comms_object.pb-c.h
${CMAKE_CURRENT_SOURCE_DIR}/protobuf_construction.h
${CMAKE_CURRENT_SOURCE_DIR}/session.h
${CMAKE_CURRENT_SOURCE_DIR}/session_service.h
PARENT_SCOPE)

set(LINKER_LIBRARIES
  ${LINKER_LIBRARIES}
  PARENT_SCOPE)

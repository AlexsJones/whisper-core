/*********************************************************************************
 *     File Name           :     test/unit_test/test_protobuf.c
 *     Created By          :     tibbar
 *     Creation Date       :     [2015-08-04 11:59]
 *     Last Modified       :     [2015-08-04 12:11]
 *     Description         :      
 **********************************************************************************/
#include "auth_comms.h"
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include "cryptography.h"
#include "utils.h"
#include "auth_initiator.pb-c.h"
#include "auth_receiver.pb-c.h"
#include "auth_joiner.pb-c.h"
#include "auth_invite.pb-c.h"
#include "handshake_control.h"
#include "secure_comms.h"

void test_auth_initiator() {
  AuthInitiator auth_parcel = AUTH_INITIATOR__INIT;

  /* fake initiator guid */
  jnx_size len = strlen("Fake Guid");
  auth_parcel.initiator_guid = malloc(sizeof(char) * len + 1);
  memcpy(auth_parcel.initiator_guid,"Fake Guid",len);
  /* bool flags */
  auth_parcel.is_requesting_public_key = 1;
  auth_parcel.is_requesting_finish = 0;
  /*Fake shared secret */
  jnx_uint8 *shared_secret = "BinaryData";
  jnx_size secret_len = strlen("BinaryData");  
  auth_parcel.shared_secret.len = secret_len;
  auth_parcel.shared_secret.data = malloc(sizeof(char) * 
      auth_parcel.shared_secret.len);

  jnx_int i;
  for(i=0;i<auth_parcel.shared_secret.len;++i) {
    auth_parcel.shared_secret.data[i] = shared_secret[i];
  }
  /* public key */
  jnx_char *pub_key_str = "Fak public key";
  jnx_size pub_len = strlen(pub_key_str);
  auth_parcel.initiator_public_key = malloc(sizeof(char*) * pub_len);
  memcpy(auth_parcel.initiator_public_key,pub_key_str,pub_len);

  /*session guid */
  jnx_char *session_guid_str = "Fake session guid str";
  jnx_size session_guid_len = strlen(session_guid_str);
  auth_parcel.session_guid = malloc(sizeof(char*) * session_guid_len + 1);
  memcpy(auth_parcel.session_guid,session_guid_str,session_guid_len + 1);

  /* secure comms port */
  jnx_char *secure_comms_port = "comms port blah";
  jnx_size secure_comms_port_len = strlen(secure_comms_port);
  auth_parcel.secure_comms_port = malloc(sizeof(char*) * secure_comms_port_len + 1);
  bzero(auth_parcel.secure_comms_port,secure_comms_port_len +1);
  memcpy(auth_parcel.secure_comms_port,secure_comms_port,secure_comms_port_len + 1);

  /* packing */
  jnx_int parcel_len = auth_initiator__get_packed_size(&auth_parcel);
  jnx_uint8 *obuffer = malloc(parcel_len);
  auth_initiator__pack(&auth_parcel,obuffer);

  JNXLOG(LDEBUG,"Unpacking auth_initiator");

  /* unpack */

  AuthInitiator *au = auth_initiator__unpack(NULL,parcel_len,obuffer);

  JNXCHECK(strcmp(au->session_guid,auth_parcel.session_guid) == 0);
  
  JNXCHECK(strcmp(au->initiator_public_key,auth_parcel.initiator_public_key) == 0);

  JNXCHECK(strcmp(au->secure_comms_port,auth_parcel.secure_comms_port) == 0);

  JNXCHECK(au->shared_secret.len == auth_parcel.shared_secret.len);

}

int main(int argc, char **argv) {
  JNXLOG_CREATE("../testlogger.conf");
  JNXLOG(LDEBUG,"Testing protobuf packing on initiator");  
  test_auth_initiator();  
  JNXLOG_DESTROY();
  return 0;
}

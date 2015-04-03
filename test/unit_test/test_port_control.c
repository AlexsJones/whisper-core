/*********************************************************************************
 *     File Name           :     test/unit_test/test_port_control.c
 *     Created By          :     tibbar
 *     Creation Date       :     [2015-04-03 05:30]
 *     Last Modified       :     [2015-04-03 13:47]
 *     Description         :      
 **********************************************************************************/
#include "port_control.h"
#include <jnxc_headers/jnxlog.h>
#define DEFAULT_SINGLE_PORT 9013
#define DEFAULT_SINGLE_PORT_NEXT 9014
void test_single_port() {
  int is_available = is_port_available(DEFAULT_SINGLE_PORT);
  JNXCHECK(is_available == 1);
  is_available = is_port_available(-1);
  JNXCHECK(is_available == 0);
  is_available = is_port_available(DEFAULT_SINGLE_PORT);
  JNXCHECK(is_available == 1);
}
void test_port_control_scan() {
  port_control *p = port_control_create(8001,9010,2);
  port_control_scan_range(p,FULL_SCAN);
  port_control_destroy(&p);
}
void test_port_control_next_available() {
  int is_available = is_port_available(DEFAULT_SINGLE_PORT);
  JNXCHECK(is_available == 1);

  port_control *p = port_control_create(DEFAULT_SINGLE_PORT,DEFAULT_SINGLE_PORT_NEXT,1);

  jnx_char *s = port_control_next_available_to_s(p);

  JNXCHECK(strcmp("9013",s)==0);

  port_control_destroy(&p);
}
int main(int argc, char **argv) {
  JNX_LOG(NULL,"Test port control service");
  test_single_port();
  JNX_LOG(NULL,"Test port control scanning");
  test_port_control_scan();
  JNX_LOG(NULL,"Test port control scan next available");
  test_port_control_next_available();
  return 0;
}


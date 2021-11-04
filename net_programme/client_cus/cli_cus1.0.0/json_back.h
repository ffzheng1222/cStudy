


#ifndef __JSON_BACK_H_
#define __JSON_BACK_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include </usr/local/include/json/json.h>

#define TEST_DEBUG

#define SELF_PRO_BUF 128

struct info{
      int order_num;
      int src_id;
      int drc_id;
      int CMD;
      char buf[SELF_PRO_BUF];
};

extern void back_on_json_max(struct info buf_info, const char **my_str);
extern void back_up_json_max(struct info *buf_info, const char **my_str);


#endif

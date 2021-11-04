 


#include "json_back.h"


/*json 的打包处理*/
void back_on_json_max(struct info buf_info, const char **my_str)
{
      struct json_object *json_test, *src_id, *drc_id, *order_num, *j_buf, *cmd;
      json_test = json_object_new_object();
      src_id = json_object_new_int(buf_info.src_id);
      drc_id = json_object_new_int(buf_info.drc_id);
      order_num = json_object_new_int(buf_info.order_num);
      cmd = json_object_new_int(buf_info.CMD);
      j_buf = json_object_new_string(buf_info.buf);

      json_object_object_add(json_test, "src_id", src_id);
      json_object_object_add(json_test, "drc_id", drc_id);
      json_object_object_add(json_test, "order_num", order_num);
      json_object_object_add(json_test, "CMD", cmd);
      json_object_object_add(json_test, "buf", j_buf);
      *my_str = json_object_to_json_string(json_test);
}



/*json 的拆包处理*/
void back_up_json_max(struct info *buf_info, const char ** my_str)
{
      json_object *jiebao_json, *jiebao_src_id, *jiebao_drc_id, *jiebao_order_num, *jiebao_j_buf, *jiebao_cmd;
      //把字符串转换成JSON对象
      jiebao_json = json_tokener_parse(*my_str);
      //通过键值获取JSON成员
      jiebao_src_id = json_object_object_get(jiebao_json,"src_id");
      jiebao_drc_id = json_object_object_get(jiebao_json,"drc_id");
      jiebao_order_num = json_object_object_get(jiebao_json,"order_num");
      jiebao_j_buf = json_object_object_get(jiebao_json,"buf");
      jiebao_cmd = json_object_object_get(jiebao_json,"CMD");
      //还原
      //int jiebao_str; 
      buf_info->src_id = json_object_get_int(jiebao_src_id);
      buf_info->drc_id = json_object_get_int(jiebao_drc_id);
      buf_info->order_num = json_object_get_int(jiebao_order_num);
      buf_info->CMD = json_object_get_int(jiebao_cmd);
     

      const char *test_buf = json_object_get_string(jiebao_j_buf);
      strncpy(buf_info->buf,test_buf,strlen(test_buf)+1);



#ifdef TEST_DEBUG
      printf("src_id :%d\n", buf_info->src_id);
      printf("drc_id :%d\n", buf_info->drc_id);
      printf("order_num :%d\n", buf_info->order_num);
      printf("buf :%s\n", buf_info->buf);
      printf("cmd :%d\n", buf_info->CMD);
#endif

}


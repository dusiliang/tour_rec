#ifndef __PROCESS_DATA_H__
#define __PROCESS_DATA_H__
#include <vector>
#include "data_struct.h"

void match_msg(mid_word_map_t &mwm,
               map<int, line_t> &line_map,
               mid_lid_map_t &mlm);

void do_filter_by_user(uid_user_map_t &uum,
                       map<int, line_t> &line_map,
                       vector<msg_t> &msg_rank);

#endif

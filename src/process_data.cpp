#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "ICTCLAS50.h"
#include "log.h"
#include "data_struct.h"

using namespace std;

#define SPLIT_SYMBLE " "

void match_msg(mid_word_map_t &mwm,
               map<int, line_t> &line_map,
               mid_lid_map_t &mlm) {
    long mid;
    //for each message
    for (mid_word_map_t::iterator mit = mwm.begin();
         mit != mwm.end(); ++mit) {
        mid = mit->first;
        set<int> tmp_set;
        //for each place mentioned in one message
        for (vector<string>::iterator it = mit->second.begin();
            it != mit->second.end(); ++it) {
            //for each line
            for (map<int, line_t>::iterator lit = line_map.begin();
                 lit != line_map.end(); ++lit) {
                //for each destination in one line
                for (vector<string>::iterator dit = lit->second.des.begin();
                     dit != lit->second.des.end(); ++dit) {
                    if ((*dit).find(*it) != string::npos) {
                        tmp_set.insert(lit->second.lid);
                        break;
                    }
                }
            }
        }
        mlm[mid] = tmp_set;
    }
}

void do_filter_by_user(uid_user_map_t &uum,
                       map<int, line_t> &line_map,
                       vector<msg_t> &msg_rank) {
    //set<int>::iterator it_del;
    for (vector<msg_t>::iterator mit = msg_rank.begin();
         mit != msg_rank.end(); ++mit) {
        if (mit->lid != NULL) {
            for (set<int>::iterator lit = mit->lid->begin();
                 lit != mit->lid->end(); ) {
                if (uum[mit->uid].home.find(line_map[*lit].home) == string::npos)
                    mit->lid->erase(lit++);
                else
                    ++lit;
            }
        }
    }
}

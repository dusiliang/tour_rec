#ifndef __DATA_STRUCT_H__
#define __DATA_STRUCT_H__
#include <set>

using namespace std;

struct msg_t {
    long mid;
    long uid;
    double score;
    set<int> *lid;
    string content;
    string ns_word;
};

struct line_t {
    int lid;
    string home;
    vector<string> des;
};

struct user_t {
    long uid;
    string name;
    string home;
    //vector<long> mids;
};

typedef map<long, user_t> uid_user_map_t;
typedef map<long, string> mid_content_map_t;
typedef map<long, vector<string> > mid_word_map_t;
typedef map<long, set<int> > mid_lid_map_t;
typedef map<long, string> uid_home_map_t;

#endif

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
#include "process_data.h"

using namespace std;

#define SPLIT_SYMBLE " "

static const string INIT_DIR = "/home/sldu/cpp/ictclas/api/";
static const string DATA_DIR = "/home/sldu/cpp/ictclas/data/";
static const string LOGS_DIR = "/home/sldu/cpp/ictclas/logs/";

static bool compare_msg(msg_t m1, msg_t m2) {
    return m1.score > m2.score;
}

int load_file(const string &filename,
        uid_user_map_t &uum,
        uid_home_map_t &uhm,
        mid_content_map_t &mcm,
        log_t *log) {
    ifstream ifile;
    string line;
    string content;
    string home;
    size_t pos1, pos2;
    long uid, mid;
    int num = 0;

    ifile.open(filename.c_str());
    if (ifile.is_open()) {
        while (getline(ifile, line)) {
            //mid uid provience city content
            pos1 = line.find(SPLIT_SYMBLE);
            if (pos1 != string::npos) {
                mid = atol(line.substr(0, pos1).c_str());
            } else {
                continue;
            }
            pos2 = line.find(SPLIT_SYMBLE, pos1+1);
            if (pos2 != string::npos) {
                uid = atol(line.substr(pos1+1, pos2-pos1).c_str());
                pos1 = pos2 + 1;
            } else {
                continue;
            }
            pos2 = line.find(SPLIT_SYMBLE, pos1);
            if (pos2 != string::npos) {
                home = line.substr(pos1, pos2-pos1);
                pos1 = pos2 + 1;
            } else {
                continue;
            }
            pos2 = line.find(SPLIT_SYMBLE, pos1);
            if (pos2 != string::npos) {
                home.append(line.substr(pos1, pos2-pos1));
                content = line.substr(pos2+1);
            } else {
                continue;
            }

            if (uum.find(uid) == uum.end()) {
                vector<long> tmp_vec(1, mid);
                user_t tmp = {uid, home, tmp_vec};
                uum[uid] = tmp;
            } else {
                uum[uid].mids.push_back(mid);
            }
            mcm[mid] = content;
            ++num;
        }
        ifile.close();
    } else {
        error(log, "open raw file %s error", filename.c_str());
    }
    return num;
}

inline bool is_ns(const string &str) {
    if (str.find("ns") != string::npos)
        return true;
    else
        return false;
}

int parse_content(mid_content_map_t &mcm,
        mid_word_map_t &mwm,
        log_t *log) {
    char buf[2048];
    string result;

    for (mid_content_map_t::iterator it = mcm.begin(); it != mcm.end(); ++it) {
        size_t pos1, pos2 = 0;
        if (ICTCLAS_ParagraphProcess(it->second.c_str(),
                                     it->second.length(),
                                     buf,
                                     CODE_TYPE_UNKNOWN,
                                     1)
            ) {
            result = buf;
            if (mwm.find(it->first) == mwm.end()) {
                vector<string> tmp;
                mwm[it->first] = tmp;
            }
            while ((pos1 = result.find(" ", pos2)) != string::npos) {
                string word = result.substr(pos2, pos1-pos2);
                if (is_ns(word)) {
                    mwm[it->first].push_back(word.substr(0, word.find("/")));
                }
                pos2 = pos1 + 1;
            }
        } else {
            error(log, "message %ld process error!", it->first);
        }
    }

    return 0;
}

void rank_user(mid_content_map_t &mcm,
        uid_user_map_t &uum,
        mid_word_map_t &mwm,
        vector<msg_t> &msg_rank) {
    for (uid_user_map_t::iterator uit = uum.begin();
         uit != uum.end(); ++uit) {
        for (vector<long>::iterator mit = uit->second.mids.begin();
             mit != uit->second.mids.end(); ++mit) {
            msg_t msg = {*mit, uit->first, 0, NULL, mcm[*mit], ""};
            mid_word_map_t::iterator it = mwm.find(*mit);
            if (it != mwm.end()) {
                msg.score = it->second.size();
#if 0
                for (vector<string>::iterator it_word = it->second.begin();
                     it_word != it->second.end(); ++it_word) {
                    msg.ns_word.append(*it_word);
                }
#endif
            }
            if (msg.score > 0)
                msg_rank.push_back(msg);
        }
    }
}

void out_put(vector<msg_t> &msg_rank,
             const string &path,
             const string &filename) {
    ofstream ofile;
    ofile.open((path+filename).c_str());
    if (ofile.is_open()) {
        for (vector<msg_t>::iterator it = msg_rank.begin();
             it != msg_rank.end(); ++it) {
            if ((it->lid == NULL) || (it->lid->size() == 0))
    	        continue;
            ofile << it->mid << " " << it->uid;
            for (set<int>::iterator sit = it->lid->begin();
                 sit != it->lid->end(); ++sit) {
                  ofile << " " << *sit;
            }
            ofile << endl;
        }
        ofile.close();
    }
}

int load_line(const string &file, map<int, line_t> &line_map) {
    ifstream ifile;
    int lid;
    int num = 0;
    string home;
    ifile.open(file.c_str());
    if (ifile.is_open()) {
        string line;
        while (getline(ifile, line)) {
            vector<string> des_vec;
            size_t pos1, pos2 = 0;
            pos1 = line.find(SPLIT_SYMBLE);
            lid = atoi(line.substr(0, pos1).c_str());
            ++pos1;
            pos2 = line.find(SPLIT_SYMBLE, pos1);
            home = line.substr(pos1, pos2-pos1);
            pos1 = pos2 + 1;

            while ((pos2 = line.find(SPLIT_SYMBLE, pos1)) != string::npos) {
                des_vec.push_back(line.substr(pos1, pos2-pos1));
                pos1 = pos2 + 1;
            }
            des_vec.push_back(line.substr(pos1));
            line_t tmp = {lid, home, des_vec};
            line_map[lid] = tmp;
            ++num;
        }
        ifile.close();
    }
    return num;
}

void show_line(vector<line_t> &lines) {
    for (vector<line_t>::iterator it = lines.begin();
         it != lines.end(); ++it) {
        cout << it->lid << " " << it->home << " to:";
        for (vector<string>::iterator itt = it->des.begin();
             itt != it->des.end(); ++itt) {
            cout << " " << *itt;
        }
        cout << endl;
    }
}

void fill_result(mid_lid_map_t &mlm,
                 vector<msg_t> &msg_rank) {
    for (vector<msg_t>::iterator it = msg_rank.begin();
         it != msg_rank.end(); ++it) {
        if (mlm[it->mid].size() > 0)
            it->lid = &mlm[it->mid];
    }
}

int main(int argc, char **argv) {
    int num;
    string raw_file(DATA_DIR+"raw.txt");
    string line_file(DATA_DIR+"line_keyword.txt");
    string out_file(DATA_DIR+"parse_data.txt");
    uid_user_map_t uid_user_map;
    mid_content_map_t mid_content_map;
    mid_word_map_t mid_word_map;
    mid_lid_map_t mid_lid_map;      //message id to line id
    uid_home_map_t uid_home_map;  //user id to home
    vector<msg_t> msg_rank;
    map<int, line_t> line_map;

    if (argc == 4) {
        raw_file = argv[1];
        line_file = argv[2];
        out_file = argv[3];
    } else {
        cout << "usage: " << argv[0] << " weibo_msg_file line_file output_file"
             << endl;
        return -1;
    }

    log_t *log = create_log((LOGS_DIR+"parse_data").c_str());
    info(log, "parse_data start!");

    if (!ICTCLAS_Init(INIT_DIR.c_str())) {
        error(log, "ICTCLAS_Init error!");
        return -1;
    } else {
        info(log, "ICTCLAS_Init successful!");
    }
    if ((num = ICTCLAS_ImportUserDictFile(
                (DATA_DIR + "UserDict.txt").c_str(),
                CODE_TYPE_UTF8)) < 0) {
        error(log, "ICTCLAS_ImportUserDictFile error!");
    } else {
        info(log, "load %d items from UserDict.txt", num);
        ICTCLAS_SaveTheUsrDic();
    }

    num = load_file(raw_file, uid_user_map, uid_home_map, mid_content_map, log);
    info(log, "message num: %d, message map size: %ld", num, mid_content_map.size());
    parse_content(mid_content_map, mid_word_map, log);
    rank_user(mid_content_map, uid_user_map, mid_word_map, msg_rank);
    sort(msg_rank.begin(), msg_rank.end(), compare_msg);

    //load travel line
    num = load_line(line_file, line_map);
    //show_line(line_vector);
    match_msg(mid_word_map, line_map, mid_lid_map);
    fill_result(mid_lid_map, msg_rank);
    do_filter_by_user(uid_user_map, line_map, msg_rank);
    out_put(msg_rank, "", out_file);

    ICTCLAS_Exit();
    destory_log(log);
    return 0;
}

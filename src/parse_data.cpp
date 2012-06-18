#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "libjson.h"
#include "ICTCLAS50.h"
#include "log.h"
#include "data_struct.h"

using namespace std;

#define SPLIT_SYMBLE " "

static const string INIT_DIR = "/home/sldu/cpp/ictclas/api/";
static const string DATA_DIR = "/home/sldu/cpp/ictclas/data/";
static const string LOGS_DIR = "/home/sldu/cpp/ictclas/logs/";

static bool compare_msg(msg_t m1, msg_t m2) {
    return m1.score > m2.score;
}

inline void check_user(user_t &user, uid_user_map_t &uum) {
    if (uum.find(user.uid) == uum.end()) {
        uum[user.uid] = user;
    }
}

int load_file(const string &filename,
        uid_user_map_t &uum,
        vector<msg_t> &msg_vec,
        log_t *log) {
    ifstream ifile;
    string line;
    string content;
    string home;
    long uid;
    int num = 0;

    ifile.open(filename.c_str());
    if (ifile.is_open()) {
        while (getline(ifile, line)) {
            //mid uid provience city content
            msg_t one_msg;
            user_t one_user = {-1, "", ""};
            JSONNode node = libjson::parse(line);
            JSONNode::const_iterator jit = node.begin();
            while (jit != node.end()) {
                string name = string(jit->name());
                if (name == "id") {
                    one_msg.mid = atol(jit->as_string().c_str());
                } else if (name == "user_id") {
                    uid = atol(jit->as_string().c_str());
                    one_user.uid = uid;
                    one_msg.uid = uid;
                } else if (name == "user_name") {
                    one_user.name = jit->as_string();
                } else if (name == "location") {
                    one_user.home = jit->as_string();
                } else if (name == "text") {
                    one_msg.content = jit->as_string();
                }
            }
            check_user(one_user, uum);
            /*
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
            */
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

int parse_content(uid_user_map_t &uum,
        vector<msg_t> &msg_vec,
        log_t *log) {
    char buf[2048];
    string result;

    for (vector<msg_t>::iterator it = msg_vec.begin(); it != msg_vec.end(); ++it) {
        if (ICTCLAS_ParagraphProcess(it->content.c_str(),
                                     it->content.length(),
                                     buf,
                                     CODE_TYPE_UNKNOWN,
                                     1)
            ) {
                it->ns_word = buf;
            /*
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
            */
        } else {
            error(log, "message %ld process error!", it->mid);
        }
    }

    return 0;
}

int init_ictclas(log_t *log, const string &init_path) {
    int num;
    if (!ICTCLAS_Init(init_path.c_str())) {
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
    return 0;
}

void out_put(vector<msg_t> &msg_vec,
             uid_user_map_t &uum,
             const string &path,
             const string &filename) {
    size_t pos1, pos2 = 0;
    ofstream ofile;
    ofile.open((path+filename).c_str());
    if (ofile.is_open()) {
        //id, user_id, user_name, location, words
        for (vector<msg_t>::iterator it = msg_vec.begin();
             it != msg_vec.end(); ++it) {
            if ((it->lid == NULL) || (it->lid->size() == 0))
    	        continue;
            JSONNode root_node(JSON_NODE);
            root_node.push_back(JSONNode("id", it->mid));
            root_node.push_back(JSONNode("user_id", it->uid));
            root_node.push_back(JSONNode("user_name", uum[it->uid].name));
            root_node.push_back(JSONNode("location", uum[it->uid].home));
            //ofile << it->mid << " " << it->uid;
            JSONNode words(JSON_ARRAY);
            words.set_name("words");
            while ((pos1 = it->content.find(" ", pos2)) != string::npos) {
                string word = it->content.substr(pos1, pos1-pos2);
                words.push_back(JSONNode("", word));
                pos2 = pos1 + 1;
            }
            root_node.push_back(words);
            ofile << root_node.write_formatted() << endl;
        }
        ofile.close();
    }
}

/*
void fill_msg(mid_content_map_t &mcm,
        uid_user_map_t &uum,
        mid_word_map_t &mwm,
        vector<msg_t> &msg_vec) {
    for (uid_user_map_t::iterator uit = uum.begin();
         uit != uum.end(); ++uit) {
        for (vector<long>::iterator mit = uit->second.mids.begin();
             mit != uit->second.mids.end(); ++mit) {
            msg_t msg = {*mit, uit->first, 0, NULL, mcm[*mit], ""};
            mid_word_map_t::iterator it = mwm.find(*mit);
            if (it != mwm.end()) {
                msg.score = it->second.size();
#if 1
                for (vector<string>::iterator it_word = it->second.begin();
                     it_word != it->second.end(); ++it_word) {
                    msg.ns_word.push_back(*it_word);
                }
#endif
            }
            if (msg.score > 0)
                msg_vec.push_back(msg);
        }
    }
}
*/

int main(int argc, char **argv) {
    int num;
    string raw_file(DATA_DIR+"raw.txt");
    string line_file(DATA_DIR+"line_keyword.txt");
    string out_file(DATA_DIR+"parse_data.txt");
    uid_user_map_t uid_user_map;
    vector<msg_t> msg_vec;

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
    init_ictclas(log, INIT_DIR);

    num = load_file(raw_file, uid_user_map, msg_vec, log);
    parse_content(uid_user_map, msg_vec, log);
    info(log, "parse complete");
    //fill_msg(mid_content_map, uid_user_map, mid_word_map, msg_vec);
    //sort(msg_vec.begin(), msg_vec.end(), compare_msg);
    out_put(msg_vec, uid_user_map, "", out_file);

    ICTCLAS_Exit();
    destory_log(log);
    return 0;
}

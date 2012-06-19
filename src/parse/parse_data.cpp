#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include "libjson.h"
#include "ICTCLAS50.h"
#include "log.h"
#include "data_struct.h"
#include "user.h"

using namespace std;

#define NDEBUG

static string INIT_DIR = "/home/sldu/cpp/ictclas/api/";
static string DATA_DIR = "/home/sldu/cpp/ictclas/data/";
static string LOGS_DIR = "/home/sldu/cpp/ictclas/logs/";

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
            msg_t one_msg;
            user_t one_user = {-1, "", ""};
            JSONNode node = libjson::parse(line);
            JSONNode::const_iterator jit = node.begin();
            while (jit != node.end()) {
                string name = (string)jit->name();
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
                ++jit;
            }
            check_user(one_user, uum);
            msg_vec.push_back(one_msg);
            ++num;
        }
        ifile.close();
    } else {
        error(log, "open raw file %s error", filename.c_str());
    }
    return num;
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

inline string &make_readable(string &str) {
    //add \ before "
    size_t pos1, pos2 = 0;
    while ((pos1 = str.find("\"", pos2)) != string::npos) {
        str.insert(pos1, 1, '\\');
        pos2 = pos1 + 2;
    }
    return str;
}

void out_put(vector<msg_t> &msg_vec,
             uid_user_map_t &uum,
             const string &path,
             const string &filename) {
    ofstream ofile;
    ofile.open((path+filename).c_str());
    if (ofile.is_open()) {
        //id, user_id, user_name, location, words
        for (vector<msg_t>::iterator it = msg_vec.begin();
             it != msg_vec.end(); ++it) {
            size_t pos1, pos2 = 0;
            stringstream ss;
            ss << "{\"id\":" << it->mid << ","
               << "\"user_id\":" << it->uid << ","
               << "\"user_name\":\"" << uum[it->uid].name << "\","
               << "\"location\":\"" << uum[it->uid].home << "\","
               << "\"words\":\"" << make_readable(it->ns_word) << "\"}";
            /*
            if ((pos1 = it->ns_word.find(" ", pos2)) != string::npos) {
                string word = it->ns_word.substr(pos2, pos1-pos2);
                ss << "\"" << make_readable(word) << "\"";
                pos2 = pos1 + 1;
            }
            while ((pos1 = it->ns_word.find(" ", pos2)) != string::npos) {
                string word = it->ns_word.substr(pos2, pos1-pos2);
                ss << ",\"" << make_readable(word) << "\"";
                pos2 = pos1 + 1;
            }
            ss << "]}";
            */
            ofile << ss.str() << endl;
            /*
            JSONNode root_node(JSON_NODE);
            root_node.push_back(JSONNode("id", it->mid));
            root_node.push_back(JSONNode("user_id", it->uid));
            root_node.push_back(JSONNode("user_name", uum[it->uid].name));
            root_node.push_back(JSONNode("location", uum[it->uid].home));
            JSONNode words(JSON_ARRAY);
            words.set_name("words");
            while ((pos1 = it->ns_word.find(" ", pos2)) != string::npos) {
                string word = it->ns_word.substr(pos2, pos1-pos2);
                words.push_back(JSONNode("", word));
                pos2 = pos1 + 1;
            }
            root_node.push_back(words);
            ofile << root_node.write() << endl;
            */
        }
        ofile.close();
    }
}

void load_config(const string filename) {
    ifstream ifile(filename.c_str());
    string line;
    size_t pos;
    string name, value;

    if (ifile.is_open()) {
        while (getline(ifile, line)) {
            if (line[0] == '#')
                continue;
            if ((pos = line.find("=")) == string::npos)
                continue;
            name = line.substr(0, pos);
            value = line.substr(pos+1);
            if (name.find("init_dir") != string::npos) {
                INIT_DIR = value;
            } else if (name.find("data_dir") != string::npos) {
                DATA_DIR = value;
            } else if (name.find("logs_dir") != string::npos) {
                LOGS_DIR = value;
            }
        }
        ifile.close();
    }
}

int main(int argc, char **argv) {
    int num;
    string conf_file("../bin/parse_data.conf");
    string raw_file(DATA_DIR+"raw.txt");
    string out_file(DATA_DIR+"parse_data.txt");
    uid_user_map_t uid_user_map;
    vector<msg_t> msg_vec;

    if (argc == 3) {
        raw_file = argv[1];
        out_file = argv[2];
    } else {
        cout << "usage: " << argv[0] << " weibo_msg_file output_file"
            << endl;
        return -1;
    }

    load_config(conf_file);
    log_t *log = create_log((LOGS_DIR+"parse_data").c_str());
    info(log, "parse_data start!");
    init_ictclas(log, INIT_DIR);

    num = load_file(raw_file, uid_user_map, msg_vec, log);
    info(log, "start parsing");
    parse_content(uid_user_map, msg_vec, log);
    info(log, "parse complete");
    out_put(msg_vec, uid_user_map, "", out_file);

    ICTCLAS_Exit();
    destory_log(log);
    return 0;
}

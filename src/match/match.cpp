#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <set>
#include "libjson.h"
#include "data_struct.h"
#include "user.h"

using namespace std;

#define SPLIT_SYMBLE " "

int load_msg(const string &filename,
              uid_user_map_t &uum,
              vector<msg_t> &msg_vec) {
    ifstream ifile;
    string line;
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
                } else if (name == "words") {
                    one_msg.ns_word = jit->as_string();
                }
                ++jit;
            }
            check_user(one_user, uum);
            msg_vec.push_back(one_msg);
            ++num;
        }
        ifile.close();
    }
    return num;
}

int load_line(const string &file, map<int, line_t> &line_map) {
    //product_id product_wid product_url product_description product_keyword
    ifstream ifile;
    int num = 0;
    string home;
    ifile.open(file.c_str());
    if (ifile.is_open()) {
        string line;
        while (getline(ifile, line)) {
            line_t one_line;
            JSONNode node = libjson::parse(line);
            JSONNode::const_iterator jit = node.begin();
            while (jit != node.end()) {
                string name = (string)jit->name();
                if (name == "product_id") {
                    one_line.pid = jit->as_int();
                } else if (name == "product_wid") {
                    one_line.pwid = jit->as_int();
                } else if (name == "product_url") {
                    one_line.url = jit->as_string();
                } else if (name == "product_description") {
                    one_line.description = jit->as_string();
                } else if (name == "product_keyword") {
                    string keyword(jit->as_string());
                    size_t pos1, pos2;
                    if ((pos1 = keyword.find(SPLIT_SYMBLE)) != string::npos) {
                        one_line.home = keyword.substr(0, pos1);
                        pos2 = pos1 + 1;
                        while ((pos1 = keyword.find(SPLIT_SYMBLE, pos2)) != string::npos) {
                            one_line.des.push_back(keyword.substr(pos2, pos1-pos2));
                            pos2 = pos1 + 1;
                        }
                        if (pos2 < keyword.length()-1) {
                            //pos2 not the last position
                            one_line.des.push_back(keyword.substr(pos2));
                        }
                    }
                }
                line_map[one_line.pid] = one_line;
                ++jit;
            }
            /*
            vector<string> des_vec;
            home = line.substr(pos1, pos2-pos1);
            des_vec.push_back(line.substr(pos1));
            line_t tmp = {lid, home, des_vec};
            line_map[lid] = tmp;
            */
            ++num;
        }
        ifile.close();
    }
    return num;
}

inline bool is_ns(const string &str) {
    if (str.find("ns") != string::npos)
        return true;
    else
        return false;
}

bool is_match(msg_t &msg, line_t &line) {
    size_t pos1, pos2 = 0;
    while ((pos1 = msg.ns_word.find(SPLIT_SYMBLE, pos2)) != string::npos) {
        string place = msg.ns_word.substr(pos2, pos1-pos2);
        pos2 = pos1 + 1;
        if (!is_ns(place))
            continue;
        for (vector<string>::iterator dit = line.des.begin();
            dit != line.des.end(); ++dit) {
            if (place.find(*dit) != string::npos) {
                //msg.lit.insert(lit->second.lid);
                return true;
            }
        }
    }

    return false;
}

void match_msg(map<int, line_t> &line_map,
               vector<msg_t> &msg_vec) {
    //for each message
    for (vector<msg_t>::iterator mit = msg_vec.begin();
         mit != msg_vec.end(); ++mit) {
        for (map<int, line_t>::iterator lit = line_map.begin();
             lit != line_map.end(); ++lit) {
            if (is_match(*mit, lit->second)) {
                mit->lid.insert(lit->first);
            }
        }
                    /*
        size_t pos1, pos2 = 0;
        //for each place mentioned in one message
        while ((pos1 = mit->ns_word.find(SPLIT_SYMBLE, pos2)) == string::npos) {
            string place = mit->ns_word.substr(pos2, pos1-pos2);
            pos2 = pos1 + 1;
            if (!is_ns(place))
                continue;
            //for each line
            for (map<int, line_t>::iterator lit = line_map.begin();
                 lit != line_map.end(); ++lit) {
                //for each destination in one line
                if (is_match(*mit, *lit)) {
                        tmp_set.insert(lit->second.lid);
                }
                for (vector<string>::iterator dit = lit->second.des.begin();
                     dit != lit->second.des.end(); ++dit) {
                    if (place.find(*dit) != string::npos) {
                        tmp_set.insert(lit->second.lid);
                        break; //next line
                    }
                }
                */
    }
}

void do_filter_by_user(map<int, line_t> &line_map,
                       uid_user_map_t &uum,
                       vector<msg_t> &msg_vec) {
    //set<int>::iterator it_del;
    for (vector<msg_t>::iterator mit = msg_vec.begin();
         mit != msg_vec.end(); ++mit) {
        if (mit->lid.size() != 0) {
            for (set<int>::iterator lit = mit->lid.begin();
                 lit != mit->lid.end(); ) {
                if (uum[mit->uid].home.find(line_map[*lit].home) == string::npos)
                    mit->lid.erase(lit++);
                else
                    ++lit;
            }
        }
    }
}

void out_put(vector<msg_t> &msg_vec,
             uid_user_map_t &uum,
             map<int, line_t> &line_map,
             const string &path,
             const string &filename) {
    //json: id user_id user_name product_id product_wid product_url product_description
    ofstream ofile;
    ofile.open((path+filename).c_str());
    if (ofile.is_open()) {
        for (vector<msg_t>::iterator it = msg_vec.begin();
             it != msg_vec.end(); ++it) {
            if (it->lid.size() == 0)
    	        continue;
            stringstream ss;
            ss << "{\"id\":" << it->mid << ","
               << "\"user_id\":" << it->uid << ","
               << "\"user_name\":\"" << uum[it->uid].name << "\","
               << "\"product\":[";
            string product_ids, product_wids, product_description;
            set<int>::iterator pit = it->lid.begin();
            line_t &one_line = line_map[*pit];
            if (pit != it->lid.end()) {
                ss << "{\"product_id\":" << *pit << ","
                   << "\"product_wid\":" << one_line.pwid << ","
                   << "\"product_url\":\"" << one_line.url << "\","
                   << "\"product_description\":\"" << one_line.description << "\"}";
                ++pit;
            }
            for (; pit != it->lid.end(); ++pit) {
                ss << ",{\"product_id\":" << *pit << ","
                   << "\"product_wid\":" << one_line.pwid << ","
                   << "\"product_url\":\"" << one_line.url << "\","
                   << "\"product_description\":\"" << one_line.description << "\"}";
            }
            ss << "]}";
            ofile << ss.str() << endl;
        }
        ofile.close();
    }
}

int main(int argc, char **argv) {
    int num;
    string msg_file;
    string line_file;
    string out_file;
    uid_user_map_t user_map;
    vector<msg_t> msg_vec;
    map<int, line_t> line_map;

    if (argc == 4) {
        msg_file = argv[1];
        line_file = argv[2];
        out_file = argv[3];
    } else {
        cout << "usage: " << argv[0] << " sop2 pos1 output_file"
            << endl;
        return -1;
    }
    num = load_msg(msg_file, user_map, msg_vec);
    num = load_line(line_file, line_map);

    cout << "load finish" << endl;
    match_msg(line_map, msg_vec);
    cout << "match finish" << endl;
    do_filter_by_user(line_map, user_map, msg_vec);
    cout << "filter finish" << endl;
    out_put(msg_vec, user_map, line_map, "", out_file);

    return 0;
}

#include <iostream>
#include <cstring>
#include <string>
#include "ICTCLAS50.h"

using namespace std;

int parse_content(char *msg, char *buf) {
    if (ICTCLAS_ParagraphProcess(msg,
                                 strlen(msg),
                                 buf,
                                 CODE_TYPE_UNKNOWN,
                                 1)
       ) {
        return 0;
    } else {
        return -1;
    }
}

int init_ictclas(const string &init_path) {
    int num;
    if (!ICTCLAS_Init(init_path.c_str())) {
        return -1;
    } else {
    }
    if ((num = ICTCLAS_ImportUserDictFile(
                                          "../ictclas/data/UserDict.txt",
                                          CODE_TYPE_UTF8)) < 0) {
    } else {
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

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "usage: " << argv[0] << " message" << endl;
        return -1;
    }

    char buf[10240] = {0};
    string result;
    init_ictclas("../ictclas/api");
    if (parse_content(argv[1], buf) == 0) {
	result = buf;
        make_readable(result);
	cout << result << endl;
    }

    ICTCLAS_Exit();
    return 0;
}

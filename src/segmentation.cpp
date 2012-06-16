#include <iostream>
#include <string>
#include "ICTCLAS50.h"

using namespace std;

static const string INIT_DIR = "/home/sldu/cpp/ictclas/api";

int main(int argc, char **argv) {
    //load_config(CONFIG_PATH);
    if (!ICTCLAS_Init(INIT_DIR.c_str())) {
        cerr << "ICTCLAS_Init error!" << endl;
        return -1;
    } else {
        cout << "ICTCLAS_Init successful!" << endl;
    }

    int num;
    if ((num = ICTCLAS_ImportUserDictFile("/home/sldu/cpp/ictclas/src/UserDict.txt", CODE_TYPE_UTF8)) < 0) {
        cerr << "ICTCLAS_ImportUserDictFile error!" << endl;
    } else {
        cout << "load " << num << " items from UserDict.txt" << endl;
        ICTCLAS_SaveTheUsrDic();
    }

    if (ICTCLAS_FileProcess("text.txt", "out.txt", CODE_TYPE_UNKNOWN, 1)) {
        cout << "ICTCLAS_FileProcess successful!" << endl;
    } else {
        cerr << "ICTCLAS_FileProcess error!" << endl;
    }

    ICTCLAS_Exit();
    return 0;
}

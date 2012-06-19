#ifndef __USER_H__
#define __USER_H__
#include "data_struct.h"

inline void check_user(user_t &user, uid_user_map_t &uum) {
    if (uum.find(user.uid) == uum.end()) {
        uum[user.uid] = user;
    }
}

#endif

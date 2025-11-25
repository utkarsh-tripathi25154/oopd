#pragma once
#include "exceptions.h"

template<typename T = long>
class CellularCore {
private:
    T max_capacity;
    T current_load;
    int registered_count;
    static const int MAX_USERS = 10000;
    char registered_ids[MAX_USERS][32];

public:
    CellularCore(T capacity = 10000) 
        : max_capacity(capacity), current_load(0), registered_count(0) {}

    bool canRegister(T user_messages) const {
        return (current_load + user_messages) <= max_capacity;
    }

    bool registerUser(const char* user_id, T user_messages) {
        if (!canRegister(user_messages) || registered_count >= MAX_USERS)
            return false;
        int i = 0;
        while (user_id[i] != '\0' && i < 31) {
            registered_ids[registered_count][i] = user_id[i];
            ++i;
        }
        registered_ids[registered_count][i] = '\0';
        current_load += user_messages;
        ++registered_count;
        return true;
    }

    void reset() { current_load = 0; registered_count = 0; }
    T getMaxCapacity() const { return max_capacity; }
    int getRegisteredCount() const { return registered_count; } // 🔑 ADDED
};

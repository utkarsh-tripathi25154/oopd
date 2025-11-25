#pragma once
#include "UserDevice.h"
#include "CellularCore.h"

class CellTower {
protected:
    char tech_name[32];
    char protocol[32];
    double total_spectrum_mhz;
    double channel_bandwidth_mhz;
    int users_per_channel;
    int antenna_factor;
    bool mimo_enabled;
    CellularCore<long>* core;
    static const int MAX_USERS = 10000;
    UserDevice users[MAX_USERS];
    int user_count;

    // Per-frequency slots (for multi-band support)
    struct FreqSlot {
        int freq_mhz;
        int current_users;
        int max_users;
    };
    static const int MAX_SLOTS = 20;
    FreqSlot slots[MAX_SLOTS];
    int slot_count;

    virtual int computeMessages(int service_type) const = 0;

public:
    CellTower(const char* name, const char* proto,
              double total_spec_mhz, double chan_bw,
              int upc, int ant, bool mimo, CellularCore<long>* c)
        : total_spectrum_mhz(total_spec_mhz), channel_bandwidth_mhz(chan_bw),
          users_per_channel(upc), antenna_factor(ant), mimo_enabled(mimo),
          core(c), user_count(0), slot_count(0) {
        // Copy name
        int i = 0;
        while (name[i] && i < 31) { tech_name[i] = name[i]; ++i; }
        tech_name[i] = '\0';
        i = 0;
        while (proto[i] && i < 31) { protocol[i] = proto[i]; ++i; }
        protocol[i] = '\0';
    }

    virtual ~CellTower() {}

    virtual void initializeSlots() = 0;

    bool addUser(int service_type, int freq_mhz) {
        if (user_count >= MAX_USERS) return false;

        int messages = computeMessages(service_type);
        if (!core->canRegister(messages)) {
            return false; // Core full
        }

        // Find slot
        int slot_idx = -1;
        for (int i = 0; i < slot_count; ++i) {
            if (slots[i].freq_mhz == freq_mhz) {
                if (slots[i].current_users >= slots[i].max_users) return false;
                slot_idx = i;
                break;
            }
        }
        if (slot_idx == -1) return false; // Invalid freq

        // Register user
        users[user_count].setID(user_count + 1);
        users[user_count].setFrequency(freq_mhz);
        users[user_count].setMessages(messages);
        users[user_count].setServiceType(service_type);
        core->registerUser(users[user_count].getID(), messages);
        slots[slot_idx].current_users++;
        user_count++;
        return true;
    }

    int getMaxUsersBySpectrum() const {
        int channels = static_cast<int>(total_spectrum_mhz / channel_bandwidth_mhz);
        return channels * users_per_channel * (mimo_enabled ? antenna_factor : 1);
    }

    void getFirstChannelUsers(char out[][32], int& count) const {
        if (slot_count == 0) { count = 0; return; }
        int cap = slots[0].max_users;
        count = (user_count < cap) ? user_count : cap;
        for (int i = 0; i < count; ++i) {
            int j = 0;
            const char* id = users[i].getID();
            while (id[j]) { out[i][j] = id[j]; ++j; }
            out[i][j] = '\0';
        }
    }

    int getCoresNeededForFull() const {
        long full_load = static_cast<long>(getMaxUsersBySpectrum()) * computeMessages(1);
        long cap = core->getMaxCapacity();
        if (cap <= 0) return 1;
        return (full_load + cap - 1) / cap;
    }

    // Getters
    const char* getTechName() const { return tech_name; }
    int getUserCount() const { return user_count; }
    int getSlotCount() const { return slot_count; }
    int getSlotFreq(int i) const { return (i < slot_count) ? slots[i].freq_mhz : 0; }
    int getSlotUsers(int i) const { return (i < slot_count) ? slots[i].current_users : 0; }
    int getSlotMax(int i) const { return (i < slot_count) ? slots[i].max_users : 0; }
};

// --- Concrete Towers ---

class G2Tower : public CellTower {
public:
    G2Tower(CellularCore<long>* core)
        : CellTower("2G", "TDMA", 1.0, 0.2, 16, 1, false, core) {
        initializeSlots();
    }
    void initializeSlots() override {
        slot_count = 5;
        for (int i = 0; i < 5; ++i) {
            slots[i].freq_mhz = 1800 + i * 200;
            slots[i].current_users = 0;
            slots[i].max_users = 16;
        }
    }
    int computeMessages(int s) const override {
        if (s == 1) return 15;
        if (s == 2) return 2;
        if (s == 3) return 5;
        return 20; // Voice+Data
    }
};

class G3Tower : public CellTower {
public:
    G3Tower(CellularCore<long>* core)
        : CellTower("3G", "CDMA", 1.0, 0.2, 32, 1, false, core) {
        initializeSlots();
    }
    void initializeSlots() override {
        slot_count = 5;
        for (int i = 0; i < 5; ++i) {
            slots[i].freq_mhz = 1800 + i * 200;
            slots[i].current_users = 0;
            slots[i].max_users = 32;
        }
    }
    int computeMessages(int s) const override { return 10; }
};

class G35Tower : public CellTower {
public:
    G35Tower(CellularCore<long>* core)
        : CellTower("3.5G", "HSPA", 1.0, 0.2, 64, 1, false, core) {
        initializeSlots();
    }
    void initializeSlots() override {
        slot_count = 5;
        for (int i = 0; i < 5; ++i) {
            slots[i].freq_mhz = 1800 + i * 200;
            slots[i].current_users = 0;
            slots[i].max_users = 64;
        }
    }
    int computeMessages(int s) const override { return 8; }
};

class G4Tower : public CellTower {
public:
    G4Tower(CellularCore<long>* core)
        : CellTower("4G", "OFDM", 1.0, 0.01, 30, 4, true, core) {
        initializeSlots();
    }
    void initializeSlots() override {
        slot_count = 10;
        for (int i = 0; i < 10; ++i) {
            slots[i].freq_mhz = 1800 + i * 10;
            slots[i].current_users = 0;
            slots[i].max_users = 30 * 4; // MIMO reuse
        }
    }
    int computeMessages(int s) const override {
        if (s == 1) return 15;
        if (s == 2) return 2;
        if (s == 3) return 25;
        return 40;
    }
};

class G4PlusTower : public CellTower {
public:
    G4PlusTower(CellularCore<long>* core)
        : CellTower("4G+", "LTE-A", 1.0, 0.01, 40, 4, true, core) {
        initializeSlots();
    }
    void initializeSlots() override {
        slot_count = 10;
        for (int i = 0; i < 10; ++i) {
            slots[i].freq_mhz = 1800 + i * 10;
            slots[i].current_users = 0;
            slots[i].max_users = 40 * 4;
        }
    }
    int computeMessages(int s) const override {
        if (s == 1) return 12;
        if (s == 2) return 2;
        if (s == 3) return 20;
        return 32;
    }
};

class G5Tower : public CellTower {
public:
    G5Tower(CellularCore<long>* core)
        : CellTower("5G", "Massive MIMO", 11.0, 1.0, 30, 16, true, core) {
        initializeSlots();
    }
    void initializeSlots() override {
        slot_count = 11;
        for (int i = 0; i < 11; ++i) {
            slots[i].freq_mhz = 1800 + i * 1000;
            slots[i].current_users = 0;
            slots[i].max_users = 30 * 16;
        }
    }
    int computeMessages(int s) const override {
        if (s == 1) return 10;
        if (s == 2) return 2;
        if (s == 3) return 25;
        return 15;
    }
};

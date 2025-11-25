#pragma once
#include "UserDevice.h"
#include "CellularCore.h"

class CellTower {
protected:
    char tech_name[32];
    char protocol[32];
    double spectrum_mhz;
    double channel_bandwidth_mhz;
    int users_per_channel;
    int messages_per_user;
    int antenna_factor;
    bool mimo_enabled;
    CellularCore<long>* core;
    static const int MAX_USERS = 10000;
    UserDevice users[MAX_USERS];
    int user_count;

public:
    CellTower(const char* name, const char* proto, double spec, double chan_bw,
              int upc, int msgs, int ant, bool mimo, CellularCore<long>* c)
        : spectrum_mhz(spec), channel_bandwidth_mhz(chan_bw),
          users_per_channel(upc), messages_per_user(msgs),
          antenna_factor(ant), mimo_enabled(mimo), core(c), user_count(0) {
        int i = 0;
        while (name[i] && i < 31) { tech_name[i] = name[i]; ++i; }
        tech_name[i] = '\0';
        i = 0;
        while (proto[i] && i < 31) { protocol[i] = proto[i]; ++i; }
        protocol[i] = '\0';
    }

    virtual ~CellTower() {}

    int getMaxUsersBySpectrum() const {
        int channels = static_cast<int>(spectrum_mhz / channel_bandwidth_mhz);
        int total = channels * users_per_channel;
        if (mimo_enabled) total *= antenna_factor;
        return total;
    }

    int simulate() {
        core->reset();
        user_count = 0;
        int max_users = getMaxUsersBySpectrum();
        int supported = 0;
        for (int i = 1; i <= max_users; ++i) {
            if (!core->canRegister(messages_per_user)) break;
            if (user_count >= MAX_USERS) break;
            users[user_count].setID(i);
            users[user_count].setMessages(messages_per_user);
            int base_freq = 1800;
            users[user_count].setFrequency(base_freq);
            ++user_count;
            ++supported;
        }
        return supported;
    }

    void getFirstChannelUsers(char out[][32], int& count) const {
        int cap = users_per_channel * (mimo_enabled ? antenna_factor : 1);
        count = (user_count < cap) ? user_count : cap;
        for (int i = 0; i < count; ++i) {
            int j = 0;
            const char* id = users[i].getID();
            while (id[j]) { out[i][j] = id[j]; ++j; }
            out[i][j] = '\0';
        }
    }

    int getCoresNeededForFull() const {
        long full_load = static_cast<long>(getMaxUsersBySpectrum()) * messages_per_user;
        long cap = core->getMaxCapacity();
        if (cap <= 0) return 1;
        return (full_load + cap - 1) / cap;
    }

    const char* getTechName() const { return tech_name; }
    const char* getProtocol() const { return protocol; }
    int getMessagesPerUser() const { return messages_per_user; }
    double getSpectrumMHz() const { return spectrum_mhz; }
    
    // 🔑 NEW GETTERS (public access to protected data)
    double getChannelBandwidthMHz() const { return channel_bandwidth_mhz; }
    int getUsersPerChannel() const { return users_per_channel; }
};

// --- Concrete Towers ---

class G2Tower : public CellTower {
public:
    G2Tower(CellularCore<long>* core) 
        : CellTower("2G", "TDMA (Time Division Multiple Access)", 
                   1.0, 0.2, 16, 15, 1, false, core) {}
};

class G3Tower : public CellTower {
public:
    G3Tower(CellularCore<long>* core) 
        : CellTower("3G", "CDMA (Code Division Multiple Access)", 
                   1.0, 0.2, 32, 10, 1, false, core) {}
};

class G35Tower : public CellTower {
public:
    G35Tower(CellularCore<long>* core) 
        : CellTower("3.5G", "HSPA (High-Speed Packet Access)", 
                   1.0, 0.2, 64, 8, 1, false, core) {}
};

class G4Tower : public CellTower {
public:
    G4Tower(CellularCore<long>* core) 
        : CellTower("4G", "OFDM (Orthogonal Frequency Division Multiplexing)", 
                   1.0, 0.01, 30, 10, 4, true, core) {}
};

class G4PlusTower : public CellTower {
public:
    G4PlusTower(CellularCore<long>* core) 
        : CellTower("4G+", "LTE-Advanced (Carrier Aggregation + OFDM)", 
                   1.0, 0.01, 40, 8, 4, true, core) {}
};

class G5Tower : public CellTower {
public:
    G5Tower(CellularCore<long>* core) 
        : CellTower("5G", "OFDM + Massive MIMO", 
                   11.0, 1.0, 30, 10, 16, true, core) {}
};

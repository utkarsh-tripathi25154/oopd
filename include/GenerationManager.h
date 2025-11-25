#pragma once
#include "UserDevice.h"
#include "CellularCore.h"
#include "basicIO.h"

extern basicIO io;

class GenerationManager {
private:
    char tech_name[32];
    char protocol[64];
    int messages_per_user;
    int current_gen;
    static const int MAX_USERS = 10000;
    static const int MAX_FREQUENCIES = 100;
    UserDevice users[MAX_USERS];
    int user_count;
    struct FrequencySlot {
        int frequency_mhz;
        int current_users;
        int max_users;
    } spectrum_slots[MAX_FREQUENCIES];
    int slot_count;
    int total_spectrum_mhz;
    double channel_bandwidth_mhz;
    int users_per_channel;
    int antenna_factor;
    bool mimo_enabled;
    CellularCore<long>* core; // 🔥 ADDED

public:
    GenerationManager(int gen, CellularCore<long>* c)
        : current_gen(gen), user_count(0), slot_count(0), core(c) {
        initializeFromGeneration(gen);
    }

    void initializeFromGeneration(int gen) {
        if (gen == 2) {
            const char* t = "2G";
            int i = 0; while (t[i]) { tech_name[i] = t[i]; ++i; } tech_name[i] = '\0';
            const char* p = "TDMA (Time Division Multiple Access)";
            i = 0; while (p[i] && i < 63) { protocol[i] = p[i]; ++i; } protocol[i] = '\0';
            messages_per_user = 15;
            total_spectrum_mhz = 1;
            channel_bandwidth_mhz = 0.2;
            users_per_channel = 16;
            antenna_factor = 1;
            mimo_enabled = false;
            slot_count = 0;
            for (int i = 0; i < 5; ++i) {
                spectrum_slots[i].frequency_mhz = 1800 + (i * 200);
                spectrum_slots[i].current_users = 0;
                spectrum_slots[i].max_users = 16;
                slot_count++;
            }
        } else if (gen == 3) {
            const char* t = "3G";
            int i = 0; while (t[i]) { tech_name[i] = t[i]; ++i; } tech_name[i] = '\0';
            const char* p = "CDMA (Code Division Multiple Access)";
            i = 0; while (p[i] && i < 63) { protocol[i] = p[i]; ++i; } protocol[i] = '\0';
            messages_per_user = 10;
            total_spectrum_mhz = 1;
            channel_bandwidth_mhz = 0.2;
            users_per_channel = 32;
            antenna_factor = 1;
            mimo_enabled = false;
            slot_count = 0;
            for (int i = 0; i < 5; ++i) {
                spectrum_slots[i].frequency_mhz = 1800 + (i * 200);
                spectrum_slots[i].current_users = 0;
                spectrum_slots[i].max_users = 32;
                slot_count++;
            }
        } else if (gen == 4) {
            const char* t = "3.5G";
            int i = 0; while (t[i]) { tech_name[i] = t[i]; ++i; } tech_name[i] = '\0';
            const char* p = "HSPA (High-Speed Packet Access)";
            i = 0; while (p[i] && i < 63) { protocol[i] = p[i]; ++i; } protocol[i] = '\0';
            messages_per_user = 8;
            total_spectrum_mhz = 1;
            channel_bandwidth_mhz = 0.2;
            users_per_channel = 64;
            antenna_factor = 1;
            mimo_enabled = false;
            slot_count = 0;
            for (int i = 0; i < 5; ++i) {
                spectrum_slots[i].frequency_mhz = 1800 + (i * 200);
                spectrum_slots[i].current_users = 0;
                spectrum_slots[i].max_users = 64;
                slot_count++;
            }
        } else if (gen == 5) {
            const char* t = "4G";
            int i = 0; while (t[i]) { tech_name[i] = t[i]; ++i; } tech_name[i] = '\0';
            const char* p = "OFDM (Orthogonal Frequency Division Multiplexing)";
            i = 0; while (p[i] && i < 63) { protocol[i] = p[i]; ++i; } protocol[i] = '\0';
            messages_per_user = 10;
            total_spectrum_mhz = 1;
            channel_bandwidth_mhz = 0.01;
            users_per_channel = 30;
            antenna_factor = 4;
            mimo_enabled = true;
            slot_count = 0;
            for (int i = 0; i < 10; ++i) {
                spectrum_slots[i].frequency_mhz = 1800 + (i * 10);
                spectrum_slots[i].current_users = 0;
                spectrum_slots[i].max_users = 120;
                slot_count++;
            }
        } else if (gen == 6) {
            const char* t = "4G+";
            int i = 0; while (t[i]) { tech_name[i] = t[i]; ++i; } tech_name[i] = '\0';
            const char* p = "LTE-Advanced (Carrier Aggregation + OFDM)";
            i = 0; while (p[i] && i < 63) { protocol[i] = p[i]; ++i; } protocol[i] = '\0';
            messages_per_user = 8;
            total_spectrum_mhz = 1;
            channel_bandwidth_mhz = 0.01;
            users_per_channel = 40;
            antenna_factor = 4;
            mimo_enabled = true;
            slot_count = 0;
            for (int i = 0; i < 10; ++i) {
                spectrum_slots[i].frequency_mhz = 1800 + (i * 10);
                spectrum_slots[i].current_users = 0;
                spectrum_slots[i].max_users = 160;
                slot_count++;
            }
        } else if (gen == 7) {
            const char* t = "5G";
            int i = 0; while (t[i]) { tech_name[i] = t[i]; ++i; } tech_name[i] = '\0';
            const char* p = "OFDM + Massive MIMO";
            i = 0; while (p[i] && i < 63) { protocol[i] = p[i]; ++i; } protocol[i] = '\0';
            messages_per_user = 10;
            total_spectrum_mhz = 11;
            channel_bandwidth_mhz = 1.0;
            users_per_channel = 30;
            antenna_factor = 16;
            mimo_enabled = true;
            slot_count = 0;
            for (int i = 0; i < 11; ++i) {
                spectrum_slots[i].frequency_mhz = 1800 + (i * 1000);
                spectrum_slots[i].current_users = 0;
                spectrum_slots[i].max_users = 480;
                slot_count++;
            }
        }
    }

    bool isValidFrequency(int freq) const {
        for (int i = 0; i < slot_count; ++i) {
            if (spectrum_slots[i].frequency_mhz == freq) return true;
        }
        return false;
    }

    void addUser(int service_type, int freq) {
        if (user_count >= MAX_USERS) return;

        // Validate frequency and slot
        int slot_idx = -1;
        for (int i = 0; i < slot_count; ++i) {
            if (spectrum_slots[i].frequency_mhz == freq) {
                if (spectrum_slots[i].current_users >= spectrum_slots[i].max_users) {
                    io.errorstring("❌ ERROR: Frequency ");
                    io.errorint(freq);
                    io.errorstring(" MHz is full.\n");
                    return;
                }
                slot_idx = i;
                break;
            }
        }
        if (slot_idx == -1) {
            io.errorstring("❌ ERROR: Frequency ");
            io.errorint(freq);
            io.errorstring(" MHz is not valid for ");
            io.errorstring(getTechName());
            io.errorstring(" generation.\n");
            return;
        }

        // Compute messages
        int messages = messages_per_user;
        if (current_gen == 2) {
            if (service_type == 1) messages = 15;
            else if (service_type == 2) messages = 2;
            else if (service_type == 3) messages = 5;
            else messages = 20;
        } else if (current_gen == 5) {
            if (service_type == 1) messages = 15;
            else if (service_type == 2) messages = 2;
            else if (service_type == 3) messages = 25;
            else messages = 40;
        } else if (current_gen == 6) {
            if (service_type == 1) messages = 12;
            else if (service_type == 2) messages = 2;
            else if (service_type == 3) messages = 20;
            else messages = 32;
        } else if (current_gen == 7) {
            if (service_type == 1) messages = 10;
            else if (service_type == 2) messages = 2;
            else if (service_type == 3) messages = 25;
            else messages = 15;
        }

        // 🔥 CORE OVERHEAD CHECK: reject if core cannot handle
        if (!core->canRegister(messages)) {
            io.errorstring("❌ Rejected: Cellular core cannot accommodate additional messages due to overhead limit.\n");
            return;
        }

        // Register user
        users[user_count].setID(user_count + 1);
        users[user_count].setFrequency(freq);
        users[user_count].setMessages(messages);
        users[user_count].setServiceType(service_type);
        user_count++;
        spectrum_slots[slot_idx].current_users++;

        // 🔥 Register with core
        core->registerUser(users[user_count - 1].getID(), messages);

        io.outputstring("✅ User added successfully.\n");
    }

    void removeUser(int id) {
        if (id <= 0 || id > user_count) return;
        int idx = id - 1;
        int freq = users[idx].getFrequency();
        for (int i = 0; i < slot_count; ++i) {
            if (spectrum_slots[i].frequency_mhz == freq) {
                if (spectrum_slots[i].current_users > 0)
                    spectrum_slots[i].current_users--;
                break;
            }
        }
        for (int j = idx; j < user_count - 1; ++j)
            users[j] = users[j + 1];
        user_count--;
        // Note: core->current_load is not decremented (optional enhancement)
    }

    const char* getTechName() const { return tech_name; }
    const char* getProtocol() const { return protocol; }
    int getTotalSpectrumMHz() const { return total_spectrum_mhz; }
    int getUserCount() const { return user_count; }
    int getSlotCount() const { return slot_count; }
    int getSlotFrequency(int idx) const { return spectrum_slots[idx].frequency_mhz; }
    int getSlotUsers(int idx) const { return spectrum_slots[idx].current_users; }
    int getSlotMaxUsers(int idx) const { return spectrum_slots[idx].max_users; }
    int getMaxUsersBySpectrum() const {
        int channels = static_cast<int>(total_spectrum_mhz / channel_bandwidth_mhz);
        int total = channels * users_per_channel;
        if (mimo_enabled) total *= antenna_factor;
        return total;
    }
    int getCoresNeededForFull() const {
        long full_load = static_cast<long>(getMaxUsersBySpectrum()) * messages_per_user;
        long cap = core->getMaxCapacity();
        if (cap <= 0) return 1;
        return (full_load + cap - 1) / cap;
    }

    void getFirstChannelUsers(UserDevice out[], int& count) const {
        int cap = users_per_channel * (mimo_enabled ? antenna_factor : 1);
        count = (user_count < cap) ? user_count : cap;
        for (int i = 0; i < count; ++i) {
            out[i] = users[i];
        }
    }

    void getUsersOnFrequency(int freq, UserDevice out[], int& count) const {
        count = 0;
        for (int i = 0; i < user_count && count < 100; ++i) {
            if (users[i].getFrequency() == freq) {
                out[count] = users[i];
                ++count;
            }
        }
    }
};

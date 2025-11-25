#pragma once
#include "UserDevice.h"
#include "CellularCore.h"
#include "CellTower.h"

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

public:
    GenerationManager(int gen) : current_gen(gen), user_count(0), slot_count(0) {
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
            // 2G: 5 channels of 200 kHz = 1 MHz (1800, 2000, 2200, 2400, 2600 MHz)
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
            // 3G: Same as 2G (1800 MHz base)
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
            // 3.5G: Same as 2G/3G (1800 MHz base)
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
            channel_bandwidth_mhz = 0.01; // 10 kHz
            users_per_channel = 30;
            antenna_factor = 4;
            mimo_enabled = true;
            // 4G: 1 MHz = 100 channels of 10 kHz
            // Show 10 channels (every 10th) for readability
            slot_count = 0;
            for (int i = 0; i < 10; ++i) {
                spectrum_slots[i].frequency_mhz = 1800 + (i * 10); // 1800, 1810, 1820... MHz
                spectrum_slots[i].current_users = 0;
                spectrum_slots[i].max_users = 120; // 30 * 4 MIMO * 10 subchannels
                slot_count++;
            }
        } else if (gen == 6) {
            const char* t = "4G+";
            int i = 0; while (t[i]) { tech_name[i] = t[i]; ++i; } tech_name[i] = '\0';
            const char* p = "LTE-Advanced (Carrier Aggregation + OFDM)";
            i = 0; while (p[i] && i < 63) { protocol[i] = p[i]; ++i; } protocol[i] = '\0';
            messages_per_user = 8;
            total_spectrum_mhz = 1;
            channel_bandwidth_mhz = 0.01; // 10 kHz
            users_per_channel = 40;
            antenna_factor = 4;
            mimo_enabled = true;
            // 4G+: Same as 4G but different user count
            slot_count = 0;
            for (int i = 0; i < 10; ++i) {
                spectrum_slots[i].frequency_mhz = 1800 + (i * 10); // 1800, 1810, 1820... MHz
                spectrum_slots[i].current_users = 0;
                spectrum_slots[i].max_users = 160; // 40 * 4 MIMO * 10 subchannels
                slot_count++;
            }
        } else if (gen == 7) {
            const char* t = "5G";
            int i = 0; while (t[i]) { tech_name[i] = t[i]; ++i; } tech_name[i] = '\0';
            const char* p = "OFDM + Massive MIMO";
            i = 0; while (p[i] && i < 63) { protocol[i] = p[i]; ++i; } protocol[i] = '\0';
            messages_per_user = 10;
            total_spectrum_mhz = 11; // 1 MHz + 10 MHz
            channel_bandwidth_mhz = 1.0; // 1 MHz
            users_per_channel = 30;
            antenna_factor = 16;
            mimo_enabled = true;
            // 5G: 11 channels of 1 MHz starting at 1800 MHz
            slot_count = 0;
            for (int i = 0; i < 11; ++i) {
                spectrum_slots[i].frequency_mhz = 1800 + (i * 1000); // 1800, 2800, 3800... MHz
                spectrum_slots[i].current_users = 0;
                spectrum_slots[i].max_users = 480; // 30 * 16 MIMO
                slot_count++;
            }
        }
    }

    // 🔥 VALIDATE FREQUENCY IS IN RANGE FOR CURRENT GENERATION
    bool isValidFrequency(int freq) const {
        if (current_gen == 2 || current_gen == 3 || current_gen == 4) {
            // 2G/3G/3.5G: 1800 MHz band (5 channels of 200 kHz)
            for (int i = 0; i < 5; ++i) {
                if (spectrum_slots[i].frequency_mhz == freq) return true;
            }
            return false;
        } else if (current_gen == 5 || current_gen == 6) {
            // 4G/4G+: 1800 MHz band (10 channels for display)
            for (int i = 0; i < 10; ++i) {
                if (spectrum_slots[i].frequency_mhz == freq) return true;
            }
            return false;
        } else if (current_gen == 7) {
            // 5G: 11 channels of 1 MHz
            for (int i = 0; i < 11; ++i) {
                if (spectrum_slots[i].frequency_mhz == freq) return true;
            }
            return false;
        }
        return false;
    }

    void addUser(int service_type, int freq) {
        if (user_count >= MAX_USERS) return;

        // 🔥 VALIDATE FREQUENCY IS IN RANGE FOR THIS GENERATION
        bool valid_freq = false;
        for (int i = 0; i < slot_count; ++i) {
            if (spectrum_slots[i].frequency_mhz == freq) {
                if (spectrum_slots[i].current_users >= spectrum_slots[i].max_users) {
                    return; // Slot full
                }
                valid_freq = true;
                break;
            }
        }
        if (!valid_freq) return; // Invalid frequency

        int messages = messages_per_user;
        if (current_gen == 2) {
            if (service_type == 1) messages = 15;      // Voice
            else if (service_type == 2) messages = 2;  // SMS
            else if (service_type == 3) messages = 5;  // Data
            else messages = 20;                        // Voice+Data
        } else if (current_gen == 3) {
            messages = 10; // Fixed for 3G
        } else if (current_gen == 4) {
            messages = 8;  // Fixed for 3.5G
        } else if (current_gen == 5) {
            if (service_type == 1) messages = 15;      // Voice
            else if (service_type == 2) messages = 2;  // SMS
            else if (service_type == 3) messages = 25; // Data
            else messages = 40;                        // Voice+Data
        } else if (current_gen == 6) {
            if (service_type == 1) messages = 12;      // Voice
            else if (service_type == 2) messages = 2;  // SMS
            else if (service_type == 3) messages = 20; // Data
            else messages = 32;                        // Voice+Data
        } else if (current_gen == 7) {
            if (service_type == 1) messages = 10;      // Voice
            else if (service_type == 2) messages = 2;  // SMS
            else if (service_type == 3) messages = 25; // Data
            else messages = 15;                        // Voice+Data
        }

        users[user_count].setID(user_count + 1);
        users[user_count].setFrequency(freq);
        users[user_count].setMessages(messages);
        users[user_count].setServiceType(service_type);
        user_count++;

        // Update spectrum slot
        for (int i = 0; i < slot_count; ++i) {
            if (spectrum_slots[i].frequency_mhz == freq) {
                spectrum_slots[i].current_users++;
                break;
            }
        }
    }

    void removeUser(int id) {
        if (id <= 0 || id > user_count) return;
        int idx = id - 1;

        int freq = users[idx].getFrequency();
        // Update spectrum slot
        for (int i = 0; i < slot_count; ++i) {
            if (spectrum_slots[i].frequency_mhz == freq) {
                if (spectrum_slots[i].current_users > 0) {
                    spectrum_slots[i].current_users--;
                }
                break;
            }
        }

        // Shift users
        for (int j = idx; j < user_count - 1; ++j)
            users[j] = users[j + 1];
        user_count--;
    }

    // 🔥 GETTERS
    const char* getTechName() const { return tech_name; }
    const char* getProtocol() const { return protocol; }
    int getMessagesPerUser() const { return messages_per_user; }
    int getTotalSpectrumMHz() const { return total_spectrum_mhz; }
    int getMaxUsersBySpectrum() const {
        int channels = static_cast<int>(total_spectrum_mhz / channel_bandwidth_mhz);
        int total = channels * users_per_channel;
        if (mimo_enabled) total *= antenna_factor;
        return total;
    }
    int getUserCount() const { return user_count; }
    int getSlotCount() const { return slot_count; }
    int getSlotFrequency(int idx) const { return spectrum_slots[idx].frequency_mhz; }
    int getSlotUsers(int idx) const { return spectrum_slots[idx].current_users; }
    int getSlotMaxUsers(int idx) const { return spectrum_slots[idx].max_users; }
    double getChannelBandwidthMHz() const { return channel_bandwidth_mhz; }
    int getUsersPerChannel() const { return users_per_channel; }
    int getCoresNeededForFull() const {
        long full_load = static_cast<long>(getMaxUsersBySpectrum()) * messages_per_user;
        long cap = 10000;
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

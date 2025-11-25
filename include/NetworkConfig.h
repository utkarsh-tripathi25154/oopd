#pragma once

class NetworkConfig {
public:
    char name[16];
    double total_spectrum_mhz;
    double channel_bandwidth_mhz;
    int users_per_channel;
    int messages_per_user;
    int antenna_factor;
    bool mimo_enabled;

    NetworkConfig() {
        name[0] = '\0';
        total_spectrum_mhz = 1.0;
        channel_bandwidth_mhz = 0.2;
        users_per_channel = 16;
        messages_per_user = 15;
        antenna_factor = 1;
        mimo_enabled = false;
    }

    static NetworkConfig for2G() {
        NetworkConfig c;
        c.setName("2G");
        c.total_spectrum_mhz = 1.0;
        c.channel_bandwidth_mhz = 0.2;   // 200 kHz
        c.users_per_channel = 16;
        c.messages_per_user = 15;
        c.antenna_factor = 1;
        c.mimo_enabled = false;
        return c;
    }

    static NetworkConfig for3G() {
        NetworkConfig c;
        c.setName("3G");
        c.total_spectrum_mhz = 1.0;
        c.channel_bandwidth_mhz = 0.2;
        c.users_per_channel = 32;
        c.messages_per_user = 10;
        c.antenna_factor = 1;
        c.mimo_enabled = false;
        return c;
    }

    static NetworkConfig for35G() {
        NetworkConfig c;
        c.setName("3.5G");
        c.total_spectrum_mhz = 1.0;
        c.channel_bandwidth_mhz = 0.2;
        c.users_per_channel = 64;
        c.messages_per_user = 8;
        c.antenna_factor = 1;
        c.mimo_enabled = false;
        return c;
    }

    static NetworkConfig for4G() {
        NetworkConfig c;
        c.setName("4G");
        c.total_spectrum_mhz = 1.0;
        c.channel_bandwidth_mhz = 0.01;  // 10 kHz
        c.users_per_channel = 30;
        c.messages_per_user = 10;
        c.antenna_factor = 4;
        c.mimo_enabled = true;
        return c;
    }

    static NetworkConfig for4GPlus() {
        NetworkConfig c;
        c.setName("4G+");
        c.total_spectrum_mhz = 1.0;
        c.channel_bandwidth_mhz = 0.01;
        c.users_per_channel = 40;
        c.messages_per_user = 8;
        c.antenna_factor = 4;
        c.mimo_enabled = true;
        return c;
    }

    static NetworkConfig for5G() {
        NetworkConfig c;
        c.setName("5G");
        c.total_spectrum_mhz = 11.0;     // 1 + 10 MHz
        c.channel_bandwidth_mhz = 1.0;
        c.users_per_channel = 30;
        c.messages_per_user = 10;
        c.antenna_factor = 16;
        c.mimo_enabled = true;
        return c;
    }

    int getMaxUsers() const {
        int channels = static_cast<int>(total_spectrum_mhz / channel_bandwidth_mhz);
        int total = channels * users_per_channel;
        if (mimo_enabled) total *= antenna_factor;
        return total;
    }

    int getCoresNeededForFull() const {
        long full_load = static_cast<long>(getMaxUsers()) * messages_per_user;
        long core_cap = 10000; // fixed core capacity
        if (core_cap <= 0) return 1;
        return (full_load + core_cap - 1) / core_cap;
    }

private:
    void setName(const char* n) {
        int i = 0;
        while (n[i] && i < 15) { name[i] = n[i]; ++i; }
        name[i] = '\0';
    }
};

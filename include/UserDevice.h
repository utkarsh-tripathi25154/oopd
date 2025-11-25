#pragma once

class UserDevice {
private:
    char user_id[32];
    int frequency_mhz;
    int messages;
    int service_type; // 1=Voice, 2=SMS, 3=Data, 4=Voice+Data

public:
    UserDevice() : frequency_mhz(0), messages(0), service_type(0) {
        user_id[0] = '\0';
    }

    void setID(int id) {
        user_id[0] = 'U';
        int_to_cstr(id, user_id + 1);
    }

    void setFrequency(int freq) { frequency_mhz = freq; }
    void setMessages(int msg) { messages = msg; }
    void setServiceType(int service) { service_type = service; } // 🔥 ADDED

    const char* getID() const { return user_id; }
    int getFrequency() const { return frequency_mhz; }
    int getMessages() const { return messages; }
    int getServiceType() const { return service_type; } // 🔥 ADDED

    void serialize(char* out) const {
        if (!out) return;
        int i = 0;
        const char* id = user_id;
        while (id[i] && i < 30) { out[i] = id[i]; ++i; }
        out[i++] = ',';
        int f = frequency_mhz;
        if (f == 0) { out[i++] = '0'; }
        else {
            char tmp[16]; int j = 0;
            int tf = f;
            do { tmp[j++] = '0' + (tf % 10); tf /= 10; } while (tf > 0);
            for (int k = j-1; k >= 0; --k) out[i++] = tmp[k];
        }
        out[i++] = ',';
        int m = messages;
        if (m == 0) { out[i++] = '0'; }
        else {
            char tmp[16]; int j = 0;
            int tm = m;
            do { tmp[j++] = '0' + (tm % 10); tm /= 10; } while (tm > 0);
            for (int k = j-1; k >= 0; --k) out[i++] = tmp[k];
        }
        out[i++] = ','; // 🔥 NEW: Add service type
        int s = service_type;
        if (s == 0) { out[i++] = '0'; }
        else {
            char tmp[16]; int j = 0;
            int ts = s;
            do { tmp[j++] = '0' + (ts % 10); ts /= 10; } while (ts > 0);
            for (int k = j-1; k >= 0; --k) out[i++] = tmp[k];
        }
        out[i++] = '\n';
        out[i] = '\0';
    }

    bool deserialize(const char* line) {
        if (!line) return false;
        int i = 0;
        while (line[i] != ',' && line[i] != '\0' && i < 31) {
            user_id[i] = line[i];
            ++i;
        }
        user_id[i] = '\0';
        if (line[i] != ',') return false;
        ++i;
        frequency_mhz = 0;
        while (line[i] >= '0' && line[i] <= '9') {
            frequency_mhz = frequency_mhz * 10 + (line[i] - '0');
            ++i;
        }
        if (line[i] != ',') return false;
        ++i;
        messages = 0;
        while (line[i] >= '0' && line[i] <= '9') {
            messages = messages * 10 + (line[i] - '0');
            ++i;
        }
        if (line[i] != ',') return false; // 🔥 NEW: Check for service type
        ++i;
        service_type = 0;
        while (line[i] >= '0' && line[i] <= '9') {
            service_type = service_type * 10 + (line[i] - '0');
            ++i;
        }
        return true;
    }

private:
    static void int_to_cstr(int num, char* buffer) {
        if (!buffer) return;
        if (num == 0) {
            buffer[0] = '0';
            buffer[1] = '\0';
            return;
        }
        int neg = 0;
        if (num < 0) { neg = 1; num = -num; }
        char digits[12];
        int count = 0;
        int t = num;
        do { digits[count++] = '0' + (t % 10); t /= 10; } while (t > 0);
        int i = 0;
        if (neg) buffer[i++] = '-';
        for (int j = count - 1; j >= 0; --j) buffer[i++] = digits[j];
        buffer[i] = '\0';
    }
};

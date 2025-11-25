#include "../include/basicIO.h"
#include "../include/exceptions.h"
#include "../include/InputValidator.h"
#include "../include/GenerationManager.h"

extern "C" long syscall3(long, long, long, long);

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_OPEN 2
#define SYS_CLOSE 3
#define O_RDONLY 0
#define O_WRONLY 1
#define O_CREAT 64
#define O_TRUNC 512

// 🔥 NO UNUSED FUNCTIONS (int_to_cstr, strings_equal removed)

GenerationManager* gen_managers[8] = {nullptr};
CellularCore<long>* gen_cores[8] = {nullptr}; // ADD THIS LINE

void runInteractiveMode();
void runFileMode();

extern "C" int main() {
    basicIO io;
    while (1) {
        try {
            io.outputstring("\n=== Cellular Network Simulator ===\n");
            io.outputstring("1. Interactive Mode (User-Driven)\n");
            io.outputstring("2. File Mode (Input File Simulation)\n");
            io.outputstring("3. Exit\n");
            io.outputstring("Choose mode (1-3): ");
            
            int mode = InputValidator::validateInt(1, 3);
            
            if (mode == 1) {
                runInteractiveMode();
            } else if (mode == 2) {
                runFileMode();
            } else if (mode == 3) {
                io.outputstring("Goodbye!\n");
                break;
            }
        }
        catch (const OutOfRangeException& e) {
            io.errorstring("ERROR: ");
            io.errorstring(e.what());
            io.terminate();
        }
        catch (const InvalidInputException& e) {
            io.errorstring("ERROR: ");
            io.errorstring(e.what());
            io.terminate();
        }
    }
    return 0;
}

void runInteractiveMode() {
    basicIO io;
    
    int current_gen_unused = 0;
    (void)current_gen_unused;
    GenerationManager* current_manager = nullptr;
    
    while (1) {
        io.outputstring("\n--- Select Network Technology ---\n");
        io.outputstring("2: 2G (TDMA) - Voice:15, Data:5 msgs\n");
        io.outputstring("3: 3G (CDMA) - Fixed 10 msgs\n");
        io.outputstring("4: 3.5G (HSPA) - Fixed 8 msgs\n");
        io.outputstring("5: 4G (OFDM) - Service-based messages\n");
        io.outputstring("6: 4G+ (LTE-A) - Enhanced capacity\n");
        io.outputstring("7: 5G (Massive MIMO) - 16x antenna reuse\n");
        io.outputstring("0: Return to Main Menu\n");
        io.outputstring("Choice (2-7, 0 to exit): ");
        
        int gen = InputValidator::validateInt(0, 7);
        if (gen == 0) return;
        
        if (gen < 2 || gen > 7) {
            io.outputstring("Invalid choice.\n");
            continue;
        }
        
        // Replace 10000 with 100
static CellularCore<long> shared_core(100); // 🔥 100 message limit
if (gen_managers[gen] == nullptr) {
    gen_cores[gen] = new CellularCore<long>(100);          // NEW: dedicated core
    gen_managers[gen] = new GenerationManager(gen, gen_cores[gen]);
}
        current_gen_unused = gen;
        current_manager = gen_managers[gen];
        
        while (1) {
            io.outputstring("\n[Interactive Mode - ");
            io.outputstring(current_manager->getTechName());
            io.outputstring("]\n");
            io.outputstring("1. Add User\n"); // 🔥 WAS "Remove User" - NOW "Add User"
            io.outputstring("2. Remove User\n");
            io.outputstring("3. View Spectrum Status\n");
            io.outputstring("4. View Users on Frequency\n");
            io.outputstring("5. View Network Stats\n");
            io.outputstring("6. Switch Technology\n");
            io.outputstring("7. Return to Main Menu\n");
            io.outputstring("Choice: ");
            
            int c = InputValidator::validateInt(1, 7);

            if (c == 1) { // 🔥 ADD USER (was Remove)
                if (current_manager->getUserCount() >= 1000) {
                    io.outputstring("❌ Max users (1000) reached.\n");
                    continue;
                }
                
                io.outputstring("\n--- Service Type ---\n");
                io.outputstring("1: Voice Call\n");
                io.outputstring("2: SMS\n");
                io.outputstring("3: Mobile Data\n");
                io.outputstring("4: Voice + Data\n");
                io.outputstring("Choice (1-4): ");
                int service = InputValidator::validateInt(1, 4);
                
                io.outputstring("\n--- Available Frequencies ---\n");
                bool has_space = false;
                for (int i = 0; i < current_manager->getSlotCount(); ++i) {
                    int used = current_manager->getSlotUsers(i);
                    int total = current_manager->getSlotMaxUsers(i);
                    if (used < total) {
                        io.outputstring("  ");
                        io.outputint(current_manager->getSlotFrequency(i));
                        io.outputstring(" MHz (");
                        io.outputint(used);
                        io.outputstring("/");
                        io.outputint(total);
                        io.outputstring(" users)\n");
                        has_space = true;
                    }
                }
                if (!has_space) {
                    io.outputstring("  (No frequencies with available space)\n");
                }
                io.outputstring("\nEnter Frequency (MHz): ");
                int freq = InputValidator::validateInt(1, 100000);
                
                if (!current_manager->isValidFrequency(freq)) {
                    io.errorstring("❌ ERROR: Frequency ");
                    io.errorint(freq);
                    io.errorstring(" MHz is not valid for ");
                    io.errorstring(current_manager->getTechName());
                    io.errorstring(" generation.\n");
                    continue;
                }
                
                current_manager->addUser(service, freq);
                
              
            } 
            else if (c == 2) { // 🔥 REMOVE USER (was Add)
                if (current_manager->getUserCount() == 0) {
                    io.outputstring("No users to remove.\n");
                    continue;
                }
                io.outputstring("User ID to remove (1-");
                io.outputint(current_manager->getUserCount());
                io.outputstring("): ");
                int id = InputValidator::validateInt(1, current_manager->getUserCount());
                current_manager->removeUser(id);
                io.outputstring("User removed.\n");
            } 
            else if (c == 3) {
                io.outputstring("\n--- SPECTRUM STATUS ---\n");
                for (int i = 0; i < current_manager->getSlotCount(); ++i) {
                    int used = current_manager->getSlotUsers(i);
                    int total = current_manager->getSlotMaxUsers(i);
                    io.outputstring("  ");
                    io.outputint(current_manager->getSlotFrequency(i));
                    io.outputstring(" MHz: ");
                    io.outputint(used);
                    io.outputstring("/");
                    io.outputint(total);
                    io.outputstring(" users (");
                    if (total > 0) {
                        io.outputint((used * 100) / total);
                        io.outputstring("%)");
                    } else {
                        io.outputstring("Invalid%)");
                    }
                    io.terminate();
                }
            }
            else if (c == 4) {
                io.outputstring("Frequency to query (MHz): ");
                int f = InputValidator::validateInt(1, 100000);
                
                bool valid_freq = false;
                for (int i = 0; i < current_manager->getSlotCount(); ++i) {
                    if (current_manager->getSlotFrequency(i) == f) {
                        valid_freq = true;
                        break;
                    }
                }
                if (!valid_freq) {
                    io.errorstring("❌ ERROR: Frequency ");
                    io.errorint(f);
                    io.errorstring(" MHz is not valid for ");
                    io.errorstring(current_manager->getTechName());
                    io.errorstring(" generation.\n");
                    continue;
                }
                
                UserDevice users_on_freq[100];
                int count = 0;
                current_manager->getUsersOnFrequency(f, users_on_freq, count);
                
                io.outputstring("\nUsers on ");
                io.outputint(f);
                io.outputstring(" MHz:\n");
                if (count == 0) {
                    io.outputstring("  (None)\n");
                } else {
                    for (int i = 0; i < count; ++i) {
                        io.outputstring("  ");
                        io.outputstring(users_on_freq[i].getID());
                        io.outputstring(" | ");
                        io.outputint(users_on_freq[i].getMessages());
                        io.outputstring(" msgs | ");
                        int service = users_on_freq[i].getServiceType();
                        if (service == 1) io.outputstring("Voice");
                        else if (service == 2) io.outputstring("SMS");
                        else if (service == 3) io.outputstring("Data");
                        else io.outputstring("Voice+Data");
                        io.terminate();
                    }
                }
            } 
            else if (c == 5) {
                io.outputstring("\n--- Network Configuration ---\n");
                io.outputstring("Technology: ");
                io.outputstring(current_manager->getTechName());
                io.terminate();
                io.outputstring("Protocol: ");
                io.outputstring(current_manager->getProtocol());
                io.terminate();
                io.outputstring("Spectrum: ");
                io.outputint(static_cast<int>(current_manager->getTotalSpectrumMHz()));
                io.outputstring(" MHz");
                io.terminate();
                io.outputstring("Max Users (Spectrum): ");
                io.outputint(current_manager->getMaxUsersBySpectrum());
                io.terminate();
                io.outputstring("Current Users: ");
                io.outputint(current_manager->getUserCount());
                io.terminate();
                io.outputstring("Cores Needed for Full Capacity: ");
                io.outputint(current_manager->getCoresNeededForFull());
                io.terminate();
            } 
            else if (c == 6) {
                break; // Return to technology selection
            } 
            else if (c == 7) {
                return; // Exit to main menu
            }
        }
    }
}


void runFileMode() {
    basicIO io;
    
    int local_gen = 0;

    // 🔥 SELECT GENERATION FIRST
    io.outputstring("\n--- Select Network Technology ---\n");
    io.outputstring("2: 2G (TDMA) - Voice:15, Data:5 msgs\n");
    io.outputstring("3: 3G (CDMA) - Fixed 10 msgs\n");
    io.outputstring("4: 3.5G (HSPA) - Fixed 8 msgs\n");
    io.outputstring("5: 4G (OFDM) - Service-based messages\n");
    io.outputstring("6: 4G+ (LTE-A) - Enhanced capacity\n");
    io.outputstring("7: 5G (Massive MIMO) - 16x antenna reuse\n");
    io.outputstring("0: Return to Main Menu\n");
    io.outputstring("Choice (2-7, 0 to exit): ");
    
    local_gen = InputValidator::validateInt(0, 7);
    if (local_gen == 0) return;
    
    if (local_gen < 2 || local_gen > 7) {
        io.outputstring("Invalid choice.\n");
        return;
    }
    
   CellularCore<long> local_core(100); // 🔥 100 message limit
// Independent core for file mode
GenerationManager local_manager(local_gen, &local_core);

    // 🔥 PROCESS INPUT.TXT FOR SELECTED GENERATION
    long fd = syscall3(SYS_OPEN, (long)"input.txt", O_RDONLY, 0);
    if (fd >= 0) {
        char file_buf[8192];
        long bytes = syscall3(SYS_READ, fd, (long)file_buf, 8191);
        syscall3(SYS_CLOSE, fd, 0, 0);
        if (bytes > 0) {
            file_buf[bytes] = '\0';
            int pos = 0;
            while (pos < bytes) {
                // Skip comments/empty lines
                while (pos < bytes && (file_buf[pos] == '#' || file_buf[pos] == '\n' || file_buf[pos] == '\r')) {
                    while (pos < bytes && file_buf[pos] != '\n') ++pos;
                    ++pos;
                }
                if (pos >= bytes) break;

                if (pos >= bytes || file_buf[pos] < '2' || file_buf[pos] > '7') {
                    while (pos < bytes && file_buf[pos] != '\n') ++pos;
                    ++pos;
                    continue;
                }
                
                int file_gen = file_buf[pos] - '0';
                ++pos;
                
                if (file_gen != local_gen) {
                    // Skip line if not current generation
                    while (pos < bytes && file_buf[pos] != '\n') ++pos;
                    ++pos;
                    continue;
                }
                
                // Skip whitespace
                while (pos < bytes && (file_buf[pos] == ' ' || file_buf[pos] == '\t')) ++pos;
                if (pos >= bytes) break;
                
                // Parse service
                if (file_buf[pos] < '1' || file_buf[pos] > '4') {
                    while (pos < bytes && file_buf[pos] != '\n') ++pos;
                    ++pos;
                    continue;
                }
                int service = file_buf[pos] - '0';
                ++pos;
                
                // Skip whitespace
                while (pos < bytes && (file_buf[pos] == ' ' || file_buf[pos] == '\t')) ++pos;
                if (pos >= bytes) break;
                
                // Parse frequency
                int freq = 0;
                while (pos < bytes && file_buf[pos] >= '0' && file_buf[pos] <= '9') {
                    freq = freq * 10 + (file_buf[pos] - '0');
                    ++pos;
                }
                
                // Skip to next line
                while (pos < bytes && file_buf[pos] != '\n') ++pos;
                ++pos;
                
                // 🔥 ADD USER TO LOCAL MANAGER
                local_manager.addUser(service, freq);
            }
        }
    }

    // 🔥 PERSISTENT FILE MODE MENU
    while (1) {
        try {
            io.outputstring("\n[File Mode - ");
            io.outputstring(local_manager.getTechName());
            io.outputstring("]\n");
            io.outputstring("1. Remove User\n");
            io.outputstring("2. View Spectrum Status\n");
            io.outputstring("3. View Users on Frequency\n");
            io.outputstring("4. View Network Stats\n");
            io.outputstring("5. Switch Technology\n");
            io.outputstring("6. Save & Return to Main Menu\n");
            io.outputstring("Choice: ");
            
            int c = InputValidator::validateInt(1, 6);

            if (c == 1) { // 🔥 REMOVE USER (PRESERVES OTHER GENERATIONS)
                if (local_manager.getUserCount() == 0) {
                    io.outputstring("No users to remove.\n");
                    continue;
                }
                io.outputstring("User ID to remove (1-");
                io.outputint(local_manager.getUserCount());
                io.outputstring("): ");
                int id = InputValidator::validateInt(1, local_manager.getUserCount());
                
                // 🔥 PRESERVE ALL GENERATIONS: READ ENTIRE FILE, MODIFY ONLY CURRENT GEN
                long orig_fd = syscall3(SYS_OPEN, (long)"input.txt", O_RDONLY, 0);
                if (orig_fd < 0) {
                    // No file exists - create with current gen only
                    local_manager.removeUser(id);
                    io.outputstring("User removed.\n");
                    continue;
                }
                
                char orig_buf[8192];
                long orig_bytes = syscall3(SYS_READ, orig_fd, (long)orig_buf, 8191);
                syscall3(SYS_CLOSE, orig_fd, 0, 0);
                
                int temp_fd = syscall3(SYS_OPEN, (long)"temp_input.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (temp_fd >= 0) {
                    int orig_pos = 0;
                    int user_counter = 0;
                    while (orig_pos < orig_bytes) {
                        // Copy comments/empty lines as-is
                        if (orig_buf[orig_pos] == '#') {
                            while (orig_pos < orig_bytes && orig_buf[orig_pos] != '\n') {
                                char ch = orig_buf[orig_pos];
                                syscall3(SYS_WRITE, temp_fd, (long)&ch, 1);
                                ++orig_pos;
                            }
                            if (orig_pos < orig_bytes) {
                                char nl = '\n';
                                syscall3(SYS_WRITE, temp_fd, (long)&nl, 1);
                                ++orig_pos;
                            }
                            continue;
                        }
                        
                        if (orig_pos >= orig_bytes || orig_buf[orig_pos] < '2' || orig_buf[orig_pos] > '7') {
                            // Copy non-data lines as-is
                            while (orig_pos < orig_bytes && orig_buf[orig_pos] != '\n') {
                                char ch = orig_buf[orig_pos];
                                syscall3(SYS_WRITE, temp_fd, (long)&ch, 1);
                                ++orig_pos;
                            }
                            if (orig_pos < orig_bytes) {
                                char nl = '\n';
                                syscall3(SYS_WRITE, temp_fd, (long)&nl, 1);
                                ++orig_pos;
                            }
                            continue;
                        }
                        
                        int file_gen = orig_buf[orig_pos] - '0';
                        int line_start = orig_pos;
                        while (orig_pos < orig_bytes && orig_buf[orig_pos] != '\n') ++orig_pos;
                        
                        if (file_gen != local_gen) {
                            // 🔥 COPY OTHER GENERATIONS AS-IS
                            char line[256];
                            int len = orig_pos - line_start;
                            if (len > 254) len = 254;
                            for (int k = 0; k < len; ++k) line[k] = orig_buf[line_start + k];
                            line[len] = '\n';
                            syscall3(SYS_WRITE, temp_fd, (long)line, len + 1);
                        } else {
                            // 🔥 PROCESS CURRENT GENERATION: ONLY WRITE IF NOT THE USER BEING DELETED
                            ++user_counter;
                            if (user_counter != id) {
                                char line[256];
                                int len = orig_pos - line_start;
                                if (len > 254) len = 254;
                                for (int k = 0; k < len; ++k) line[k] = orig_buf[line_start + k];
                                line[len] = '\n';
                                syscall3(SYS_WRITE, temp_fd, (long)line, len + 1);
                            }
                        }
                        
                        if (orig_pos < orig_bytes && orig_buf[orig_pos] == '\n') ++orig_pos;
                    }
                    
                    syscall3(SYS_CLOSE, temp_fd, 0, 0);
                    
                    // Replace original file with temp file
                    long orig_write_fd = syscall3(SYS_OPEN, (long)"input.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (orig_write_fd >= 0) {
                        long temp_read_fd = syscall3(SYS_OPEN, (long)"temp_input.txt", O_RDONLY, 0);
                        if (temp_read_fd >= 0) {
                            char temp_buf[8192];
                            long temp_bytes = syscall3(SYS_READ, temp_read_fd, (long)temp_buf, 8191);
                            syscall3(SYS_WRITE, orig_write_fd, (long)temp_buf, temp_bytes);
                            syscall3(SYS_CLOSE, temp_read_fd, 0, 0);
                        }
                        syscall3(SYS_CLOSE, orig_write_fd, 0, 0);
                    }
                }
                
                local_manager.removeUser(id);
                io.outputstring("User removed and file updated.\n");
            } 
            else if (c == 2) { // 🔥 VIEW SPECTRUM STATUS
                io.outputstring("\n--- SPECTRUM STATUS ---\n");
                for (int i = 0; i < local_manager.getSlotCount(); ++i) {
                    int used = local_manager.getSlotUsers(i);
                    int total = local_manager.getSlotMaxUsers(i);
                    io.outputstring("  ");
                    io.outputint(local_manager.getSlotFrequency(i));
                    io.outputstring(" MHz: ");
                    io.outputint(used);
                    io.outputstring("/");
                    io.outputint(total);
                    io.outputstring(" users (");
                    if (total > 0) {
                        io.outputint((used * 100) / total);
                        io.outputstring("%)");
                    } else {
                        io.outputstring("Invalid%)");
                    }
                    io.terminate();
                }
            }
            else if (c == 3) { // 🔥 VIEW USERS ON FREQUENCY
                io.outputstring("Frequency to query (MHz): ");
                int f = InputValidator::validateInt(1, 100000);
                
                bool valid_freq = false;
                for (int i = 0; i < local_manager.getSlotCount(); ++i) {
                    if (local_manager.getSlotFrequency(i) == f) {
                        valid_freq = true;
                        break;
                    }
                }
                if (!valid_freq) {
                    io.errorstring("❌ ERROR: Frequency ");
                    io.errorint(f);
                    io.errorstring(" MHz is not valid for ");
                    io.errorstring(local_manager.getTechName());
                    io.errorstring(" generation.\n");
                    continue;
                }
                
                UserDevice users_on_freq[100];
                int count = 0;
                local_manager.getUsersOnFrequency(f, users_on_freq, count);
                
                io.outputstring("\nUsers on ");
                io.outputint(f);
                io.outputstring(" MHz:\n");
                if (count == 0) {
                    io.outputstring("  (None)\n");
                } else {
                    for (int i = 0; i < count; ++i) {
                        io.outputstring("  ");
                        io.outputstring(users_on_freq[i].getID());
                        io.outputstring(" | ");
                        io.outputint(users_on_freq[i].getMessages());
                        io.outputstring(" msgs | ");
                        int service = users_on_freq[i].getServiceType();
                        if (service == 1) io.outputstring("Voice");
                        else if (service == 2) io.outputstring("SMS");
                        else if (service == 3) io.outputstring("Data");
                        else io.outputstring("Voice+Data");
                        io.terminate();
                    }
                }
            } 
            else if (c == 4) { // 🔥 VIEW NETWORK STATS
                io.outputstring("\n--- Network Configuration ---\n");
                io.outputstring("Technology: ");
                io.outputstring(local_manager.getTechName());
                io.terminate();
                io.outputstring("Protocol: ");
                io.outputstring(local_manager.getProtocol());
                io.terminate();
                io.outputstring("Spectrum: ");
                io.outputint(static_cast<int>(local_manager.getTotalSpectrumMHz()));
                io.outputstring(" MHz");
                io.terminate();
                io.outputstring("Max Users (Spectrum): ");
                io.outputint(local_manager.getMaxUsersBySpectrum());
                io.terminate();
                io.outputstring("Current Users: ");
                io.outputint(local_manager.getUserCount());
                io.terminate();
                io.outputstring("Cores Needed for Full Capacity: ");
                io.outputint(local_manager.getCoresNeededForFull());
                io.terminate();
            } 
            else if (c == 5) { // 🔥 SWITCH TECHNOLOGY
                runFileMode(); // Recursive call for new generation
                return; // Exit this instance
            } 
            else if (c == 6) { // 🔥 SAVE & EXIT
                io.outputstring("💾 File updated. Returning to main menu...\n");
                return; // Back to main menu
            }
        }
        catch (const CellularException& e) {
            io.errorstring("❌ ERROR: ");
            io.errorstring(e.what());
            io.terminate();
        }
    }
}

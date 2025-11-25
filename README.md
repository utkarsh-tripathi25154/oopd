# Cellular Network Simulator (2G–5G)

A console-based simulator that models capacity and spectrum usage across different cellular generations (2G, 3G, 3.5G, 4G, 4G+, 5G).  
It lets you add/remove users, assign them to frequencies, and inspect how spectrum and core capacity are utilized in each generation.

> **Note:** This project uses a custom `basicIO` layer and a raw `syscall3` interface instead of standard C++ I/O, so it is intended for a specific OS/emulator environment where those syscalls are provided. :contentReference[oaicite:0]{index=0}

---

## Features

- **Multiple cellular generations supported:**

  - 2G – TDMA
  - 3G – CDMA
  - 3.5G – HSPA
  - 4G – OFDM
  - 4G+ – LTE-Advanced
  - 5G – OFDM + Massive MIMO :contentReference[oaicite:1]{index=1}

- **Two modes of operation:**

  1. **Interactive Mode** – Fully menu-driven, add/remove users and inspect the network in real time.
  2. **File Mode** – Reads an `input.txt` file with pre-defined users, then lets you inspect and modify them. :contentReference[oaicite:2]{index=2}

- **Service-aware load modelling**

  Each user has a **service type** (Voice, SMS, Data, Voice+Data), which determines how many “messages” they generate depending on the generation. 

- **Spectrum slot modelling**

  - Each generation has different spectrum allocation and frequency slots.
  - Each slot has a maximum number of users based on:
    - Total spectrum
    - Channel bandwidth
    - Users per channel
    - MIMO antenna factor (for 4G, 4G+, and 5G) 

- **Capacity and core usage**

  - Theoretical maximum users given spectrum configuration.
  - Number of core “instances” needed to serve full load. 

- **Robust input validation**

  - Numeric range checks.
  - Invalid input and out-of-range exceptions.
  - Spectrum misuse detection. 

---

## Project Structure

- `main.cpp`  
  Entry point. Implements the main menu and both operation modes:
  - Mode 1 – Interactive mode (`runInteractiveMode`)
  - Mode 2 – File mode (`runFileMode`)
  - Mode 3 – Exit :contentReference[oaicite:7]{index=7}  

- `basicIO.h`  
  Low-level I/O wrapper using `syscall3` for read/write/exit. Provides:
  - `inputint`, `inputstring`
  - `outputint`, `outputstring`
  - `errorint`, `errorstring`
  - `terminate` :contentReference[oaicite:8]{index=8}  

- `exceptions.h`  
  Custom exception hierarchy for input and spectrum errors:
  - `CellularException` (base)
  - `InvalidInputException`
  - `OutOfRangeException`
  - `InvalidSpectrumException` :contentReference[oaicite:9]{index=9}  

- `InputValidator.h`  
  - `validateInt(min, max)` – reads from stdin via `syscall3`, ensures numeric and in range, throws `InvalidInputException` / `OutOfRangeException`.  
  - `validateSpectrum(assigned, max_allowed)` – throws `InvalidSpectrumException` if over-allocated.   

- `UserDevice.h`  
  Models a single user device:
  - Fields: `user_id`, `frequency_mhz`, `messages`, `service_type`.
  - Methods: `setID`, `setFrequency`, `setMessages`, `setServiceType`, `get*`.
  - CSV-like `serialize` / `deserialize` for file representation. :contentReference[oaicite:11]{index=11}  

- `CellularCore.h`  
  Core capacity model:
  - Tracks `max_capacity`, current load, registered users.
  - `canRegister`, `registerUser`, `reset`, `getMaxCapacity`, `getRegisteredCount`. :contentReference[oaicite:12]{index=12}  

- `CellTower.h`  
  Abstracts a cell tower for a given technology, with concrete subclasses:
  - `G2Tower`, `G3Tower`, `G35Tower`, `G4Tower`, `G4PlusTower`, `G5Tower`
  - Computes capacity based on spectrum, channel bandwidth, MIMO, users/channel. :contentReference[oaicite:13]{index=13}  

- `GenerationManager.h`  
  High-level per-generation controller used by the menus:
  - Configures tech name, protocol, messages per user, spectrum slots, etc. based on selected generation.
  - Keeps an array of `UserDevice` objects.
  - Methods to add/remove users, validate frequencies, list users on a given frequency, and compute stats.   

- `NetworkConfig.h`  
  A standalone configuration helper for different generations (not directly used in `main.cpp` but useful for extension / refactoring). :contentReference[oaicite:15]{index=15}  

- `Makefile`  
  Build rules for the project (target name and flags may vary; see file in project root).

- # `INTERACTIVE MODE O/P FILE`

- `interactive_state.bin`

- `sim_state.bin`

- # `FILE MODE O/P FILE`

- `input.txt`

- `temp_input.txt`

---
## MENU 1

`=== Cellular Network Simulator ===`

`1. Interactive Mode (User-Driven)`

`2. File Mode (Input File Simulation)`

`3. Exit`

`Choose mode (1-3):` 

## MENU 2

`--- Select Network Technology ---`

`2: 2G (TDMA) - Voice:15, Data:5 msgs`

`3: 3G (CDMA) - Fixed 10 msgs`

`4: 3.5G (HSPA) - Fixed 8 msgs`

`5: 4G (OFDM) - Service-based messages`

`6: 4G+ (LTE-A) - Enhanced capacity`

`7: 5G (Massive MIMO) - 16x antenna reuse`

`0: Return to Main Menu`

`Choice (2-7, 0 to exit):` 

## MENU 3

`[Interactive Mode - 2G]`

`1. Add User`

`2. Remove User`

`3. View Spectrum Status`

`4. View Users on Frequency`

`5. View Network Stats`

`6. Switch Technology`

`7. Return to Main Menu`

`Choice: 

---

## Building

From the project root (where the `Makefile` is), run:

```bash
make clean
make
./bin/cellsim-debug

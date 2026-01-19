# MSFS Bluetooth LE Bridge

Bridges Simionic G1000 hardware bezels to Microsoft Flight Simulator via Bluetooth LE, enabling direct button and encoder input without an iPad or extra screens. Ideal for VR setups.

## Quick Start

### Prerequisites

- **Visual Studio 2022** with "Desktop development with C++" workload (includes CMake and Ninja)
- **MSFS SDK** - In MSFS, enable Developer Mode, and download the SDK. Install to default location (`C:/MSFS 2024 SDK`)
- **Bluetooth adapter** on your PC

### Clone and Build

```
# Clone with submodules
git clone --recurse-submodules https://github.com/YOUR_USERNAME/MSFS_BluetoothLE_Bridge_Cpp.git
cd MSFS_BluetoothLE_Bridge_Cpp

# If you already cloned without submodules:
git submodule update --init --recursive
```

Open "x64 Native Tools Command Prompt for VS 2022" and run:

```
# Configure
cmake --preset debug

# Build
cmake --build --preset debug
```

### Run

```
# Main bridge application (requires MSFS running)
build\debug\msfs_ble_bridge.exe

# BLE scanner utility - discover nearby Bluetooth LE devices
build\debug\ble_scanner.exe
```

Use `ble_scanner` first to verify your Bluetooth adapter works and to find your G1000 bezel's device info.

## Troubleshooting

**Bluetooth adapter not found?** Verify you built 64-bit:
```
dumpbin /headers build\debug\msfs_ble_bridge.exe | findstr /C:"machine"
```

**CMake can't find SimConnect?** Ensure the SDK is installed at `C:/MSFS 2024 SDK`. If installed elsewhere, update `SIMCONNECT_SDK_ROOT` in `CMakePresets.json`.

## Development in VS2022

1. Open the folder in VS2022 (File > Open > Folder)
2. VS will auto-detect CMakePresets.json
3. Select "debug" or "release" preset from the configuration dropdown
4. Build with Ctrl+Shift+B, debug with F5

If CMake doesn't activate: Tools > Options > CMake > check "Use CMake when both CMakeLists.txt and solution are present"

## Background

The Simionic G1000 bezels normally work in two modes:
- **iPad mode**: Bezel sends inputs to iPad via BLE, iPad displays G1000 and relays to MSFS. VR users experience noticeable latency.
- **LCD mode**: Dedicated screens with USB serial. Extra hardware cost.

This project enables a third option: the bezels send BLE inputs directly to this bridge app, which updates MSFS via SimConnect/HVars with minimal latency.
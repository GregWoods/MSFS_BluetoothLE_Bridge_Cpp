#include <iostream>
#include <string>
#include <vector>

#include "simpleble/SimpleBLE.h"
#include "simpleble/Config.h"
#include "ble_session.h"

static std::string capabilities_string(SimpleBLE::Characteristic& chr) {
    std::string caps;
    if (chr.can_read())     caps += "read ";
    if (chr.can_write_request()) caps += "write ";
    if (chr.can_write_command()) caps += "write_no_resp ";
    if (chr.can_notify())   caps += "notify ";
    if (chr.can_indicate()) caps += "indicate ";
    if (caps.empty()) caps = "none";
    return caps;
}

static void print_services(SimpleBLE::Peripheral& p) {
    std::cout << "\n  Services and Characteristics:\n";
    try {
        auto services = p.services();
        if (services.empty()) {
            std::cout << "    (no services found)\n";
            return;
        }
        for (auto& service : services) {
            std::cout << "    Service: " << service.uuid() << "\n";
            for (auto& chr : service.characteristics()) {
                std::cout << "      Characteristic: " << chr.uuid()
                          << "  [" << capabilities_string(chr) << "]\n";
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "    Error enumerating services: " << e.what() << "\n";
    }
}

int main() {
    constexpr const char* DEVICE_IDENTIFIER = "SHB1000";
    constexpr int SCAN_TIMEOUT_SEC = 20;

    // WinRT configuration for reliable BLE operations
    SimpleBLE::Config::WinRT::experimental_use_own_mta_apartment = true;
    SimpleBLE::Config::WinRT::experimental_reinitialize_winrt_apartment_on_main_thread = true;

    auto adapter_opt = get_first_adapter();
    if (!adapter_opt) {
        std::cerr << "No Bluetooth adapter found." << std::endl;
        return EXIT_FAILURE;
    }
    auto adapter = *adapter_opt;

    std::vector<SimpleBLE::Peripheral> scanned;
    scanned.reserve(32);
    std::unordered_set<std::string> seen_addresses;

    adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral p) {
        if (!p.is_connectable()) return;
        auto addr = p.address();
        if (!addr.empty() && seen_addresses.insert(addr).second) {
            std::cout << "Found: " << p.identifier() << " [" << addr << "]" << std::endl;
            scanned.push_back(p);
        }
    });
    adapter.set_callback_on_scan_start([=]() { std::cout << "Scanning for " << SCAN_TIMEOUT_SEC << " seconds...\n" << std::endl; });
    adapter.set_callback_on_scan_stop([]() { std::cout << "\nScan complete." << std::endl; });

    adapter.scan_for(SCAN_TIMEOUT_SEC * 1000);

    // Filter to G1000 devices
    std::vector<SimpleBLE::Peripheral> targets;
    for (auto& p : scanned) {
        if (p.identifier() == DEVICE_IDENTIFIER) {
            targets.push_back(p);
        }
    }

    if (targets.empty()) {
        std::cerr << "No " << DEVICE_IDENTIFIER << " devices found." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "\nFound " << targets.size() << " " << DEVICE_IDENTIFIER << " device(s). Connecting to enumerate services...\n" << std::endl;

    for (auto& p : targets) {
        std::cout << "Device: " << p.identifier() << " [" << p.address() << "]" << std::flush;
        try {
            p.connect();
            std::cout << " - connected" << std::endl;
            print_services(p);
            p.disconnect();
        }
        catch (const std::exception& e) {
            std::cout << std::endl;
            std::cerr << "  Error: " << e.what() << std::endl;
        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}
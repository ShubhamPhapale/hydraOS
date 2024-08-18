#include <drivers/driver.h>

using namespace hydraos::drivers;

Driver::Driver() {
    // Initialize the driver
}

Driver::~Driver() {
    // Deinitialize the driver
}

void Driver::Activate() {
    // Activate the driver
}

int Driver::Reset() {
    // Reset the driver
    return 0;
}

void Driver::Deactivate() {
    // Deactivate the driver
}

DriverManager::DriverManager() {
    numDrivers = 0;
}

void DriverManager::AddDriver(Driver* driver) {
    drivers[numDrivers] = driver;
    numDrivers++;
}

void DriverManager::ActivateAll() {
    for (int i = 0; i < numDrivers; i++) {
        drivers[i]->Activate();
    }
}
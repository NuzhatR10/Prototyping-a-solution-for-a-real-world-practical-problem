#include <iostream> // For input/output operations
#include <thread> // For multi-threading support
#include <mutex> // For synchronization using mutex
#include <random> // For generating random numbers
#include <chrono> // For time-related functions
#include <condition_variable> // For conditional variable synchronization

std::mutex bridgeMutex; // Mutex for bridge synchronization
std::condition_variable northCV, southCV; // Condition variables for northbound and southbound farmers
bool isBridgeFree = true; // Flag indicating whether the bridge is free or occupied
int numNorthFarmers = 3; // Number of northbound farmers
int numSouthFarmers = 3; // Number of southbound farmers

void NorthboundFarmer(int id)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 1000)); // Simulate traveling time

    std::unique_lock<std::mutex> lock(bridgeMutex); // Acquire the lock on the bridgeMutex
    northCV.wait(lock, [] { return isBridgeFree; }); // Wait until the bridge is free
    isBridgeFree = false; // Set the bridge as occupied

    std::cout << "Northbound farmer " << id << " waiting to pass.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 1000)); // Simulate crossing the bridge

    std::cout << "Northbound farmer " << id << " passing.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 1000)); // Simulate crossing the bridge

    numNorthFarmers--; // Decrement the count of northbound farmers
    if (numNorthFarmers > 0)
        northCV.notify_one(); // Notify one waiting northbound farmer to pass

    numSouthFarmers--; // Decrement the count of southbound farmers
    southCV.notify_one(); // Notify one waiting southbound farmer to pass

    isBridgeFree = true; // Set the bridge as free

    std::cout << "Northbound farmer " << id << " unlocked the bridge.\n";
}

void SouthboundFarmer(int id)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 1000)); // Simulate traveling time

    std::unique_lock<std::mutex> lock(bridgeMutex); // Acquire the lock on the bridgeMutex
    southCV.wait(lock, [] { return isBridgeFree; }); // Wait until the bridge is free
    isBridgeFree = false; // Set the bridge as occupied

    std::cout << "Southbound farmer " << id << " waiting to pass.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 1000)); // Simulate crossing the bridge

    std::cout << "Southbound farmer " << id << " passing.\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 1000)); // Simulate crossing the bridge

    numSouthFarmers--; // Decrement the count of southbound farmers
    if (numSouthFarmers > 0)
        southCV.notify_one(); // Notify one waiting southbound farmer to pass

    numNorthFarmers--; // Decrement the count of northbound farmers
    northCV.notify_one(); // Notify one waiting northbound farmer to pass

    isBridgeFree = true; // Set the bridge as free

    std::cout << "Southbound farmer " << id << " unlocked the bridge.\n";
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr))); // Seed the random number generator

    std::thread northFarmers[numNorthFarmers]; // Array of threads for northbound farmers
    std::thread southFarmers[numSouthFarmers]; // Array of threads for southbound farmers

    for (int i = 0; i < numNorthFarmers; ++i)
        northFarmers[i] = std::thread(NorthboundFarmer, i + 1); // Create threads for northbound farmers

    for (int i = 0; i < numSouthFarmers; ++i)
        southFarmers[i] = std::thread(SouthboundFarmer, i + 1); // Create threads for southbound farmers

    for (int i = 0; i < numNorthFarmers; ++i)
        northFarmers[i].join(); // Wait for northbound farmer threads to finish

    for (int i = 0; i < numSouthFarmers; ++i)
        southFarmers[i].join(); // Wait for southbound farmer threads to finish

    return 0;
}

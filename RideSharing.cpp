#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <string>
#include <map>
#include <fstream>
#include <ctime>

using namespace std;

const string ADMIN_PASSWORD = "nandinee123";
const string DRIVER_FILE = "drivers.txt";
const string USER_FILE = "users.txt";

// Structs
struct Driver {
    string name;
    string phone;
    string carModel;
    string locationArea;
};

struct User {
    string name;
    string pickupArea;
    string destinationArea;
    string rideDate;  // New field
};

// Global vectors
vector<Driver> drivers;
vector<User> users;

// Area Coordinates
map<string, pair<int, int>> areaCoordinates = {
    {"Shibabari", {5, 5}},
    {"Fulbari", {10, 10}},
    {"Khalishpur", {8, 9}},
    {"Shonadanga", {2, 7}},
    {"Rupsha", {12, 14}},
};

// Utility: Get current date
string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[11];
    sprintf(buf, "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    return string(buf);
}

// Load drivers from file
void loadDrivers() {
    ifstream fin(DRIVER_FILE);
    if (!fin) return;

    Driver d;
    while (getline(fin, d.name)) {
        getline(fin, d.phone);
        getline(fin, d.carModel);
        getline(fin, d.locationArea);
        drivers.push_back(d);
    }
    fin.close();
}

// Save drivers to file
void saveDrivers() {
    ofstream fout(DRIVER_FILE);
    for (const auto& d : drivers) {
        fout << d.name << '\n'
             << d.phone << '\n'
             << d.carModel << '\n'
             << d.locationArea << '\n';
    }
    fout.close();
}

// Load users from file
void loadUsers() {
    ifstream fin(USER_FILE);
    if (!fin) return;

    User u;
    while (getline(fin, u.name)) {
        getline(fin, u.pickupArea);
        getline(fin, u.destinationArea);
        getline(fin, u.rideDate);
        users.push_back(u);
    }
    fin.close();
}

// Save users to file
void saveUsers() {
    ofstream fout(USER_FILE);
    for (const auto& u : users) {
        fout << u.name << '\n'
             << u.pickupArea << '\n'
             << u.destinationArea << '\n'
             << u.rideDate << '\n';
    }
    fout.close();
}

// Distance calculation
double calculateDistance(string area1, string area2) {
    if (areaCoordinates.find(area1) == areaCoordinates.end() ||
        areaCoordinates.find(area2) == areaCoordinates.end()) {
        return -1;
    }

    auto [x1, y1] = areaCoordinates[area1];
    auto [x2, y2] = areaCoordinates[area2];
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

void showAvailableAreas() {
    cout << "Available Areas:\n";
    for (const auto& area : areaCoordinates) {
        cout << "- " << area.first << endl;
    }
    cout << endl;
}

// Register Driver
void registerDriver() {
    Driver d;
    cout << "Enter Driver Name: ";
    cin >> ws;
    getline(cin, d.name);

    cout << "Enter Phone: ";
    cin >> d.phone;

    cout << "Enter Car Model: ";
    cin >> ws;
    getline(cin, d.carModel);

    showAvailableAreas();
    cout << "Enter Current Location Area (exact name): ";
    cin >> ws;
    getline(cin, d.locationArea);

    if (areaCoordinates.find(d.locationArea) == areaCoordinates.end()) {
        cout << "Invalid area. Registration failed." << endl;
        return;
    }

    drivers.push_back(d);
    saveDrivers();
    cout << "Driver Registered Successfully!" << endl;
}

// User Panel
void userPanel() {
    User u;
    cout << "Enter Your Name: ";
    cin >> ws;
    getline(cin, u.name);

    showAvailableAreas();
    cout << "Enter Pickup Area (exact name): ";
    cin >> ws;
    getline(cin, u.pickupArea);

    cout << "Enter Destination Area (exact name): ";
    cin >> ws;
    getline(cin, u.destinationArea);

    if (areaCoordinates.find(u.pickupArea) == areaCoordinates.end() ||
        areaCoordinates.find(u.destinationArea) == areaCoordinates.end()) {
        cout << "Invalid area. Please try again." << endl;
        return;
    }

    u.rideDate = getCurrentDate();
    users.push_back(u);
    saveUsers();

    if (drivers.empty()) {
        cout << "No drivers available." << endl;
        return;
    }

    vector<pair<double, size_t>> dist_idx;
    for (size_t i = 0; i < drivers.size(); ++i) {
        double dist = calculateDistance(u.pickupArea, drivers[i].locationArea);
        dist_idx.push_back({dist, i});
    }

    sort(dist_idx.begin(), dist_idx.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    cout << "\nNearby Drivers (based on pickup):" << endl;
    for (const auto& p : dist_idx) {
        double dist = p.first;
        const Driver& d = drivers[p.second];
        cout << "Driver: " << d.name << " -- Car: " << d.carModel
             << " -- Phone: " << d.phone << " -- Current Area: " << d.locationArea
             << " -- Distance to pickup: " << dist << endl;
    }
}

// Admin Panel
void adminPanel() {
    const int MAX_ATTEMPTS = 3;
    string inputPass;
    bool authorized = false;

    for (int attempt = 1; attempt <= MAX_ATTEMPTS; ++attempt) {
        cout << "Enter Admin Password (attempt " << attempt << " of " << MAX_ATTEMPTS << "): ";
        cin >> inputPass;

        if (inputPass == ADMIN_PASSWORD) {
            authorized = true;
            break;
        } else {
            cout << "Incorrect password." << endl;
        }
    }

    if (!authorized) {
        cout << "Failed to authenticate. Returning to main menu." << endl;
        return;
    }

    int choice;
    while (true) {
        cout << "\n--- Admin Panel ---\n";
        cout << "1. View All Drivers\n";
        cout << "2. Remove Driver\n";
        cout << "3. View Users\n";
        cout << "4. Exit Admin Panel\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            if (drivers.empty()) {
                cout << "No drivers registered." << endl;
            } else {
                for (size_t i = 0; i < drivers.size(); ++i) {
                    cout << i + 1 << ". " << drivers[i].name << " | " << drivers[i].carModel
                         << " | Phone: " << drivers[i].phone
                         << " | Location Area: " << drivers[i].locationArea << endl;
                }
            }
        } else if (choice == 2) {
            if (drivers.empty()) {
                cout << "No drivers to remove." << endl;
                continue;
            }
            int idx;
            cout << "Enter driver number to remove: ";
            cin >> idx;
            if (idx > 0 && idx <= (int)drivers.size()) {
                drivers.erase(drivers.begin() + idx - 1);
                saveDrivers();
                cout << "Driver removed successfully." << endl;
            } else {
                cout << "Invalid index." << endl;
            }
        } else if (choice == 3) {
            if (users.empty()) {
                cout << "No users." << endl;
            } else {
                for (const auto& u : users) {
                    cout << "User: " << u.name
                         << " | Pickup: " << u.pickupArea
                         << " | Destination: " << u.destinationArea
                         << " | Ride Date: " << u.rideDate << endl;
                }
            }
        } else if (choice == 4) {
            break;
        } else {
            cout << "Invalid choice." << endl;
        }
    }
}

// Main Menu
int main() {
    loadDrivers();
    loadUsers();

    int choice;
    while (true) {
        cout << "\n--- Ride Sharing System ---\n";
        cout << "1. Admin Panel\n";
        cout << "2. Register as Driver\n";
        cout << "3. User Panel\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) adminPanel();
        else if (choice == 2) registerDriver();
        else if (choice == 3) userPanel();
        else if (choice == 4) break;
        else cout << "Invalid choice" << endl;
    }

    return 0;
}

#include<iostream>
#include<algorithm>
#include<vector>
#include<cmath>
#include<string>
#include<map>
#include<fstream>
#include<ctime>

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
    string rideDate;
    string bookedDriver; // New field
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

// Get current date
string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[11];
    sprintf(buf, "%04d-%02d-%02d", 1900 + ltm->tm_year, 1 + ltm->tm_mon, ltm->tm_mday);
    return string(buf);
}

// Load drivers
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

// Save drivers
void saveDrivers() {
    ofstream fout(DRIVER_FILE);
    for (auto& d : drivers) {
        fout << d.name << '\n' << d.phone << '\n' << d.carModel << '\n' << d.locationArea << '\n';
    }
    fout.close();
}

// Load users
void loadUsers() {
    ifstream fin(USER_FILE);
    if (!fin) return;
    User u;
    while (getline(fin, u.name)) {
        getline(fin, u.pickupArea);
        getline(fin, u.destinationArea);
        getline(fin, u.rideDate);
        getline(fin, u.bookedDriver);
        users.push_back(u);
    }
    fin.close();
}

// Save users
void saveUsers() {
    ofstream fout(USER_FILE);
    for (auto& u : users) {
        fout << u.name << '\n'
             << u.pickupArea << '\n'
             << u.destinationArea << '\n'
             << u.rideDate << '\n'
             << u.bookedDriver << '\n';
    }
    fout.close();
}

// Distance calculation
double calculateDistance(string a1, string a2) {
    if (areaCoordinates.find(a1) == areaCoordinates.end() || areaCoordinates.find(a2) == areaCoordinates.end())
        return -1;
    auto [x1, y1] = areaCoordinates[a1];
    auto [x2, y2] = areaCoordinates[a2];
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Show areas
void showAvailableAreas() {
    cout << "Available Areas:\n";
    for (auto& a : areaCoordinates) cout << "- " << a.first << endl;
    cout << endl;
}

// Register Driver
void registerDriver() {
    Driver d;
    cout << "Enter Driver Name: ";
    cin >> ws; getline(cin, d.name);
    cout << "Enter Phone: "; cin >> d.phone;
    cout << "Enter Car Model: "; cin >> ws; getline(cin, d.carModel);
    showAvailableAreas();
    cout << "Enter Current Location Area: "; cin >> ws; getline(cin, d.locationArea);

    if (areaCoordinates.find(d.locationArea) == areaCoordinates.end()) {
        cout << "Invalid area. Registration failed.\n";
        return;
    }

    drivers.push_back(d);
    saveDrivers();
    cout << "Driver Registered Successfully!\n";
}

// User Panel
void userPanel() {
    if (drivers.empty()) {
        cout << "No drivers available.\n";
        return;
    }

    User u;
    cout << "Enter Your Name: ";
    cin >> ws;
    getline(cin, u.name);

    showAvailableAreas();

    cout << "Enter Pickup Area: ";
    getline(cin, u.pickupArea);

    cout << "Enter Destination Area: ";
    getline(cin, u.destinationArea);

    // Validate areas
    if (areaCoordinates.find(u.pickupArea) == areaCoordinates.end() ||
        areaCoordinates.find(u.destinationArea) == areaCoordinates.end()) {
        cout << "Invalid area name entered.\n";
        return;
    }

    u.rideDate = getCurrentDate();

    // Sort drivers by distance from pickup point
    vector<pair<double, size_t>> dist_idx;
    for (size_t i = 0; i < drivers.size(); i++) {
        double dist = calculateDistance(u.pickupArea, drivers[i].locationArea);
        if (dist < 0) continue; // invalid area skip
        dist_idx.push_back({dist, i});
    }

    if (dist_idx.empty()) {
        cout << "No drivers available for your pickup area.\n";
        return;
    }

    sort(dist_idx.begin(), dist_idx.end());

    cout << "\nNearby Drivers:\n";
    cout << "-----------------------------------------------\n";
    for (size_t i = 0; i < dist_idx.size(); i++) {
        const Driver& d = drivers[dist_idx[i].second];
        cout << i + 1 << ". " << d.name
             << " | Car: " << d.carModel
             << " | Phone: " << d.phone
             << " | Area: " << d.locationArea
             << " | Distance: " << dist_idx[i].first << endl;
    }
    cout << "-----------------------------------------------\n";

    int choice;
    cout << "\nSelect a driver number to book: ";

    // 🧠 Safe input check
    if (!(cin >> choice)) {
        cin.clear();                 // reset cin error state
        cin.ignore(10000, '\n');     // flush bad input
        cout << " Invalid input. Please enter a number.\n";
        return;
    }

    // Range check
    if (choice < 1 || choice > (int)dist_idx.size()) {
        cout << "❌ Invalid choice number.\n";
        return;
    }

    // Assign booked driver
    u.bookedDriver = drivers[dist_idx[choice - 1].second].name;

    users.push_back(u);
    saveUsers();

    cout << "\nRide booked successfully!\n";
    cout << "Driver: " << u.bookedDriver << endl;
    cout << "Date: " << u.rideDate << endl;
    cout << "Pickup: " << u.pickupArea << " -- Destination: " << u.destinationArea << endl;
}

// Admin Panel
void adminPanel() {
    string pass;
    cout << "Enter Admin Password: ";
    cin >> pass;
    if (pass != ADMIN_PASSWORD) {
        cout << "Incorrect password.\n";
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
            if (drivers.empty()) cout << "No drivers.\n";
            else {
                for (size_t i = 0; i < drivers.size(); i++)
                    cout << i + 1 << ". " << drivers[i].name << " | "
                         << drivers[i].carModel << " | Phone: "
                         << drivers[i].phone << " | Area: "
                         << drivers[i].locationArea << endl;
            }
        }
        else if (choice == 2) {
            if (drivers.empty()) { cout << "No drivers to remove.\n"; continue; }
            int idx; cout << "Enter driver number: "; cin >> idx;
            if (idx >= 1 && idx <= (int)drivers.size()) {
                drivers.erase(drivers.begin() + idx - 1);
                saveDrivers();
                cout << "Driver removed.\n";
            } else cout << "Invalid index.\n";
        }
        else if (choice == 3) {
            if (users.empty()) cout << "No users.\n";
            else {
                for (auto& u : users) {
                    cout << "User: " << u.name
                         << " | Pickup: " << u.pickupArea
                         << " | Destination: " << u.destinationArea
                         << " | Date: " << u.rideDate
                         << " | Driver: " << (u.bookedDriver.empty() ? "None" : u.bookedDriver)
                         << endl;
                }
            }
        }
        else if (choice == 4) break;
        else cout << "Invalid choice.\n";
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
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) adminPanel();
        else if (choice == 2) registerDriver();
        else if (choice == 3) userPanel();
        else if (choice == 4) break;
        else cout << "Invalid choice.\n";
    }

    return 0;
}



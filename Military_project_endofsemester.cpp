#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>

using namespace std;

// ------------------- Enumerations -------------------
enum class AccessLevel { CONFIDENTIAL = 1, SECRET, TOP_SECRET, SCI };
enum class MilitaryBranch { ARMY, NAVY, AIR_FORCE, MARINES, COAST_GUARD, SPACE_FORCE };

// ------------------- BaseEntity Class (Abstract) -------------------
class BaseEntity {
public:
    virtual void displayInfo() const = 0;  // pure virtual function to display info
    virtual ~BaseEntity() {}
};

// ------------------- MilitaryRank -------------------
class MilitaryRank : public BaseEntity {
private:
    string name;
    int rankLevel;
    AccessLevel accessLevel;
    MilitaryBranch branch;
public:
    MilitaryRank(string n = "Private", int rl = 1, AccessLevel al = AccessLevel::CONFIDENTIAL,
                 MilitaryBranch b = MilitaryBranch::ARMY)
        : name(n), rankLevel(rl), accessLevel(al), branch(b) {}

    bool outranks(const MilitaryRank& other) const {
        return rankLevel > other.rankLevel;
    }

    AccessLevel getAccessLevel() const { return accessLevel; }
    MilitaryBranch getBranch() const { return branch; }

    void displayInfo() const override {
        cout << "Rank: " << name << " (Level: " << rankLevel << ", Access: " << static_cast<int>(accessLevel) << ")" << endl;
    }

    string toString() const {
        return name + " (Level: " + to_string(rankLevel) + ", Access: " + to_string(static_cast<int>(accessLevel)) + ")";
    }
};

// ------------------- Weapon -------------------
class Weapon : public BaseEntity {
private:
    string name, type;
    int damageRating, range, accuracy;
    AccessLevel requiredAccess;
public:
    Weapon(string n = "Rifle", string t = "Assault", int dmg = 50, int r = 300, int acc = 70,
           AccessLevel al = AccessLevel::CONFIDENTIAL)
        : name(n), type(t), damageRating(dmg), range(r), accuracy(acc), requiredAccess(al) {}

    string getName() const { return name; }
    AccessLevel getRequiredAccess() const { return requiredAccess; }

    void displayInfo() const override {
        cout << "Weapon: " << name << " (Type: " << type << ", Damage: " << damageRating << ", Range: " << range << " meters, Accuracy: " << accuracy << "%)" << endl;
    }

    string toString() const {
        return name + " (Type: " + type + ", Damage: " + to_string(damageRating) + ")";
    }

    // Overload the '+' operator to combine two weapons
    friend Weapon operator+(const Weapon& w1, const Weapon& w2) {
        return Weapon(w1.name + " & " + w2.name, "Combined", w1.damageRating + w2.damageRating, w1.range + w2.range, (w1.accuracy + w2.accuracy) / 2, w1.requiredAccess);
    }

    // Overload the '<<' operator for printing Weapon details
    friend ostream& operator<<(ostream& os, const Weapon& weapon) {
        os << "Weapon: " << weapon.name << " (Type: " << weapon.type << ", Damage: " << weapon.damageRating << ")";
        return os;
    }
};

// ------------------- Soldier -------------------
class Soldier : public MilitaryRank {
private:
    string id;
    string firstName, lastName;
    MilitaryRank rank;
    string specialization;
    int experienceYears;
    bool active;
    vector<string> skills;
    vector<Weapon> assignedWeapons;

public:
    Soldier(string i, string fn, string ln, MilitaryRank r, string spec = "Infantry", int exp = 0)
        : id(i), firstName(fn), lastName(ln), rank(r), specialization(spec), experienceYears(exp), active(true) {}

    void addSkill(const string& skill) { skills.push_back(skill); }
    void assignWeapon(const Weapon& weapon) { assignedWeapons.push_back(weapon); }
    void removeWeapon(const string& weaponName) {
        assignedWeapons.erase(remove_if(assignedWeapons.begin(), assignedWeapons.end(),
            [&](const Weapon& w) { return w.getName() == weaponName; }), assignedWeapons.end());
    }

    bool canAccess(const AccessLevel requiredAccess) const {
        return static_cast<int>(rank.getAccessLevel()) >= static_cast<int>(requiredAccess);
    }

    void displayInfo() const override {
        cout << "Soldier: " << firstName << " " << lastName << ", Rank: " << rank.toString() << endl;
    }

    string toString() const {
        return id + ": " + firstName + " " + lastName + " - " + rank.toString();
    }

    string getId() const { return id; }
    AccessLevel getAccessLevel() const { return rank.getAccessLevel(); }
    const vector<Weapon>& getWeapons() const { return assignedWeapons; }
};

// ------------------- Warzone -------------------
class Warzone : public BaseEntity {
private:
    string id, name, location, description;
    AccessLevel requiredAccessLevel;
public:
    Warzone(string i, string n, string loc, string desc, AccessLevel al)
        : id(i), name(n), location(loc), description(desc), requiredAccessLevel(al) {}

    bool canAccess(const Soldier* soldier) const { return soldier->canAccess(requiredAccessLevel); }

    void displayInfo() const override {
        cout << "Warzone: " << name << " located at " << location << ", Access Level: " << static_cast<int>(requiredAccessLevel) << endl;
    }

    string toString() const { return name + " at " + location; }
};

// ------------------- Inventory -------------------
class Inventory : public BaseEntity {
private:
    map<string, pair<Weapon, int>> weapons;
    map<string, pair<string, int>> supplies;
    AccessLevel requiredAccessLevel;

public:
    Inventory(AccessLevel al = AccessLevel::CONFIDENTIAL) : requiredAccessLevel(al) {}

    void addWeapon(const Weapon& weapon, int quantity) {
        string name = weapon.getName();
        if (weapons.count(name)) {
            weapons[name].second += quantity;
        } else {
            weapons[name] = make_pair(weapon, quantity);
        }
    }

    void removeWeapon(const string& weaponName, int quantity) {
        auto it = weapons.find(weaponName);
        if (it != weapons.end()) {
            it->second.second -= quantity;
            if (it->second.second <= 0) {
                weapons.erase(it);
            }
        }
    }

    void addSupply(const string& supplyId, const string& description, int quantity) {
        if (supplies.count(supplyId)) {
            supplies[supplyId].second += quantity;
        } else {
            supplies[supplyId] = make_pair(description, quantity);
        }
    }

    void removeSupply(const string& supplyId, int quantity) {
        auto it = supplies.find(supplyId);
        if (it != supplies.end()) {
            it->second.second -= quantity;
            if (it->second.second <= 0) {
                supplies.erase(it);
            }
        }
    }

    int getWeaponQuantity(const string& weaponName) const {
        auto it = weapons.find(weaponName);
        return (it != weapons.end()) ? it->second.second : 0;
    }

    int getSupplyQuantity(const string& supplyId) const {
        auto it = supplies.find(supplyId);
        return (it != supplies.end()) ? it->second.second : 0;
    }

    bool canAccess(const Soldier* soldier) const {
        return static_cast<int>(soldier->getAccessLevel()) >= static_cast<int>(requiredAccessLevel);
    }

    void displayInfo() const override {
        cout << "Inventory Access Level: " << static_cast<int>(requiredAccessLevel) << endl;
        cout << "Weapons:\n";
        for (const auto& [name, pair] : weapons) {
            cout << "- " << name << " x" << pair.second << "\n";
        }
        cout << "Supplies:\n";
        for (const auto& [id, pair] : supplies) {
            cout << "- " << id << ": " << pair.first << " x" << pair.second << "\n";
        }
    }

    string toString() const {
        string result = "Inventory:\nWeapons:\n";
        for (const auto& [name, pair] : weapons) {
            result += "- " + name + " x" + to_string(pair.second) + "\n";
        }
        result += "Supplies:\n";
        for (const auto& [id, pair] : supplies) {
            result += "- " + id + ": " + pair.first + " x" + to_string(pair.second) + "\n";
        }
        return result;
    }
};

// ------------------- MilitaryManagementSystem -------------------
class MilitaryManagementSystem {
private:
    map<string, unique_ptr<Soldier>> soldiers;
    map<string, Weapon> weaponTypes;
    map<string, Warzone*> warzones;
    Soldier* currentUser;
    Inventory inventory;

public:
    MilitaryManagementSystem();
    ~MilitaryManagementSystem();
    bool login(string soldierId);
    void logout();
    void run();
    void showHelp();
    void showRankInfo();
    void showBranchInfo();
    void showAccessLevelInfo();
    MilitaryRank createRank();
    Soldier* createSoldier();
    void addWeaponManually();
    void addWarzoneManually();
    void addDefaultWeaponsAndWarzones();
    void assignWeaponToSoldier();
    void assignWarzoneToSoldier();
    void displaySoldierInfo();
};

MilitaryManagementSystem::MilitaryManagementSystem() : currentUser(nullptr) {
    addDefaultWeaponsAndWarzones();
}

MilitaryManagementSystem::~MilitaryManagementSystem() {
    for (auto& pair : warzones) delete pair.second;
}

bool MilitaryManagementSystem::login(string soldierId) {
    auto it = soldiers.find(soldierId);
    if (it != soldiers.end()) {
        currentUser = it->second.get();
        return true;
    }
    return false;
}

void MilitaryManagementSystem::logout() {
    currentUser = nullptr;
    cout << "Logged out successfully.\n";
}

void MilitaryManagementSystem::showHelp() {
    cout << "\n--- Military Management System Commands ---\n";
    cout << "login - Log in with Soldier ID\n";
    cout << "create_soldier - Create a new soldier\n";
    cout << "create_rank - Create a new rank\n";
    cout << "add_weapon - Add a new weapon manually\n";
    cout << "add_warzone - Add a new warzone manually\n";
    cout << "assign_weapon - Assign a weapon to the logged-in soldier\n";
    cout << "assign_warzone - Assign a warzone to the logged-in soldier\n";
    cout << "display_soldier - Display the information of the logged-in soldier\n";
    cout << "view_inventory - View current inventory status (based on access level)\n";
    
    if (currentUser) {  // Only show logout option if logged in
        cout << "logout - Log out from the system\n";
    }

    cout << "exit - Exit the system\n\n";
}


void MilitaryManagementSystem::showRankInfo() {
    cout << "\n--- Available Rank Levels ---\n";
    cout << "1-10: Enlisted\n";
    cout << "11-20: Officers\n\n";
}

void MilitaryManagementSystem::showBranchInfo() {
    cout << "\n--- Available Military Branches ---\n";
    cout << "1: ARMY\n2: NAVY\n3: AIR_FORCE\n4: MARINES\n5: COAST_GUARD\n6: SPACE_FORCE\n\n";
}

void MilitaryManagementSystem::showAccessLevelInfo() {
    cout << "\n--- Available Access Levels ---\n";
    cout << "1: CONFIDENTIAL\n2: SECRET\n3: TOP_SECRET\n4: SCI\n\n";
}


MilitaryRank MilitaryManagementSystem::createRank() {
    showRankInfo(); showAccessLevelInfo(); showBranchInfo();
    
    string name;
    int rankLevel, accessLevel, branch;

    cout << "Enter Rank Name: "; cin >> name;
    cout << "Enter Rank Level: "; cin >> rankLevel;
    cout << "Enter Access Level: "; cin >> accessLevel;
    cout << "Enter Branch (1-6): "; cin >> branch;

    return MilitaryRank(name, rankLevel, static_cast<AccessLevel>(accessLevel), static_cast<MilitaryBranch>(branch));
}

Soldier* MilitaryManagementSystem::createSoldier() {
    string soldierId, firstName, lastName, specialization;
    int experience;
    MilitaryRank rank = createRank();
    
    cout << "Enter Soldier ID: "; cin >> soldierId;
    cout << "Enter First Name: "; cin >> firstName;
    cout << "Enter Last Name: "; cin >> lastName;
    cout << "Enter Specialization: "; cin >> specialization;
    cout << "Enter Years of Experience: "; cin >> experience;
    
    auto soldier = make_unique<Soldier>(soldierId, firstName, lastName, rank, specialization, experience);
    soldiers[soldierId] = move(soldier); // Store soldier in map
    return soldiers[soldierId].get();
}

void MilitaryManagementSystem::addWeaponManually() {
    string name, type;
    int damageRating, range, accuracy, accessLevel;

    cout << "Enter weapon name: ";
    cin >> name;
    cout << "Enter type: ";
    cin >> type;
    cout << "Enter damage rating: ";
    cin >> damageRating;
    cout << "Enter range: ";
    cin >> range;
    cout << "Enter accuracy: ";
    cin >> accuracy;
    cout << "Enter access level (0=LOW, 1=MEDIUM, 2=HIGH): ";
    cin >> accessLevel;

    Weapon newWeapon(name, type, damageRating, range, accuracy, static_cast<AccessLevel>(accessLevel));
    weaponTypes[name] = newWeapon;

    int quantity;
    cout << "Enter quantity to add to inventory: ";
    cin >> quantity;
    inventory.addWeapon(newWeapon, quantity); // 🔥 ADDED THIS LINE

    cout << "Weapon added successfully.\n";
}



void MilitaryManagementSystem::addWarzoneManually() {
    string id, name, location, description;
    int accessLevel;
    cout << "Enter warzone ID: "; cin >> id;
    cout << "Enter warzone name: "; cin >> name;
    cout << "Enter warzone location: "; cin >> location;
    cout << "Enter warzone description: "; cin >> description;
    cout << "Enter required access level (1-4): "; cin >> accessLevel;
    warzones[id] = new Warzone(id, name, location, description, static_cast<AccessLevel>(accessLevel));
}

void MilitaryManagementSystem::addDefaultWeaponsAndWarzones() {
    weaponTypes["Rifle"] = Weapon("Rifle", "Assault", 50, 300, 70, AccessLevel::CONFIDENTIAL);
    weaponTypes["Pistol"] = Weapon("Pistol", "Sidearm", 30, 100, 80, AccessLevel::SECRET);
    weaponTypes["Sniper"] = Weapon("Sniper", "Precision", 100, 600, 90, AccessLevel::TOP_SECRET);
    
    warzones["Z1"] = new Warzone("Z1", "Desert Storm", "Middle East", "Tense desert combat zone.", AccessLevel::TOP_SECRET);
    warzones["Z2"] = new Warzone("Z2", "Arctic Warfare", "Northern Region", "Cold and hazardous environment.", AccessLevel::SECRET);
}

void MilitaryManagementSystem::assignWeaponToSoldier() {
    string soldierId, weaponName;
    cout << "Enter Soldier ID: "; cin >> soldierId;
    cout << "Enter Weapon Name: "; cin >> weaponName;
    
    auto soldierIt = soldiers.find(soldierId);
    if (soldierIt != soldiers.end() && weaponTypes.find(weaponName) != weaponTypes.end()) {
        Soldier* soldier = soldierIt->second.get();
        Weapon weapon = weaponTypes[weaponName];
        
        if (soldier->canAccess(weapon.getRequiredAccess())) {
            soldier->assignWeapon(weapon);
            cout << "Weapon assigned successfully.\n";
        } else {
            cout << "Insufficient access level to assign this weapon.\n";
        }
    } else {
        cout << "Soldier or weapon not found.\n";
    }
}

void MilitaryManagementSystem::assignWarzoneToSoldier() {
    string soldierId, warzoneId;
    cout << "Enter Soldier ID: "; cin >> soldierId;
    cout << "Enter Warzone ID: "; cin >> warzoneId;
    
    auto soldierIt = soldiers.find(soldierId);
    if (soldierIt != soldiers.end() && warzones.find(warzoneId) != warzones.end()) {
        Soldier* soldier = soldierIt->second.get();
        Warzone* warzone = warzones[warzoneId];
        
        if (warzone->canAccess(soldier)) {
            cout << "Warzone assigned to soldier.\n";
        } else {
            cout << "Insufficient access level to assign to this warzone.\n";
        }
    } else {
        cout << "Soldier or warzone not found.\n";
    }
}


void MilitaryManagementSystem::displaySoldierInfo() {
    if (currentUser) {
        currentUser->displayInfo();

        // Show assigned weapons
        const auto& weapons = currentUser->getWeapons();
        if (!weapons.empty()) {
            cout << "Assigned Weapons:\n";
            for (const auto& weapon : weapons) {
                cout << "- " << weapon << "\n";
            }
        } else {
            cout << "No weapons assigned.\n";
        }

        // Show accessible warzones
        cout << "Accessible Warzones:\n";
        bool hasAccess = false;
        for (const auto& [id, warzone] : warzones) {
            if (warzone->canAccess(currentUser)) {
                cout << "- " << warzone->toString() << "\n";
                hasAccess = true;
            }
        }
        if (!hasAccess) {
            cout << "No accessible warzones.\n";
        }
    } else {
        cout << "No soldier logged in.\n";
    }
}


void MilitaryManagementSystem::run() {
    string command;
    while (true) {
        if (currentUser) {
            cout << "\n[Logged in as: " << currentUser->getId() << "]\n";
        } else {
            cout << "\n[No soldier logged in]\n";
        }

        cout << "Enter command (type 'help' for available commands): ";
        cin >> command;

        if (command == "help") {
            showHelp();
        } else if (command == "login") {
            if (currentUser) {
                cout << "A soldier is already logged in. Please logout first.\n";
            } else {
                string soldierId;
                cout << "Enter Soldier ID to login: ";
                cin >> soldierId;
                if (login(soldierId)) {
                    cout << "Logged in successfully.\n";
                } else {
                    cout << "Soldier not found.\n";
                }
            }
        } else if (command == "logout") {
            if (currentUser) {
                logout();
            } else {
                cout << "No soldier is currently logged in.\n";
            }
        } else if (command == "create_soldier") {
            createSoldier();
        } else if (command == "add_weapon") {
            addWeaponManually();
        } else if (command == "add_warzone") {
            addWarzoneManually();
        } else if (command == "assign_weapon") {
            assignWeaponToSoldier();
        } else if (command == "assign_warzone") {
            assignWarzoneToSoldier();
        } else if (command == "display_soldier") {
            displaySoldierInfo();
        } else if (command == "view_inventory") {
            if (currentUser) {
                if (inventory.canAccess(currentUser)) {
                    inventory.displayInfo();
                } else {
                    cout << "Access denied to inventory.\n";
                }
            } else {
                cout << "No soldier logged in.\n";
            }
        } else if (command == "exit") {
            cout << "Exiting system...\n";
            break;
        } else {
            cout << "Unknown command.\n";
        }
    }
}


int main() {
    MilitaryManagementSystem system;
    system.run();
    return 0;
}

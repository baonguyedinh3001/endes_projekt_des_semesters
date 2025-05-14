// Military Management System - Extended Version
// Author: Based on user's spec
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <memory>
#include <algorithm>
using namespace std;

// ------------------- Enumerations -------------------
enum class AccessLevel { CONFIDENTIAL = 1, SECRET, TOP_SECRET, SCI };

// ------------------- Abstract Base Class -------------------
class Person {
protected:
    string firstName, lastName;
public:
    Person(string fn = "John", string ln = "Doe") : firstName(fn), lastName(ln) {}
    virtual void display() const = 0;
    virtual ~Person() {}
};

// ------------------- MilitaryRank -------------------
class MilitaryRank {
private:
    string name;
    int rankLevel;
    AccessLevel accessLevel;
public:
    MilitaryRank(string n = "Private", int rl = 1, AccessLevel al = AccessLevel::CONFIDENTIAL)
        : name(n), rankLevel(rl), accessLevel(al) {}

    bool outranks(const MilitaryRank& other) const {
        return rankLevel > other.rankLevel;
    }

    AccessLevel getAccessLevel() const { return accessLevel; }

    friend ostream& operator<<(ostream& os, const MilitaryRank& rank) {
        os << rank.name << " (Level " << rank.rankLevel << ")";
        return os;
    }
};

// ------------------- Weapon -------------------
class Weapon {
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

    friend ostream& operator<<(ostream& os, const Weapon& w) {
        os << w.name << " [" << w.type << ", Damage: " << w.damageRating
           << ", Range: " << w.range << ", Accuracy: " << w.accuracy << "]";
        return os;
    }
};

// ------------------- Soldier -------------------
class Soldier : public Person {
protected:
    string id;
    MilitaryRank rank;
    string specialization;
    int experienceYears;
    bool active;
    vector<string> skills;
    vector<Weapon> assignedWeapons;

public:
    Soldier(string i, string fn, string ln, MilitaryRank r, string spec = "Infantry", int exp = 0)
        : Person(fn, ln), id(i), rank(r), specialization(spec), experienceYears(exp), active(true) {}

    void addSkill(const string& skill) { skills.push_back(skill); }

    void assignWeapon(const Weapon& weapon) {
        if ((int)rank.getAccessLevel() >= (int)weapon.getRequiredAccess())
            assignedWeapons.push_back(weapon);
        else
            cout << "Access Denied: Weapon " << weapon.getName() << endl;
    }

    void removeWeapon(const string& weaponName) {
        assignedWeapons.erase(remove_if(assignedWeapons.begin(), assignedWeapons.end(),
            [&weaponName](Weapon& w) { return w.getName() == weaponName; }), assignedWeapons.end());
    }

    void display() const override {
        cout << "Soldier: " << firstName << " " << lastName << " | ID: " << id
             << " | Rank: " << rank << endl;
        for (const auto& w : assignedWeapons) {
            cout << " - Weapon: " << w << endl;
        }
    }

    AccessLevel getAccessLevel() const { return rank.getAccessLevel(); }
    string getId() const { return id; }

    Soldier operator+(const Soldier& other) const {
        Soldier merged = *this;
        merged.skills.insert(merged.skills.end(), other.skills.begin(), other.skills.end());
        return merged;
    }
};

// ------------------- Unit -------------------
class Unit {
private:
    string id, name;
    Soldier* commander = nullptr;
    vector<Soldier*> members;
    AccessLevel clearance;
public:
    Unit(string i, string n, AccessLevel cl = AccessLevel::CONFIDENTIAL)
        : id(i), name(n), clearance(cl) {}

    void setCommander(Soldier* s) { commander = s; }
    void addMember(Soldier* s) { members.push_back(s); }
    void removeMember(const string& soldierId) {
        members.erase(remove_if(members.begin(), members.end(),
            [&soldierId](Soldier* s) { return s->getId() == soldierId; }), members.end());
    }

    void display() const {
        cout << "Unit: " << name << " [ID: " << id << "]" << endl;
        if (commander) {
            cout << "Commander: ";
            commander->display();
        }
        for (auto* m : members) {
            cout << "Member: ";
            m->display();
        }
    }
};

// ------------------- Inventory -------------------
class Inventory {
private:
    map<string, pair<Weapon, int>> weapons;
    map<string, pair<string, int>> supplies;
    AccessLevel requiredAccessLevel;
public:
    Inventory(AccessLevel al = AccessLevel::CONFIDENTIAL) : requiredAccessLevel(al) {}

    void addWeapon(const Weapon& weapon, int qty) {
        weapons[weapon.getName()].first = weapon;
        weapons[weapon.getName()].second += qty;
    }

    void removeWeapon(const string& name, int qty) {
        if (weapons.count(name)) {
            weapons[name].second = max(0, weapons[name].second - qty);
            if (weapons[name].second == 0)
                weapons.erase(name);
        }
    }

    void addSupply(const string& id, const string& desc, int qty) {
        supplies[id].first = desc;
        supplies[id].second += qty;
    }

    void removeSupply(const string& id, int qty) {
        if (supplies.count(id)) {
            supplies[id].second = max(0, supplies[id].second - qty);
            if (supplies[id].second == 0)
                supplies.erase(id);
        }
    }

    int getWeaponQuantity(const string& name) const {
        return weapons.count(name) ? weapons.at(name).second : 0;
    }

    int getSupplyQuantity(const string& id) const {
        return supplies.count(id) ? supplies.at(id).second : 0;
    }

    bool canAccess(const Soldier* soldier) const {
        return static_cast<int>(soldier->getAccessLevel()) >= static_cast<int>(requiredAccessLevel);
    }

    void display() const {
        cout << "Inventory (Access Level: " << static_cast<int>(requiredAccessLevel) << ")\n";
        for (auto& [name, pair] : weapons) {
            cout << name << ": Qty: " << pair.second << ", Details: " << pair.first << endl;
        }
        for (auto& [id, pair] : supplies) {
            cout << id << ": " << pair.first << " | Qty: " << pair.second << endl;
        }
    }
};

// ------------------- Main -------------------
int main() {
    // Create Ranks
    MilitaryRank rank1("Sergeant", 5, AccessLevel::SECRET);
    MilitaryRank rank2("Lieutenant", 12, AccessLevel::TOP_SECRET);

    // Create Soldiers
    Soldier* alice = new Soldier("S001", "Alice", "Smith", rank1);
    Soldier* bob = new Soldier("S002", "Bob", "Jones", rank2);

    alice->addSkill("Sniper");
    bob->addSkill("Tactics");

    // Create Weapons
    Weapon rifle("Rifle", "Assault", 60, 300, 75, AccessLevel::SECRET);
    Weapon drone("Drone", "Recon", 20, 1000, 90, AccessLevel::TOP_SECRET);

    // Assign Weapons
    alice->assignWeapon(rifle);  // Allowed
    alice->assignWeapon(drone);  // Denied
    bob->assignWeapon(drone);    // Allowed

    // Inventory
    Inventory inv(AccessLevel::SECRET);
    inv.addWeapon(rifle, 10);
    inv.addSupply("MED01", "Medkit", 20);

    // Unit
    Unit unit1("U001", "Alpha");
    unit1.setCommander(bob);
    unit1.addMember(alice);
    unit1.addMember(bob);

    // Display
    alice->display();
    bob->display();
    unit1.display();
    inv.display();

    // Access check
    cout << "\nInventory access check:\n";
    cout << "Alice can access? " << (inv.canAccess(alice) ? "Yes" : "No") << endl;
    cout << "Bob can access? " << (inv.canAccess(bob) ? "Yes" : "No") << endl;

    delete alice;
    delete bob;

    return 0;
}

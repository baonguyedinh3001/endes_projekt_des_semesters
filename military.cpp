#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <memory>

using namespace std;

// Enumeration for Military Branches
enum class MilitaryBranch { ARMY, NAVY, AIR_FORCE, MARINES, COAST_GUARD, SPACE_FORCE };

// Enumeration for Access Levels
enum class AccessLevel { CONFIDENTIAL = 1, SECRET = 2, TOP_SECRET = 3, SCI = 4 };

// Abstract Base Class: MilitaryEntity
class MilitaryEntity {
protected:
    string id;
public:
    MilitaryEntity(string id) : id(id) {}  // Constructor
    virtual ~MilitaryEntity() {}  // Virtual destructor

    virtual void display() const = 0;  // Pure virtual function (Polymorphism)
};

// Concrete Class: MilitaryRank (Inherits MilitaryEntity)
class MilitaryRank : public MilitaryEntity {
private:
    string name;
    int rankLevel;
    AccessLevel accessLevel;
    MilitaryBranch branch;
public:
    MilitaryRank(string id, string name, int rankLevel, AccessLevel accessLevel, MilitaryBranch branch)
        : MilitaryEntity(id), name(name), rankLevel(rankLevel), accessLevel(accessLevel), branch(branch) {}

    void display() const override {
        cout << "Rank: " << name << ", Level: " << rankLevel << ", Access: " << static_cast<int>(accessLevel) << endl;
    }

    bool outranks(const MilitaryRank& other) const {
        return rankLevel > other.rankLevel;
    }

    friend ostream& operator<<(ostream& os, const MilitaryRank& rank) {  // Operator Overloading (<<)
        os << rank.name << " (" << rank.rankLevel << ")";
        return os;
    }
};

// Subclass: Soldier (Inherits MilitaryRank)
class Soldier : public MilitaryRank {
private:
    string firstName, lastName;
    vector<string> skills;
public:
    Soldier(string id, string name, int rankLevel, AccessLevel accessLevel, MilitaryBranch branch,
            string firstName, string lastName)
        : MilitaryRank(id, name, rankLevel, accessLevel, branch), firstName(firstName), lastName(lastName) {}

    void addSkill(const string& skill) { skills.push_back(skill); }

    void display() const override {
        cout << "Soldier: " << firstName << " " << lastName << " (" << *this << ")\nSkills: ";
        for (const auto& skill : skills) cout << skill << " ";
        cout << endl;
    }
};

// Class: Weapon
class Weapon {
private:
    string name;
    string type;
    int damageRating;
public:
    Weapon(string name, string type, int damageRating)
        : name(name), type(type), damageRating(damageRating) {}

    void display() const {
        cout << "Weapon: " << name << ", Type: " << type << ", Damage: " << damageRating << endl;
    }

    friend Weapon operator+(const Weapon& w1, const Weapon& w2) {  // Operator Overloading (+)
        return Weapon(w1.name + "-" + w2.name, w1.type, w1.damageRating + w2.damageRating);
    }
};

// Class: Unit (Using Dynamic Memory Allocation)
class Unit {
private:
    string unitName;
    unique_ptr<Soldier> commander;  // Using smart pointer
    set<Soldier*> members;
public:
    Unit(string name) : unitName(name) {}

    void setCommander(Soldier* soldier) { commander.reset(soldier); }  // Assign commander

    void addMember(Soldier* soldier) { members.insert(soldier); }

    void display() const {
        cout << "Unit: " << unitName << "\nCommander: ";
        if (commander) commander->display();
        cout << "Members:\n";
        for (const auto& member : members) member->display();
    }
};

// Main function to demonstrate the system
int main() {
    // Creating Objects
    Soldier* soldier1 = new Soldier("S001", "Sergeant", 10, AccessLevel::SECRET, MilitaryBranch::ARMY, "John", "Doe");
    Soldier* soldier2 = new Soldier("S002", "Captain", 15, AccessLevel::TOP_SECRET, MilitaryBranch::NAVY, "Jane", "Smith");

    Weapon weapon1("Rifle", "Firearm", 80);
    Weapon weapon2("Sniper", "Firearm", 100);

    Unit unit("Alpha Squad");
    unit.setCommander(soldier2);
    unit.addMember(soldier1);
    unit.addMember(soldier2);

    // Displaying objects
    unit.display();
    weapon1.display();
    weapon2.display();

    // Demonstrating operator overloading
    Weapon combinedWeapon = weapon1 + weapon2;
    combinedWeapon.display();

    // Cleaning up dynamic memory allocation
    delete soldier1;
    delete soldier2;

    return 0;
}
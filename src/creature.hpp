#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <string>
#include <cstdlib>
#include "JsonBox.h"

#include "entity.hpp"
#include "inventory.hpp"

class Area;
class EntityManager;
class Weapon;
class Armor;
class Door;

class Creature : public Entity
{
	public:

	// Name of the creature
	std::string name;

	// Creature stats
	int hp;
	int maxHp;
	int strength;
	int agility;
	double evasion;
	unsigned int xp;

	// Items that the creature possesses
	Inventory inventory;

	// Currently equipped weapon. Used as a pointer to an atlas entry,
	// but not necessary. nullptr denotes that no weapon is equipped
	Weapon* equippedWeapon;

	// Currently equipped armor
	Armor* equippedArmor;

	// Area the creature resides in. Used for player motion but also could
	// be used for enemy AI
	std::string currentArea;

	// Constructors
	Creature(std::string id, std::string name, int hp, int strength, int agility, double evasion,
		unsigned int xp);
	Creature(std::string id, JsonBox::Value& v, EntityManager* mgr);
	Creature();

	// Equip a weapon by setting the equipped weapon pointer. Currently
	// a pointless function (simple enough to be rewritten each time)
	// but handy if dual wielding is ever added, or shields etc
	void equipWeapon(Weapon* weapon);
	
	// Equip the armor into it's correct slot. A slightly more useful
	// function!
	void equipArmor(Armor* armor);

	// Convert internal area id into a pointer
	Area* getAreaPtr(EntityManager* mgr);

	// Attack the target creature, reducing their health if necessary
	int attack(Creature* target);

	// Go through a door
	// 0 = Door is locked
	// 1 = Door unlocked using key
	// 2 = Door is open
	int traverse(Door* door);

	// Create a JSON object containing the creature data
	virtual JsonBox::Object toJson();

	// Attempt to load all data from the JSON value
	virtual void load(std::string id, JsonBox::Value& v, EntityManager* mgr);
};

#endif /* CREATURE_HPP */

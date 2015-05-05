#ifndef CREATURE_HPP
#define CREATURE_HPP

#include "entity_manager.hpp"
#include "entity.hpp"
#include "inventory.hpp"
#include "weapon.hpp"
#include "armour.hpp"
#include "door.hpp"

#include <string>
#include <fstream>
#include "JsonBox.h"

class Area;

class Creature : public Entity
{
	public:

	// Name of the creature
	std::string name;

	// Creature stats. Reasonable values are in parentheses
	int health;		// Current hit points (10-1000+)
	int maxHealth;	// Maximum hit points (10-1000+)
	int str;		// Strength. Determines damage in battle (1-100)
	int end;		// Endurance. Determines maximum health (1-100)
	int dex;		// Dexterity. Determines speed in battle (1-100)
	double hitRate;	// Modifier to hit chance. (1-150)

	// Current level of the creature. Determines the amount of experience
	// that it gives to the victor when defeated (see Battle class for more)
	// and the amount of experience required to level up again. Upon
	// levelling up the creature will gain stat improvements.
	// 1-50 is reasonable
	unsigned int level;

	// Items that the creature possesses
	Inventory inventory;

	// Currently equipped weapon. Used as a pointer to an atlas entry,
	// but not necessary. nullptr denotes that no weapon is equipped
	Weapon* equippedWeapon;

	// Armour currently equipped into each slot
	Armour* equippedArmour[Armour::Slot::N];

	// Area the creature resides in. Used for player motion but also could
	// be used for enemy AI
	std::string currentArea;

	Creature(std::string id, std::string name, int health, int str, int end, int dex, double hitRate,
		unsigned int level) : Entity(id)
	{
		this->name = name;
		this->health = health;
		this->maxHealth = health;
		this->str = str;
		this->end = end;
		this->dex = dex;
		this->hitRate = hitRate;
		this->equippedArmour[Armour::Slot::HEAD] = nullptr;
		this->equippedArmour[Armour::Slot::TORSO] = nullptr;
		this->equippedArmour[Armour::Slot::LEGS] = nullptr;
		this->equippedWeapon = nullptr;
		this->level = level;
	}

	Creature() : Creature("nullid", "", 0, 0, 0, 0, 0, 0)
	{
	}

	Creature(std::string id, JsonBox::Value v, EntityManager* mgr) : Creature()
	{
		this->load(id, v, mgr);
	}

	// Equip a weapon by setting the equipped weapon pointer. Currently
	// a pointless function (simple enough to be rewritten each time)
	// but handy if dual wielding is ever added, or shields etc
	void equipWeapon(Weapon* weapon)
	{
		this->equippedWeapon = weapon;

		return;
	}

	// Equip the armour into it's correct slot. A slightly more useful
	// function!
	void equipArmour(Armour* armour)
	{
		this->equippedArmour[(int)armour->slot] = armour;

		return;
	}

	Area* getAreaPtr(EntityManager* mgr)
	{
		return mgr->getEntity<Area>(this->currentArea);
	}

	// Go through a door
	// 0 = Door is locked
	// 1 = Door unlocked using key
	// 2 = Door is open
	int traverse(Door* door)
	{
		int flag = 2;
		// Open the door if it is shut
		if(door->locked == 0)
		{
			door->locked = -1;
			flag = 2;
		}
		else if(door->locked > 0)
		{
			// Unlock and open the door if the creature has the key
			if(this->inventory.hasItem(door->key))
			{
				door->locked = -1;
				flag = 1;
			}
			// Creature does not have key so door remains locked
			else
			{
				return 0;
			}
		}
		if(door->areas.first == this->currentArea)
		{
			this->currentArea = door->areas.second;
		}
		else if(door->areas.second == this->currentArea)
		{
			this->currentArea = door->areas.first;
		}

		return flag;
	}

	// Create a JSON object containing the creature data
	virtual JsonBox::Object toJson()
	{
		JsonBox::Object o;
		o["name"] = JsonBox::Value(this->name);
		o["health"] = JsonBox::Value(this->health);
		o["maxHealth"] = JsonBox::Value(this->maxHealth);
		o["str"] = JsonBox::Value(this->str);
		o["end"] = JsonBox::Value(this->end);
		o["dex"] = JsonBox::Value(this->dex);
		o["hitRate"] = JsonBox::Value(this->hitRate);
		o["level"] = JsonBox::Value(int(this->level));
		o["inventory"] = JsonBox::Value(this->inventory.getJson());
		o["equipped_weapon"] = JsonBox::Value(this->equippedWeapon == nullptr ? "nullptr" : this->equippedWeapon->id);
		JsonBox::Array a;
		for(auto armour : this->equippedArmour)
		{
			a.push_back(armour == nullptr ? "nullptr" : armour->id);
		}
		o["equipped_armour"] = a;

		return o;
	}

	// Load the creature's variables from the JSON value, excluding those
	// requiring an EntityManager (Items etc.)
	void load(std::string id, JsonBox::Value v)
	{
		JsonBox::Object o = v.getObject();
		this->name = o["name"].getString();
		this->health = o["health"].getInteger();
		if(o.find("maxHealth") != o.end())
		{
			this->maxHealth = o["maxHealth"].getInteger();
		}
		else
		{
			this->maxHealth = health;
		}
		this->str = o["str"].getInteger();
		this->end = o["end"].getInteger();
		this->dex = o["dex"].getInteger();
		this->hitRate = o["hitRate"].getDouble();
		this->level = o["level"].getInteger();

		Entity::load(id, v);

		return;
	}

	// Attempt to load all data from the JSON value
	virtual void load(std::string id, JsonBox::Value v, EntityManager* mgr)
	{
		// Load variables
		this->load(id, v);

		// Load entity variables
		JsonBox::Object o = v.getObject();

		if(o.find("inventory") != o.end())
		{
			this->inventory = Inventory(o["inventory"], mgr);
		}
		if(o.find("equipped_weapon") != o.end())
		{
			std::string equippedWeaponName = o["equipped_weapon"].getString();
			this->equippedWeapon = equippedWeaponName == "nullptr" ? nullptr : mgr->getEntity<Weapon>(equippedWeaponName);
		}
		if(o.find("equipped_armour") != o.end())
		{
			JsonBox::Array a = o["equipped_amour"].getArray();
			for(int i = 0; i < a.size(); ++i)
			{
				std::string equippedArmourName = a[i].getString();
				this->equippedArmour[i] = equippedArmourName == "nullptr" ? nullptr : mgr->getEntity<Armour>(equippedArmourName);
			}
		}

		return;
	}
};

#endif /* CREATURE_HPP */

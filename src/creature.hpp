#ifndef CREATURE_HPP
#define CREATURE_HPP

#include "entity_manager.hpp"
#include "entity.hpp"
#include "inventory.hpp"
#include "weapon.hpp"
#include "armor.hpp"
#include "door.hpp"

#include <string>
#include <fstream>
#include <cstdlib>
#include "JsonBox.h"

class Area;

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

	Creature(std::string id, std::string name, int hp, int strength, int agility, double evasion,
		unsigned int xp) : Entity(id)
	{
		this->name = name;
		this->hp = hp;
		this->maxHp = hp;
		this->strength = strength;
		this->agility = agility;
		this->evasion = evasion;
		this->equippedArmor = nullptr;
		this->equippedWeapon = nullptr;
		this->xp = xp;
	}

	Creature() : Creature("nullid", "", 0, 0, 0, 0.0, 1)
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

	// Equip the armor into it's correct slot. A slightly more useful
	// function!
	void equipArmor(Armor* armor)
	{
		this->equippedArmor = armor;

		return;
	}

	Area* getAreaPtr(EntityManager* mgr)
	{
		return mgr->getEntity<Area>(this->currentArea);
	}

	int attack(Creature* target)
	{
		// Damage done
		int damage = 0;

		if(double(std::rand()) / RAND_MAX  > target->evasion)
		{
			// Calculate attack based on strength and weapon damage
			int attack = this->strength + (this->equippedWeapon == nullptr ? 0 : this->equippedWeapon->damage);
			// Calculate defense based on agility and armor defense
			int defense = target->agility + (target->equippedArmor == nullptr ? 0 : target->equippedArmor->defense);
			// 1/32 chance of a critical hit
			if(std::rand() % 32 == 0)
			{
				// Ignore defense and do damage in range [attack/2, attack]
				damage = attack / 2 + std::rand() % (attack / 2);
			}
			else
			{
				// Normal hit so factor in defense
				int baseDamage = (attack - defense) / 2;
				// Do damage in range [baseDamage/4, baseDamage/2]
				damage = baseDamage / 4 + std::rand() % (baseDamage / 4);
				// If the damage is zero then have a 50% chance to do 1 damage
				if(damage < 1)
				{
					damage = std::rand() % 2;
				}
			}
			// Damage the target
			target->hp -= damage;
		}

		return damage;
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
		o["hp"] = JsonBox::Value(this->hp);
		o["hp_max"] = JsonBox::Value(this->maxHp);
		o["strength"] = JsonBox::Value(this->strength);
		o["agility"] = JsonBox::Value(this->agility);
		o["evasion"] = JsonBox::Value(this->evasion);
		o["xp"] = JsonBox::Value(int(this->xp));
		o["inventory"] = JsonBox::Value(this->inventory.getJson());
		o["equipped_weapon"] = JsonBox::Value(this->equippedWeapon == nullptr ? "nullptr" : this->equippedWeapon->id);
		o["equipped_armor"] = JsonBox::Value(this->equippedArmor == nullptr ? "nullptr" : this->equippedArmor->id);

		return o;
	}

	// Load the creature's variables from the JSON value, excluding those
	// requiring an EntityManager (Items etc.)
	void load(std::string id, JsonBox::Value v)
	{
		JsonBox::Object o = v.getObject();
		this->name = o["name"].getString();
		this->hp = o["hp"].getInteger();
		if(o.find("hp_max") != o.end())
		{
			this->maxHp = o["hp_max"].getInteger();
		}
		else
		{
			this->maxHp = hp;
		}
		this->strength = o["strength"].getInteger();
		this->agility = o["agility"].getInteger();
		this->evasion = o["evasion"].getDouble();

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
		if(o.find("equipped_armor") != o.end())
		{
			std::string equippedArmorName = o["equipped_armor"].getString();
			this->equippedArmor = equippedArmorName == "nullptr" ? nullptr : mgr->getEntity<Armor>(equippedArmorName);
		}

		return;
	}
};

#endif /* CREATURE_HPP */

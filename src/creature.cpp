#include <string>
#include <cstdlib>
#include "JsonBox.h"

#include "creature.hpp"
#include "entity.hpp"
#include "inventory.hpp"
#include "weapon.hpp"
#include "armor.hpp"
#include "door.hpp"
#include "area.hpp"
#include "entity_manager.hpp"

Creature::Creature(std::string id, std::string name, int hp, int strength, int agility, double evasion,
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

Creature::Creature(std::string id, JsonBox::Value& v, EntityManager* mgr) : Entity(id)
{
	this->load(id, v, mgr);
}

void Creature::equipWeapon(Weapon* weapon)
{
	this->equippedWeapon = weapon;

	return;
}

void Creature::equipArmor(Armor* armor)
{
	this->equippedArmor = armor;

	return;
}

Area* Creature::getAreaPtr(EntityManager* mgr)
{
	return mgr->getEntity<Area>(this->currentArea);
}

int Creature::attack(Creature* target)
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
			damage = attack / 2 + std::rand() % (attack / 2 + 1);
		}
		else
		{
			// Normal hit so factor in defense
			int baseDamage = attack - defense / 2;
			// Do damage in range [baseDamage/4, baseDamage/2]
			damage = baseDamage / 4 + std::rand() % (baseDamage / 4 + 1);
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


int Creature::traverse(Door* door)
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
		if(this->inventory.count(door->key))
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

JsonBox::Object Creature::toJson()
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

void Creature::load(std::string id, JsonBox::Value& v, EntityManager* mgr)
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

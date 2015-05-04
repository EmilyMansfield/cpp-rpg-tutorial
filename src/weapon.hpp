#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "entity_manager.hpp"
#include "item.hpp"

#include <string>
#include "JsonBox.h"

// Weapons are items, so they should inherit their properties
class Weapon : public Item
{
	public:

	// Weapon damage. See the Battle class for formula, but values
	// between 1-50 are reasonable
	unsigned damage;

	// Modifier to hit chance. Small values are encouraged, e.g.
	// 5-30%
	double hitRate;

	// Pass inherited qualities to the normal item constructor
	Weapon(std::string id, std::string name, std::string description, int damage, double hitRate) :
		Item(id, name, description)
	{
		this->damage = damage;
		this->hitRate = hitRate;
	}

	Weapon() : Item()
	{
	}

	Weapon(std::string id, JsonBox::Value v, EntityManager* mgr) : Item()
	{
		this->load(id, v);
	}

	void load(std::string id, JsonBox::Value v)
	{
		Item::load(id, v);
		JsonBox::Object o = v.getObject();
		this->damage = o["damage"].getInteger();
		this->hitRate = o["hitRate"].getDouble();

		return;
	}
};

#endif /* WEAPON_HPP */

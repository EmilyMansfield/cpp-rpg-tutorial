#ifndef ARMOUR_HPP
#define ARMOUR_HPP

#include "entity_manager.hpp"
#include "item.hpp"

#include <string>

// Armour should also inherit item properties
class Armour : public Item
{
	public:

	// Armour can go into one of three slots, and only one piece of
	// armour may occupy each slot at a time. The N is there to quickly
	// retrieve the number of slots, without having to use another variable
	enum Slot { TORSO, HEAD, LEGS, N };

	Slot slot;

	// See Battle class for exact formula, values from 1-50 are reasonable
	int defense;

	// Usual constructor
	Armour(std::string id, std::string name, std::string description, int defense, Armour::Slot slot) :
		Item(id, name, description)
	{
		this->defense = defense;
		this->slot = slot;
	}

	Armour() : Armour("nullid", "", "", 0, Slot::TORSO)
	{
	}

	Armour(std::string id, JsonBox::Value v, EntityManager* mgr) : Item()
	{
		this->load(id, v);
	}

	void load(std::string id, JsonBox::Value v)
	{
		Item::load(id, v);
		JsonBox::Object o = v.getObject();
		this->defense = o["defense"].getInteger();
		this->slot = Slot(o["slot"].getInteger());

		return;
	}
};

#endif /* ARMOUR_HPP */

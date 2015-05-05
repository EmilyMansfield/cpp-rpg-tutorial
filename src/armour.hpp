#ifndef ARMOUR_HPP
#define ARMOUR_HPP

#include "entity_manager.hpp"
#include "item.hpp"

#include <string>

// Armour should also inherit item properties
class Armour : public Item
{
	public:

	int defense;

	// Usual constructor
	Armour(std::string id, std::string name, std::string description, int defense) :
		Item(id, name, description)
	{
		this->defense = defense;
	}

	Armour() : Armour("nullid", "", "", 0)
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

		return;
	}
};

#endif /* ARMOUR_HPP */

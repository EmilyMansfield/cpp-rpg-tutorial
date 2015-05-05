#ifndef ARMOR_HPP
#define ARMOR_HPP

#include "entity_manager.hpp"
#include "item.hpp"

#include <string>

// Armor should also inherit item properties
class Armor : public Item
{
	public:

	int defense;

	// Usual constructor
	Armor(std::string id, std::string name, std::string description, int defense) :
		Item(id, name, description)
	{
		this->defense = defense;
	}

	Armor() : Armor("nullid", "", "", 0)
	{
	}

	Armor(std::string id, JsonBox::Value v, EntityManager* mgr) : Item()
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

#endif /* ARMOR_HPP */

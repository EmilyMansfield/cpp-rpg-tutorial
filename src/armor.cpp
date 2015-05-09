#include <string>
#include "JsonBox.h"

#include "armor.hpp"
#include "item.hpp"
#include "entity_manager.hpp"

Armor::Armor(std::string id, std::string name, std::string description, int defense) :
	Item(id, name, description)
{
	this->defense = defense;
}

Armor::Armor() : Armor::Armor("nullid", "", "", 0)
{
}

Armor::Armor(std::string id, JsonBox::Value& v, EntityManager* mgr) : Item()
{
	this->load(id, v);
}

void Armor::load(std::string id, JsonBox::Value& v)
{
	Item::load(id, v);
	JsonBox::Object o = v.getObject();
	this->defense = o["defense"].getInteger();

	return;
}

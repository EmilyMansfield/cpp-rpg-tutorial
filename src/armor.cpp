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

Armor::Armor(std::string id, JsonBox::Value& v, EntityManager* mgr) : Item(id, v, mgr)
{
	this->load(id, v, mgr);
}

void Armor::load(std::string id, JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	this->defense = o["defense"].getInteger();

	return;
}

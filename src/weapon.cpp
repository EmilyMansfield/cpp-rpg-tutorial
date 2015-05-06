#include <string>
#include "JsonBox.h"

#include "weapon.hpp"
#include "entity_manager.hpp"
#include "item.hpp"

Weapon::Weapon(std::string id, std::string name, std::string description, int damage) :
	Item(id, name, description)
{
	this->damage = damage;
}

Weapon::Weapon() : Weapon::Weapon("nullid", "", "", 0)
{
}

Weapon::Weapon(std::string id, JsonBox::Value v, EntityManager* mgr) : Item()
{
	this->load(id, v);
}

void Weapon::load(std::string id, JsonBox::Value v)
{
	Item::load(id, v);
	JsonBox::Object o = v.getObject();
	this->damage = o["damage"].getInteger();

	return;
}

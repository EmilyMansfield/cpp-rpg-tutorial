#include <string>
#include "JsonBox.h"

#include "item.hpp"
#include "entity_manager.hpp"
#include "entity.hpp"

Item::Item(std::string id, std::string name, std::string description) : Entity(id)
{
	this->name = name;
	this->description = description;
}

Item::Item() : Entity("nullid")
{
}

Item::Item(std::string id, JsonBox::Value v, EntityManager* mgr) : Item::Item()
{
	this->load(id, v);
}

void Item::load(std::string id, JsonBox::Value v)
{
	JsonBox::Object o = v.getObject();
	this->name = o["name"].getString();
	this->description = o["description"].getString();

	Entity::load(id, v);

	return;
}
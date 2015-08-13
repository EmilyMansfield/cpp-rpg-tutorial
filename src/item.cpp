#include <string>
#include "JsonBox.h"

#include "item.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"

Item::Item(std::string id, std::string name, std::string description) : Entity(id)
{
	this->name = name;
	this->description = description;
}

Item::Item() : Entity("nullid")
{
}

Item::Item(std::string id, JsonBox::Value& v, EntityManager* mgr) : Item::Item()
{
	this->load(id, v, mgr);
}

void Item::load(std::string id, JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	this->id = id;
	this->name = o["name"].getString();
	this->description = o["description"].getString();

	return;
}

#include <string>
#include <JsonBox.h>

#include "item.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"

Item::Item(std::string id, std::string name, std::string description) : Entity(id)
{
	this->name = name;
	this->description = description;
}

Item::Item(std::string id, JsonBox::Value& v, EntityManager* mgr) : Entity(id)
{
	this->load(v, mgr);
}

void Item::load(JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	this->name = o["name"].getString();
	this->description = o["description"].getString();

	return;
}

#include <string>
#include <utility>

#include "door.hpp"
#include "item.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"

Door::Door(std::string id, std::string description, std::pair<std::string, std::string> areas,
	int locked, Item* key) : Entity(id)
{
	this->description = description;
	this->areas = areas;
	this->locked = locked;
	this->key = key;
}

Door::Door(std::string id, JsonBox::Value& v, EntityManager* mgr) : Entity(id)
{
	this->load(v, mgr);
}

void Door::load(JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	this->description = o["description"].getString();
	this->locked = o["locked"].getInteger();
	if(o.find("key") != o.end())
	{
		this->key = mgr->getEntity<Item>(o["key"].getString());
	}
	JsonBox::Array a = o["areas"].getArray();
	if(a.size() == 2)
	{
		this->areas.first = a[0].getString();
		this->areas.second = a[1].getString();
	}

	return;
}

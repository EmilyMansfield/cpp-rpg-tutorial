#ifndef DOOR_HPP
#define DOOR_HPP

#include "entity_manager.hpp"

#include "entity.hpp"

#include <string>
#include <utility>

class Area;
class Item;

class Door : Entity
{
	public:

	// Door description e.g. large wooden door, rusted iron gate
	std::string description;

	// < 0 is open
	// 0 is unlocked but closed
	// > 0 is locked and needs key to open
	int locked;

	// If the player has the required key then they can unlock the door.
	Item* key;

	std::pair<std::string, std::string> areas;

	Door(std::string id, std::string description, std::pair<std::string, std::string> areas,
		int locked, Item* key = nullptr) : Entity(id)
	{
		this->description = description;
		this->areas = areas;
		this->locked = locked;
		this->key = key;
	}

	Door() : Door("nullid", "", std::make_pair("", ""), 0)
	{
	}

	Door(std::string id, JsonBox::Value v, EntityManager* mgr) : Door()
	{
		this->load(id, v, mgr);
	}

	void load(std::string id, JsonBox::Value v, EntityManager* mgr)
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

		Entity::load(id, v);

		return;
	}
};

#endif /* DOOR_HPP */
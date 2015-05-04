#ifndef ITEM_HPP
#define ITEM_HPP

#include "entity_manager.hpp"
#include "entity.hpp"

#include <string>
#include "JsonBox.h"

class Item : public Entity
{
	public:

	// Name and description of the item
	std::string name;
	std::string description;

	// Standard constructors, nothing special
	Item(std::string id, std::string name, std::string description) : Entity(id)
	{
		this->name = name;
		this->description = description;
	}

	Item() : Entity("nullid")
	{
	}

	Item(std::string id, JsonBox::Value v, EntityManager* mgr) : Item()
	{
		this->load(id, v);
	}

	// Load the item information from the JSON value
	virtual void load(std::string id, JsonBox::Value v)
	{
		JsonBox::Object o = v.getObject();
		this->name = o["name"].getString();
		this->description = o["description"].getString();

		Entity::load(id, v);

		return;
	}
};

#endif /* ITEM_HPP */

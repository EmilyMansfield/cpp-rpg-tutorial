#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>
#include "JsonBox.h"

#include "entity.hpp"

class EntityManager;

class Item : public Entity
{
	public:

	// Name and description of the item
	std::string name;
	std::string description;

	// Constructors
	Item(std::string id, std::string name, std::string description);
	Item(std::string id, JsonBox::Value& v, EntityManager* mgr);
	Item();

	// Load the item information from the JSON value
	virtual void load(std::string id, JsonBox::Value& v);
};

#endif /* ITEM_HPP */

#ifndef DOOR_HPP
#define DOOR_HPP

#include <string>
#include <utility>

#include "entity.hpp"

class Item;
class EntityManager;

class Door : public Entity
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
		int locked, Item* key = nullptr);
	Door(std::string id, JsonBox::Value& v, EntityManager* mgr);

	void load(std::string id, JsonBox::Value& v, EntityManager* mgr);
};

#endif /* DOOR_HPP */

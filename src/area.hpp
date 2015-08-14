#ifndef AREA_HPP
#define AREA_HPP

#include <vector>
#include <string>
#include "JsonBox.h"

#include "entity.hpp"
#include "inventory.hpp"
#include "creature.hpp"
#include "dialogue.hpp"

class EntityManager;
class Door;

// Movement is achieved through the use of areas, which are contained
// units of space consisting of an inventory, a list of creatures and
// a dialogue
class Area : public Entity
{
	public:

	// Dialogue is run whenever the area is entered
	Dialogue dialogue;

	// Items contained within the area. Not split into individual containers
	// for simplicity
	Inventory items;

	// Links between rooms. Every door should have this as one of its area
	// pointers
	std::vector<Door*> doors;

	// Creatures contained within the area. Not pointers because we want unique
	// instances of the creatures
	std::vector<Creature> creatures;

	// Constructors
	Area(std::string id, Dialogue dialogue, Inventory items,
		std::vector<Creature*> creatures);
	Area(std::string id, JsonBox::Value& v, EntityManager* mgr);

	// Load the area from the given Json value
	void load(std::string id, JsonBox::Value& v, EntityManager* mgr);

	// Return a Json object representing the area
	JsonBox::Object getJson();
};

#endif /* AREA_HPP */

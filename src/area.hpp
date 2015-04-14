/*
The MIT License (MIT)

Copyright (c) 2013 Daniel Mansfield

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef AREA_HPP
#define AREA_HPP

#include "entity.hpp"
#include "inventory.hpp"
#include "creature.hpp"
#include "dialogue.hpp"

#include <vector>

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

	// Creatures contained within the area. Currently this is limited
	// to just one creature due to how the battle system works, but it
	// made sense to set it up as a vector from the start to simplify
	// things later
	std::vector<Creature*> creatures;

	Area(std::string id, Dialogue dialogue, Inventory items,
		std::vector<Creature*> creatures) : Entity(id)
	{
		this->dialogue = dialogue;
		this->items = items;
		this->creatures = creatures;
	}

	Area() : Entity("nullid")
	{
	}

	Area(std::string id, JsonBox::Value v,
		std::map<std::string, Item>& itemAtlas,
		std::map<std::string, Weapon>& weaponAtlas,
		std::map<std::string, Armour>& armourAtlas,
		std::map<std::string, Creature>& creatureAtlas) : Entity(id)
	{
		this->load(id, v, itemAtlas, weaponAtlas, armourAtlas, creatureAtlas);
	}

	// Search the area for items and give them to the searcher, notifying
	// them of their rewards
	void search(Creature& player)
	{
		std::cout << "You find:" << std::endl;

		this->items.print();
		player.inventory.merge(&(this->items));
		this->items.clear();

		return;
	}

	void load(std::string id, JsonBox::Value v,
		std::map<std::string, Item>& itemAtlas,
		std::map<std::string, Weapon>& weaponAtlas,
		std::map<std::string, Armour>& armourAtlas,
		std::map<std::string, Creature>& creatureAtlas)
	{
		JsonBox::Object o = v.getObject();

		// Build the dialogue
		JsonBox::Object dialogue = o["dialogue"].getObject();
		std::string dialogue_description = dialogue["description"].getString();
		std::vector<std::string> dialogue_choices;
		for(auto choice : dialogue["choices"].getArray())
		{
			dialogue_choices.push_back(choice.getString());
		}
		this->dialogue = Dialogue(dialogue_description, dialogue_choices);

		// Build the inventory
		this->items = Inventory(o["inventory"], itemAtlas, weaponAtlas, armourAtlas);

		// Build the creature list
		for(auto creature : o["creatures"].getArray())
		{
			this->creatures.push_back(&creatureAtlas[creature.getString()]);
		}

		Entity::load(id, v);

		return;
	}
};

#endif /* AREA_HPP */

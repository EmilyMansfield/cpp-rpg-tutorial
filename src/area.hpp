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

#include "inventory.hpp"
#include "creature.hpp"
#include "dialogue.hpp"

#include <vector>

class Door;

// Movement is achieved through the use of areas, which are contained
// units of space consisting of an inventory, a list of creatures and
// a dialogue
class Area
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

	// Creatures contained within the area. Currently this is limited
	// to just one creature due to how the battle system works, but it
	// made sense to set it up as a vector from the start to simplify
	// things later
	std::vector<Creature*> creatures;

	Area(Dialogue dialogue, Inventory items,
		std::vector<Creature*> creatures)
	{
		this->dialogue = dialogue;
		this->items = items;
		this->creatures = creatures;
	}

	Area()
	{
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
};

#endif /* AREA_HPP */

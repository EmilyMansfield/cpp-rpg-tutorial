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

#include "atlas.hpp"
#include "JsonBox.h"

void buildatlas_creature(std::map<std::string, Creature>& atlas)
{
	// Fill the atlas
	JsonBox::Value v;
	v.loadFromFile("creatures.json");

	JsonBox::Object o = v.getObject();
	for(auto creature : o)
	{
		std::string key = creature.first;
		// Nicer with a constructor but that makes the Creature code ugly
		// TODO?
		atlas[key] = Creature();
		atlas[key].load(key, creature.second);
	}

	return;
}

void buildatlas_item(std::map<std::string, Item>& atlas)
{
	// Item(Name, Description)
	atlas["item_gold_coin"] = Item("item_gold_coin", "Gold Coin", "A small disc made of lustrous metal");
	atlas["item_iron_key"] = Item("item_iron_key", "Iron Key", "A heavy iron key with a simple cut");

	return;
}

void buildatlas_weapon(std::map<std::string, Weapon>& atlas)
{
	// Weapon(Name, Description, Damage, Hit Rate)
	atlas["weapon_iron_dagger"] = Weapon("weapon_iron_dagger", "Iron Dagger", "A short blade made of iron with a leather-bound hilt", 5, 10.0);
	atlas["weapon_excalibur"] = Weapon("weapon_excalibure", "Excalibur", "The legendary blade, bestowed upon you by the Lady of the Lake", 35, 35.0);

	return;
}

void buildatlas_armour(std::map<std::string, Armour>& atlas)
{
	// Armour(Name, Description, Defense, Slot)
	atlas["armour_leather_cuirass"] = Armour("armour_leather_cuirass", "Leather Cuirass", "Torso armour made of tanned hide", 4, Armour::Slot::TORSO);

	return;
}

void buildatlas_area(std::map<std::string, Area>& atlas,
	std::map<std::string, Item>& items, std::map<std::string, Weapon>& weapons,
	std::map<std::string, Armour>& armour, std::map<std::string, Creature>& creatures)
{
	// Area definitions are somewhat more complicated:
	atlas["area_01"] = Area("area_01", Dialogue( // Standard dialogue definiton
		"You are in room 1",				// Description
		{"Go to room 2", "Search"}),		// Choices
		Inventory(							// Area inventory
		{
			std::make_pair(&items["item_gold_coin"], 5)	// Pair of item and quantity

		},
		{
			std::make_pair(&weapons["weapon_iron_dagger"], 1)	// Pair of weapon and quantity
		},
		{
			std::make_pair(&armour["armour_leather_cuirass"], 1)	// Pair of armour and quantity
		}),
		{									// Creatures
		});

	atlas["area_02"] = Area("area_02", Dialogue(
		"You are in room 2",
		{"Go to room 1", "Search"}),
		Inventory(
		{
			std::make_pair(&items["item_gold_coin"], 10),
			std::make_pair(&items["item_iron_key"], 1)
		},
		{
		},
		{
		}),
		{
			&creatures["creature_rat"]
		});

	return;
}

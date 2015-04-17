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
		atlas[key] = Creature(key, creature.second);
	}

	return;
}

void buildatlas_item(std::map<std::string, Item>& atlas)
{
	JsonBox::Value v;
	v.loadFromFile("items.json");

	JsonBox::Object o = v.getObject();
	for(auto item : o)
	{
		std::string key = item.first;
		atlas[key] = Item(key, item.second);
	}

	return;
}

void buildatlas_weapon(std::map<std::string, Weapon>& atlas)
{
	JsonBox::Value v;
	v.loadFromFile("weapons.json");

	JsonBox::Object o = v.getObject();
	for(auto weapon : o)
	{
		std::string key = weapon.first;
		atlas[key] = Weapon(key, weapon.second);
	}

	return;
}

void buildatlas_armour(std::map<std::string, Armour>& atlas)
{
	JsonBox::Value v;
	v.loadFromFile("armour.json");

	JsonBox::Object o = v.getObject();
	for(auto armour : o)
	{
		std::string key = armour.first;
		atlas[key] = Armour(key, armour.second);
	}

	return;
}

void buildatlas_area(std::map<std::string, Area>& atlas,
	std::map<std::string, Item>& items, std::map<std::string, Weapon>& weapons,
	std::map<std::string, Armour>& armour, std::map<std::string, Creature>& creatures)
{
	JsonBox::Value v;
	v.loadFromFile("areas.json");

	JsonBox::Object o = v.getObject();
	for(auto area : o)
	{
		std::string key = area.first;
		atlas[key] = Area(key, area.second, items, weapons, armour, creatures);
	}

	return;
}

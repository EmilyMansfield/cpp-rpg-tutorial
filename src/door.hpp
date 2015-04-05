/*
The MIT License (MIT)

Copyright (c) 2014 Daniel Mansfield

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

#ifndef DOOR_HPP
#define DOOR_HPP

#include "item.hpp"

#include <string>
#include <utility>

class Area;

class Door
{
	public:

	// Door description e.g. large wooden door, rusted iron gate
	std::string description;

	// < 0 is open
	// 0 is unlocked but closed
	// > 100 is locked and needs key to open
	int locked;

	// If the player has the required key then they can unlock the door.
	Item* key;

	std::pair<Area*,Area*> areas;

	Door(std::string description, std::pair<Area*,Area*> areas, int locked, Item* key = nullptr)
	{
		this->description = description;
		this->areas = areas;
		this->locked = locked;
		this->key = key;
	}

	Door()
	{

	}
};

#endif /* DOOR_HPP */
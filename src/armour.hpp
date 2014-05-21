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

#ifndef ARMOUR_HPP
#define ARMOUR_HPP

#include "item.hpp"

#include <string>

// Armour should also inherit item properties
class Armour : public Item
{
	public:

	// Armour can go into one of three slots, and only one piece of
	// armour may occupy each slot at a time. The N is there to quickly
	// retrieve the number of slots, without having to use another variable
	enum Slot { TORSO, HEAD, LEGS, N };

	Slot slot;

	// See Battle class for exact formula, values from 1-50 are reasonable
	int defense;

	// Usual constructor
	Armour(std::string name, std::string description, int defense, Armour::Slot slot) :
		Item(name, description)
	{
		this->defense = defense;
		this->slot = slot;
	}

	Armour()
	{
	}
};

#endif /* ARMOUR_HPP */

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

#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "item.hpp"

#include <string>

// Weapons are items, so they should inherit their properties
class Weapon : public Item
{
	public:

	// Weapon damage. See the Battle class for formula, but values
	// between 1-50 are reasonable
	unsigned damage;

	// Modifier to hit chance. Small values are encouraged, e.g.
	// 5-30%
	double hitRate;

	// Pass inherited qualities to the normal item constructor
	Weapon(std::string name, std::string description, int damage, double hitRate) :
		Item(name, description)
	{
		this->damage = damage;
		this->hitRate = hitRate;
	}

	Weapon()
	{
	}
};

#endif /* WEAPON_HPP */

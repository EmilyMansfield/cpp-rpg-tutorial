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

#ifndef ATLAS_HPP
#define ATLAS_HPP

#include <vector>

#include "creature.hpp"
#include "item.hpp"
#include "weapon.hpp"
#include "armour.hpp"
#include "area.hpp"

// Atlas building functions. Atlases contain vectors of game data that
// is not modified in gameplay, so the base versions of creatures, items
// etc. Could easily be replaced with functions that load the information
// from config files instead of just defining the values in code
void buildatlas_creature(std::vector<Creature>& atlas);
void buildatlas_item(std::vector<Item>& atlas);
void buildatlas_weapon(std::vector<Weapon>& atlas);
void buildatlas_armour(std::vector<Armour>& atlas);
void buildatlas_area(std::vector<Area>& atlas,
	std::vector<Item>& items, std::vector<Weapon>& weapons,
	std::vector<Armour>& armour, std::vector<Creature>& creatures);

#endif /* ATLAS_HPP */

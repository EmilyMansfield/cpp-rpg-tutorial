/*
The MIT License (MIT)

Copyright (c) 2015 Daniel Mansfield

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

#ifndef ENTITY_MANAGER_HPP
#define ENTITY_MANAGER_HPP

#include <string>
#include <map>

class Item;
class Weapon;
class Armour;
class Creature;
class Area;

class EntityManager
{
	private:

	std::map<std::string, Item> dataItem;
	std::map<std::string, Weapon> dataWeapon;
	std::map<std::string, Armour> dataArmour;
	std::map<std::string, Creature> dataCreature;
	std::map<std::string, Area> dataArea;

	template<typename T>
	void loadJson(std::string filename, std::map<std::string, T>& data)
	{
			// Fill the data map
			JsonBox::Value v;
			v.loadFromFile(filename);

			JsonBox::Object o = v.getObject();
			for(auto entity : o)
			{
				std::string key = entity.first;
				data[key] = T(key, entity.second, this);
			}

			return;
	}

	public:

	template<typename T>
	void loadJson(std::string filename)
	{
		if(std::is_same<T, Item>::value)			loadJson(filename, dataItem);
		else if(std::is_same<T, Weapon>::value)		loadJson(filename, dataWeapon);
		else if(std::is_same<T, Armour>::value)		loadJson(filename, dataArmour);
		else if(std::is_same<T, Creature>::value)	loadJson(filename, dataCreature);
		else if(std::is_same<T, Area>::value)		loadJson(filename, dataArea);

		return;
	}

	template<typename T>
	T* getEntity(std::string id)
	{
		if(std::is_same<T, Item>::value)			return dynamic_cast<T*>(&dataItem[id]);
		else if(std::is_same<T, Weapon>::value)		return dynamic_cast<T*>(&dataWeapon[id]);
		else if(std::is_same<T, Armour>::value)		return dynamic_cast<T*>(&dataArmour[id]);
		else if(std::is_same<T, Creature>::value)	return dynamic_cast<T*>(&dataCreature[id]);
		else if(std::is_same<T, Area>::value)		return dynamic_cast<T*>(&dataArea[id]);
		else										return nullptr;
	}

	EntityManager() {}

};
#endif /* ENTITY_MANAGER_HPP */
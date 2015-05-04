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

#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include "entity_manager.hpp"
#include "item.hpp"
#include "weapon.hpp"
#include "armour.hpp"

#include <list>
#include <utility>
#include <iostream>
#include "JsonBox.h"

class Inventory
{
	private:

	// Given the Json value v which contains a list of items, weapons, or armour of type T
	// load the Ts into the storage list (either items, weapons, or armour)
	template <typename T>
	void loadItems(JsonBox::Value v, std::list<std::pair<T*, int>>& storage, EntityManager* mgr)
	{
		for(auto item : v.getArray())
		{
			std::string itemId = item.getArray()[0].getString();
			int quantity = item.getArray()[1].getInteger();
			storage.push_back(std::make_pair(mgr->getEntity<T>(itemId), quantity));
		}
	}

	// Add an item to the inventory, specified by a pointer to it
	// Should be from the entity manager
	template <typename T>
	void add_item(T* item, int count, std::list<std::pair<T*, int>>& storage)
	{
		// Perform the same operation as merging, but for a single item
		for(auto& it : storage)
		{
			if(it.first == item)
			{
				it.second += count;
				return;
			}
		}
		// If the item doesn't already exist in the inventory, then a
		// pair must be created too
		storage.push_back(std::make_pair(item, count));
	}

	// Remove the specified number of items from the inventory
	template <typename T>
	void remove_item(T* item, int count, std::list<std::pair<T*, int>>& storage)
	{
		// Iterate through the items, and if they are found then decrease
		// the quantity by the quantity removed
		for(auto& it : storage)
		{
			if(it.first == item)
			{
				it.second -= count;
				break;
			}
		}
		// Iterate through the list again, and remove any elements from
		// the list that have zero or less for their quantity
		// We do this in two passes because removing an element from
		// a list during a for loop invalidates the iterators, and the
		// loop stops working
		storage.remove_if([](std::pair<T*, int>& element)
		{
			return element.second < 1;
		});
	}

	// Returns the count of the specified item
	template <typename T>
	unsigned int has_item(T* item, std::list<std::pair<T*, int>>& storage)
	{
		unsigned int count = 0;
		for(auto it : storage)
		{
			if(it.first == item) ++count;
		}
		return count;
	}

	// Output a list of the items onto stdout, formatted nicely and
	// numbered if required
	template <typename T>
	int print_items(std::list<std::pair<T*, int>>& storage, bool label = false)
	{
		unsigned int i = 1;

		for(auto it : storage)
		{
			// Number the items if asked
			if(label) std::cout << i++ << ": ";
			// Output the item name, quantity and description, e.g.
			// Gold Piece (29) - Glimmering discs of wealth
			std::cout << it.first->name << " (" << it.second << ") - ";
			std::cout << it.first->description << std::endl;
		}

		// Return the number of items outputted, for convenience
		return storage.size();
	}

	template <typename T>
	JsonBox::Array to_json(std::list<std::pair<T*, int>>& storage)
	{
		JsonBox::Array a;
		for(auto item : storage)
		{
			JsonBox::Array pair;
			pair.push_back(JsonBox::Value(item.first->id));
			pair.push_back(JsonBox::Value(item.second));
			a.push_back(JsonBox::Value(pair));
		}

		return a;
	}

	public:

	// Whilst weapons and armour are also items, they have their own
	// specific properties and so cannot be stored inside the same
	// list as the items. We use a list and not a vector as inventories
	// are highly mutable. This way they can also be efficiently sorted
	// The first element of the pair stores a pointer to the item in
	// the item/weapon/armour atlas, defined in main(), and the second
	// element stores the quantity of the item
	std::list<std::pair<Item*, int>> items;
	std::list<std::pair<Weapon*, int>> weapons;
	std::list<std::pair<Armour*, int>> armour;

	Inventory()
	{
	}

	// Load the inventory from a JSON value
	Inventory(JsonBox::Value v, EntityManager* mgr)
	{
		JsonBox::Object o = v.getObject();
		loadItems<Item>(o["items"], this->items, mgr);
		loadItems<Weapon>(o["weapons"], this->weapons, mgr);
		loadItems<Armour>(o["armour"], this->armour, mgr);
	}

	// Remove all items from the inventory, destroying them in the process
	// (They remain in the entity manager though)
	void clear()
	{
		this->items.clear();
		this->weapons.clear();
		this->armour.clear();
	}

	// Template specialisations
	void add_item(Item* item, int count) { add_item<Item>(item, count, this->items); }
	void add_item(Weapon* weapon, int count) { add_item<Weapon>(weapon, count, this->weapons); }
	void add_item(Armour* armour, int count) { add_item<Armour>(armour, count, this->armour); }

	void remove_item(Item* item, int count) { remove_item<Item>(item, count, this->items); }
	void remove_item(Weapon* weapon, int count) { remove_item<Weapon>(weapon, count, this->weapons); }
	void remove_item(Armour* armour, int count) { remove_item<Armour>(armour, count, this->armour); }

	unsigned int has_item(Item* item) { return has_item<Item>(item, this->items); }
	unsigned int has_item(Weapon* weapon) { return has_item<Weapon>(weapon, this->weapons); }
	unsigned int has_item(Armour* armour) { return has_item<Armour>(armour, this->armour); }

	int print_items(bool label = false) { return print_items<Item>(this->items, label); }
	int print_weapons(bool label = false) { return print_items<Weapon>(this->weapons, label); }
	int print_armour(bool label = false) { return print_items<Armour>(this->armour, label); }

	// Merge the specified inventory with the current one, adding
	// item quantities together if they already exist and adding the item
	// into a new slot if they do not
	void merge(Inventory* inventory)
	{
		// You can't merge an inventory with itself!
		if(inventory == this) return;

		// Loop through the items to be added, and add them. Our addition
		// function will take care of everything else for us
		for(auto it : inventory->items)		this->add_item(it.first, it.second);
		for(auto it : inventory->weapons)	this->add_item(it.first, it.second);
		for(auto it : inventory->armour)	this->add_item(it.first, it.second);

		return;
	}

	// Print the entire inventory; items, then weapons, then armour,
	// but if the inventory is empty then output "Nothing"
	void print(bool label = false)
	{
		if(this->items.size() == 0 &&
			this->weapons.size() == 0 &&
			this->armour.size() == 0)
		{
			std::cout << "Nothing" << std::endl;
		}
		else
		{
			this->print_items(label);
			this->print_weapons(label);
			this->print_armour(label);
		}

		return;
	}

	JsonBox::Object to_json()
	{
		JsonBox::Object o;

		o["items"] = JsonBox::Value(to_json<Item>(this->items));
		o["weapons"] = JsonBox::Value(to_json<Weapon>(this->weapons));
		o["armour"] = JsonBox::Value(to_json<Armour>(this->armour));

		return o;
	}
};

#endif /* INVENTORY_HPP */

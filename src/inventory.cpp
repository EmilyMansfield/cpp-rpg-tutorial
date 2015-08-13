#include <list>
#include <string>
#include <utility>
#include <iostream>
#include "JsonBox.h"

#include "inventory.hpp"
#include "item.hpp"
#include "weapon.hpp"
#include "armor.hpp"
#include "entity_manager.hpp"

template <typename T>
void Inventory::load(JsonBox::Value& v, EntityManager* mgr)
{
	for(auto item : v.getArray())
	{
		std::string itemId = item.getArray()[0].getString();
		int quantity = item.getArray()[1].getInteger();
		this->items.push_back(std::make_pair(dynamic_cast<Item*>(mgr->getEntity<T>(itemId)), quantity));
	}
}

template <typename T>
JsonBox::Array Inventory::jsonArray()
{
	JsonBox::Array a;
	for(auto item : this->items)
	{
		// Skip if the id does not match to the type T
		if(item.first->id.substr(0, entityToString<T>().size()) != entityToString<T>())
			continue;
		// Otherwise add the item to the array
		JsonBox::Array pair;
		pair.push_back(JsonBox::Value(item.first->id));
		pair.push_back(JsonBox::Value(item.second));
		a.push_back(JsonBox::Value(pair));
	}

	return a;
}

template <typename T>
void Inventory::add(T* item, int count)
{
	// Perform the same operation as merging, but for a single item
	for(auto& it : this->items)
	{
		if(it.first->id == item->id)
		{
			it.second += count;
			return;
		}
	}
	// If the item doesn't already exist in the inventory, then a
	// pair must be created too
	this->items.push_back(std::make_pair(dynamic_cast<Item*>(item), count));
}

template <typename T>
void Inventory::remove(T* item, int count)
{
	// Iterate through the items, and if they are found then decrease
	// the quantity by the quantity removed
	for(auto& it : this->items)
	{
		if(it.first->id == item->id)
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
	this->items.remove_if([](std::pair<Item*, int>& element)
	{
		return element.second < 1;
	});
}

template <typename T>
unsigned int Inventory::count(T* item)
{
	unsigned int count = 0;
	for(auto it : this->items)
	{
		if(it.first->id == item->id) ++count;
	}
	return count;
}

template <typename T>
T* Inventory::get(unsigned int n)
{
	// Using a list so we don't have random access, and must
	// step through n times from the start instead
	auto it = this->items.begin();
	for(int i = 0; i < n; ++i) { ++it; }
	if(it != this->items.end())
		return dynamic_cast<T*>((*it).first);
	else
		return nullptr;
}

template <typename T>
int Inventory::getq(unsigned int n)
{
	auto it = this->items.begin();
	for(int i = 0; i < n; ++i) { ++it; }
	if(it != this->items.end())
		return (*it).second;
	else
		return 0;
}

template <typename T>
int Inventory::print(bool label)
{
	unsigned int i = 1;

	for(auto it : this->items)
	{
		// Skip if the id does not match to the type T
		if(it.first->id.substr(0, entityToString<T>().size()) != entityToString<T>())
			continue;
		// Number the items if asked
		if(label) std::cout << i++ << ": ";
		// Output the item name, quantity and description, e.g.
		// Gold Piece (29) - Glimmering discs of wealth
		std::cout << it.first->name << " (" << it.second << ") - ";
		std::cout << it.first->description << std::endl;
	}

	// Return the number of items outputted, for convenience
	return this->items.size();
}

// Overload of print to print all items when the template argument is empty
int Inventory::print(bool label)
{
	unsigned int i = 0;

	if(items.empty())
	{
		std::cout << "Nothing" << std::endl;
	}
	else
	{
		i += print<Item>(label);
		i += print<Weapon>(label);
		i += print<Armor>(label);
	}

	return i;
}

void Inventory::clear()
{
	this->items.clear();
}

void Inventory::merge(Inventory* inventory)
{
	// You can't merge an inventory with itself!
	if(inventory == this) return;

	// Loop through the items to be added, and add them. Our addition
	// function will take care of everything else for us
	for(auto it : inventory->items) this->add<Item>(it.first, it.second);

	return;
}

Inventory::Inventory(JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	load<Item>(o["items"], mgr);
	load<Weapon>(o["weapons"], mgr);
	load<Armor>(o["armor"], mgr);
}

JsonBox::Object Inventory::getJson()
{
	JsonBox::Object o;

	o["items"] = JsonBox::Value(jsonArray<Item>());
	o["weapons"] = JsonBox::Value(jsonArray<Weapon>());
	o["armor"] = JsonBox::Value(jsonArray<Armor>());

	return o;
}

// Template instantiations
template void Inventory::load<Item>(JsonBox::Value&, EntityManager*);
template void Inventory::load<Weapon>(JsonBox::Value&, EntityManager*);
template void Inventory::load<Armor>(JsonBox::Value&, EntityManager*);

template JsonBox::Array Inventory::jsonArray<Item>();
template JsonBox::Array Inventory::jsonArray<Weapon>();
template JsonBox::Array Inventory::jsonArray<Armor>();

template void Inventory::add<Item>(Item*, int);
template void Inventory::add<Weapon>(Weapon*, int);
template void Inventory::add<Armor>(Armor*, int);

template void Inventory::remove<Item>(Item*, int);
template void Inventory::remove<Weapon>(Weapon*, int);
template void Inventory::remove<Armor>(Armor*, int);

template unsigned int Inventory::count<Item>(Item*);
template unsigned int Inventory::count<Weapon>(Weapon*);
template unsigned int Inventory::count<Armor>(Armor*);

template Item* Inventory::get<Item>(unsigned int);
template Weapon* Inventory::get<Weapon>(unsigned int);
template Armor* Inventory::get<Armor>(unsigned int);

template int Inventory::getq<Item>(unsigned int);
template int Inventory::getq<Weapon>(unsigned int);
template int Inventory::getq<Armor>(unsigned int);

template int Inventory::print<Item>(bool);
template int Inventory::print<Weapon>(bool);
template int Inventory::print<Armor>(bool);


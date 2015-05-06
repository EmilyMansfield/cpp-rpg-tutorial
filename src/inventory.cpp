#include <list>
#include <utility>
#include <iostream>
#include "JsonBox.h"

#include "inventory.hpp"
#include "entity_manager.hpp"
#include "item.hpp"
#include "weapon.hpp"
#include "armor.hpp"

// Given the Json value v which contains a list of items, weapons, or armor of type T
// load the Ts into the storage list (either items, weapons, or armor)
template <typename T>
void Inventory::loadItems(JsonBox::Value v, std::list<std::pair<T*, int>>& storage, EntityManager* mgr)
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
void Inventory::addItem(T* item, int count, std::list<std::pair<T*, int>>& storage)
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
void Inventory::removeItem(T* item, int count, std::list<std::pair<T*, int>>& storage)
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
unsigned int Inventory::hasItem(T* item, std::list<std::pair<T*, int>>& storage)
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
int Inventory::printItems(std::list<std::pair<T*, int>>& storage, bool label = false)
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
JsonBox::Array Inventory::getJson(std::list<std::pair<T*, int>>& storage)
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

Inventory::Inventory()
{
}

// Load the inventory from a JSON value
Inventory::Inventory(JsonBox::Value v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	loadItems<Item>(o["items"], this->items, mgr);
	loadItems<Weapon>(o["weapons"], this->weapons, mgr);
	loadItems<Armor>(o["armor"], this->armor, mgr);
}

// Remove all items from the inventory, destroying them in the process
// (They remain in the entity manager though)
void Inventory::clear()
{
	this->items.clear();
	this->weapons.clear();
	this->armor.clear();
}

// Template specialisations
void Inventory::addItem(Item* item, int count) { addItem<Item>(item, count, this->items); }
void Inventory::addItem(Weapon* weapon, int count) { addItem<Weapon>(weapon, count, this->weapons); }
void Inventory::addItem(Armor* armor, int count) { addItem<Armor>(armor, count, this->armor); }

void Inventory::removeItem(Item* item, int count) { removeItem<Item>(item, count, this->items); }
void Inventory::removeItem(Weapon* weapon, int count) { removeItem<Weapon>(weapon, count, this->weapons); }
void Inventory::removeItem(Armor* armor, int count) { removeItem<Armor>(armor, count, this->armor); }

unsigned int Inventory::hasItem(Item* item) { return hasItem<Item>(item, this->items); }
unsigned int Inventory::hasItem(Weapon* weapon) { return hasItem<Weapon>(weapon, this->weapons); }
unsigned int Inventory::hasItem(Armor* armor) { return hasItem<Armor>(armor, this->armor); }

int Inventory::printItems(bool label = false) { return printItems<Item>(this->items, label); }
int Inventory::printWeapons(bool label = false) { return printItems<Weapon>(this->weapons, label); }
int Inventory::printArmor(bool label = false) { return printItems<Armor>(this->armor, label); }

// Merge the specified inventory with the current one, adding
// item quantities together if they already exist and adding the item
// into a new slot if they do not
void Inventory::merge(Inventory* inventory)
{
	// You can't merge an inventory with itself!
	if(inventory == this) return;

	// Loop through the items to be added, and add them. Our addition
	// function will take care of everything else for us
	for(auto it : inventory->items)		this->addItem(it.first, it.second);
	for(auto it : inventory->weapons)	this->addItem(it.first, it.second);
	for(auto it : inventory->armor)	this->addItem(it.first, it.second);

	return;
}

// Print the entire inventory; items, then weapons, then armor,
// but if the inventory is empty then output "Nothing"
void Inventory::print(bool label = false)
{
	if(this->items.empty() &&
		this->weapons.empty() &&
		this->armor.empty())
	{
		std::cout << "Nothing" << std::endl;
	}
	else
	{
		this->printItems(label);
		this->printWeapons(label);
		this->printArmor(label);
	}

	return;
}

JsonBox::Object Inventory::getJson()
{
	JsonBox::Object o;

	o["items"] = JsonBox::Value(getJson<Item>(this->items));
	o["weapons"] = JsonBox::Value(getJson<Weapon>(this->weapons));
	o["armor"] = JsonBox::Value(getJson<Armor>(this->armor));

	return o;
}

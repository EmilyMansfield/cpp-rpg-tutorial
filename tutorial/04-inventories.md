# Inventories

Now that we've got a few entities that the player will be able to pick
up and collect, we need a way for the player (and others) to store them.
The simplest method to do this is to just give whatever needs to store
items a few arrays (or `std::vector`s, more likely) and let them manage
the entities. The problem with this is there'll be a lot of repeated
code, and it isn't very elegant. Instead what we'll do is create an
`Inventory` class that manages any number of entities derived from and
including `Item`---henceforth all called items---in a manner similar to
the `EntityManager`. Instead of using an `std::map`, we will use an
`std::list` of `std::pair`s, where each pair contains a pointer to the
item (in the `EntityManager`) and a quantity. We'll also create a
selection of helper functions which make it easier to manage the
`Inventory` instead of working directly with the `std::list`.

Every time we need to represent multiple items, we'll use the
`Inventory` class; the items scattered about a room, in a treasure
chest, or held by the player. We'll want a few functions to make life
easier for us when dealing with the `Inventory`s, namely functions to
add and remove items, check if an inventory contains and item and if so
how many, and a function to merge two inventories together.

Now brace yourself, there are a lot of functions in this class and I'm
going to give all their declarations to you in one go!

```cpp
/* inventory.hpp */
#ifndef INVENTORY_HPP
#define INVENTORY_HPP

#include <list>
#include <utility>
#include <JsonBox.h>

#include "entity_manager.hpp"

class Item;
class Weapon;
class Armor;

class Inventory
{
	private:

	// We use a similar method here to in EntityManager where we store
	// a list of base pointers. We use a list and not a vector as inventories
	// are highly mutable. This way they can also be efficiently sorted.
	// The first element of the pair stores a pointer to the item in
	// the EntityManager, and the second element stores the quantity of the item
	std::list<std::pair<Item*, int>> items;

	// Given the Json value v which contains a list of items, weapons, or armor of type T
	// load the Ts into the storage list (either items, weapons, or armor)
	template <typename T>
	void load(JsonBox::Value& v, EntityManager* mgr);

	// Return a JSON representation of all the items of the type T
	template <typename T>
	JsonBox::Array jsonArray();

	public:

	// Add an item to the inventory
	void add(Item* item, int count);

	// Remove the specified number of items from the inventory
	void remove(Item* item, int count);

	// Returns the count of the specified item
	int count(Item* item);
	template <typename T>
	int count(unsigned int n);

	// Return the nth item in the storage list
	template <typename T>
	T* get(unsigned int n);

	// Output a list of the items onto stdout, formatted nicely and
	// numbered if required
	template <typename T>
	int print(bool label = false);

	// Remove all items from the inventory
	void clear();

	// Merge the specified inventory with the current one, adding
	// item quantities together if they already exist and adding the item
	// into a new slot if they do not
	void merge(Inventory* inventory);

	// Load the inventory from a JSON value
	Inventory(JsonBox::Value& v, EntityManager* mgr);
	Inventory() {}

	// Print the entire inventory; items, then weapons, then armor,
	// but if the inventory is empty then output "Nothing"
	int print(bool label = false);

	// Get a Json object representation of the inventory
	JsonBox::Object getJson();
};

#endif /* INVENTORY_HPP */
```

The comments should make most things clear, but basic usage of the
`Inventory` class is as follows; first either the empty constructor or
the JSON constructor are called. If the JSON constructor is called, the
value is taken to have three keys---`"items"`, `"weapons"`, and
`"armor"`---which are then loaded by the `load` function. The
`Inventory` is then queried using the `get`, and `count` functions, and
modified using `add`, `remove`, and `merge`. It's contents can be
displayed on a per-type basis using the `print` function template, or
all together using the `print` function, and then the `Inventory` can be
converted back to a JSON representation using `getJson` (which calls
`jsonArray` on each item type).

Firstly we have the `load` function and the headers.

```cpp
/* inventory.cpp */
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
        this->items.push_back(std::make_pair(mgr->getEntity<T>(itemId), quantity));
    }
}
```

Before we understand the `load` function it makes sense to see the
syntax of a JSON representation of an `Inventory`.

```json
{
    "items": [
        ["item_gold_coin", 5],
        ["item_iron_key", 1]
    ],
    "weapons": [
        ["weapon_dagger", 1]
    ],
    "armor": [
        ["armor_leather", 1]
    ]
}
```

The `load` function operates on an individual key---either `"items"`,
`"weapons"`, or `"armor"`---and so assumes that the `JsonBox::Value` is
an array, where each element of that is another array containing the
item id and its quantity, in that order. `load` therefore converts `v`
to an array and iterates over each of its elements (which will be
`JsonBox::Value`s). For each element it converts it to an array and
extracts the `itemId` and `quantity` from the relevant positions. In one
step it then uses `std::make_pair` and `EntityManager::getEntity` to
obtain a pointer to the item with that id and construct a new
`std::pair<Item*, int>` containing that pointer and the quantity. This
`std::pair` is then appended to the end of the item list.

```cpp
/* inventory.cpp */
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
```

To go in the opposite direction and convert the inventory to a
`JsonBox::Array` we use the `jsonArray` function, which iterates over
all the items in the item list and for each item constructs a new array
which it stores the id and quantity of that item in. This array is then
appended to the main array, which is returned when all items have been
added. Note that because of the structure of the JSON file, if we want
to properly output an `Inventory` we have to be able to output `Item`s,
`Weapon`s, and `Armor` separately. Hence this function accepts a
template argument `T` which it compares each item's id against in order
to confirm that they are the correct type (like the `EntityManager`
does). If they aren't it ignores then and continues to the next element.

```cpp
/* inventory.cpp */
JsonBox::Object Inventory::getJson()
{
    JsonBox::Object o;

    o["items"] = JsonBox::Value(jsonArray<Item>());
    o["weapons"] = JsonBox::Value(jsonArray<Weapon>());
    o["armor"] = JsonBox::Value(jsonArray<Armor>());

    return o;
}
```

To output the entire inventory we use the `getJson` function, which
simply calls `jsonArray` for each each item type and creates a new
`JsonBox::Object` from them.

```cpp
/* inventory.cpp */
void Inventory::add(Item* item, int count)
{
    for(auto& it : this->items)
    {
        if(it.first->id == item->id)
        {
            it.second += count;
            return;
        }
    }
    this->items.push_back(std::make_pair(item, count));
}

void Inventory::remove(Item* item, int count)
{
    // Iterate through the items, and if they are found then decrease
    // the quantity by the quantity removed
    for(auto it = this->items.begin(); it != this->items.end(); ++it)
    {
        if((*it).first->id == item->id)
        {
            (*it).second -= count;
            if((*it).second < 1) this->items.erase(it);
            return;
        }
    }
}
```

`add` takes a pointer to an `Item` and a quantity and checks to see if
the inventory already contains an item with the same id as the given
`Item` pointer. If it does it increases that item's quantity by the
given value and then returns, otherwise the inventory doesn't contain
the item yet and so it creates a new `std::pair<Item*, int>` and adds it
to the item list. C++ implicitly converts pointers to derived pointers
to pointers to base classes (no need for `dynamic_cast`) and so making
this function accept `Item*` instead of `T*` and converting it is
perfectly fine.

`remove` is slightly more complicated, because in order to delete an
element of an `std::list` we have to use the `std::list::erase`
function, which takes an *iterator* as an argument. Iterators are
similar to pointers in that they represent a location, but they're
restricted to a single data structure---in this case the item
list---instead of an arbitrary location.

Whilst a `range-for` loop like in the `add` function has a nice syntax,
it doesn't give us an iterator, and so we use an uglier `for` loop which
does. Iterators also don't give us direct element access via `->` like
pointers do, so we have to use the indirection operator `*` and then `.`
instead. If the id matches then we decrease the quantity of that item by
the `count` and remove it from the list if its new quantity is
non-positive. Note that we aren't using `unsigned int`s for the item
quantity (because JsonBox only supports `int`), and so we don't have any
risk of overflow here.

```cpp
/* inventory.cpp */
template <typename T>
T* Inventory::get(unsigned int n)
{
    // Using a list so we don't have random access, and must
    // step through n times from the start instead
    unsigned int i = 0;
    auto it = this->items.begin();
    for(; it != this->items.end(); ++it)
    {
        if((*it).first->id.substr(0, entityToString<T>().size()) != entityToString<T>())
            continue;
        if(i++ == n) break;
    }
    if(it != this->items.end())
        return dynamic_cast<T*>((*it).first);
    else
        return nullptr;
}
```

The `get` function returns a `T*` pointer to the `n`th element in the
item list that is of the same type as `T`. This function is quite
complicated because we've used an `std::list`---which doesn't have
random element access---instead of an `std::vector`---which does. We
therefore can't just access the `n`th element, and must step through
using iterators instead. Having said that, because we want the `n`the
element of a given type, and not the `n`th element overall, there's no
escaping this even with an `std::vector`.

We iterate over each element in the list, and increment `i` every time
we find a item of type `T`. When we have found `n` such items, we break
from the loop, at which point `it` will be an iterator to the `n`th
element, or will be an iterator past the end of the list
(`std::list::end`) if no element was found. We then return either the
pointer to the item or `nullptr` accordingly.

```cpp
/* inventory.cpp */
int Inventory::count(Item* item)
{
    for(auto it : this->items)
    {
        if(it.first->id == item->id)
            return it.second;
    }
    return 0;
}

template <typename T>
int Inventory::count(unsigned int n)
{
    return count(get<T>(n));
}
```

The two `count` functions are much simpler. The first returns the
quantity of `item` that the inventory contains by finding the first (and
only) `std::pair` in the item list whose first element has the same id
as `item`. The second uses the `get` function to find a pointer of type
`T*` to the `n`th item, which is then implicitly converted to an `Item*`
pointer before being sent to the first `count` function. Note the
difference here; the first `count` returns the quantity for any item,
including those that aren't in the inventory, whereas the second returns
the count for the `n`th element of a given type, not the `n`th element
overall. You could easily overload the function to do that though.

```cpp
/* inventory.cpp */
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
```

Next we have the two `print` functions which format and output the names
and descriptions of the items in the inventory. The first (templated)
function only prints an item if it has the same type as `T`, and has an
optional feature to numerically label each item. By using this label in
conjunction with the templated `get` we'll be able to create a simple
user interface for managing items, where the player selects an item
based on its number. The second `print` function prints all the
different item types in sequence, or `"Nothing"` if the inventory is
empty. Both functions also return the number of items outputted, which
will come in handy later.

```cpp
/* inventory.cpp */
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
    for(auto it : inventory->items) this->add(it.first, it.second);

    return;
}

Inventory::Inventory(JsonBox::Value& v, EntityManager* mgr)
{
    JsonBox::Object o = v.getObject();
    load<Item>(o["items"], mgr);
    load<Weapon>(o["weapons"], mgr);
    load<Armor>(o["armor"], mgr);
}
```

Finally we have the `clear` and `merge` functions, and the JSON
constructor. All of these are pretty self-explanatory in how they work;
`clear` removes all the items from the inventory, `merge` takes the
items in one inventory and adds (not moves) them to the calling one, and
the constructor uses `load` to load the inventory from the
`JsonBox::Value`.

```cpp
/* inventory.cpp */
// Template instantiations
template void Inventory::load<Item>(JsonBox::Value&, EntityManager*);
template void Inventory::load<Weapon>(JsonBox::Value&, EntityManager*);
template void Inventory::load<Armor>(JsonBox::Value&, EntityManager*);

template JsonBox::Array Inventory::jsonArray<Item>();
template JsonBox::Array Inventory::jsonArray<Weapon>();
template JsonBox::Array Inventory::jsonArray<Armor>();

template int Inventory::count<Item>(unsigned int);
template int Inventory::count<Weapon>(unsigned int);
template int Inventory::count<Armor>(unsigned int);

template Item* Inventory::get<Item>(unsigned int);
template Weapon* Inventory::get<Weapon>(unsigned int);
template Armor* Inventory::get<Armor>(unsigned int);

template int Inventory::print<Item>(bool);
template int Inventory::print<Weapon>(bool);
template int Inventory::print<Armor>(bool);
```

Right at the end of the file we have the explicit instantiations of the
template functions for each item type, and with that the inventory
system is done!

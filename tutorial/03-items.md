# Items

The first type of entity that we will make is the `Item` class, which
will be used for anything the player can pick up and collect in their
adventure through the dungeon. We will also be using it as a base class
for `Weapon`s and `Armor`, which as well as being collectible will also
have more useful properties such as damage done or damage reduced. The
`Item` class will define just the attributes they share, specifically a
name and a description.

```cpp
/* item.hpp */
#ifndef ITEM_HPP
#define ITEM_HPP

#include <string>
#include <JsonBox.h>

#include "entity.hpp"

class EntityManager;

class Item : public Entity
{
	public:

	// Name and description of the item
	std::string name;
	std::string description;

	// Constructors
	Item(std::string id, std::string name, std::string description);
	Item(std::string id, JsonBox::Value& v, EntityManager* mgr);

	// Load the item information from the JSON value
	virtual void load(JsonBox::Value& v, EntityManager* mgr);
};

#endif /* ITEM_HPP */
```

`Item` of course inherits from the `Entity` base class, and so shares
the `load` function as well as taking an `id` variable in its
constructor arguments. The second constructor takes the same arguments
as `load` and is required by the `EntityManager` as we discussed
previously. We won't be using the first constructor because all `Item`s
will be defined in JSON files, but its useful if you want to hardcode an
`Item` into the game so I've included it.

Note that like in `entity.hpp` we've forward declared the
`EntityManager` class, because we need to know of its existence. Whilst
`Item` isn't used directly by `EntityManager`, it is used in a template
instantiation so one of the files has to forward declare instead of
`#include`.

The definitions of these functions are all rather simple, and are
contained in the `item.cpp` file.

```cpp
/* item.cpp */
#include <string>
#include <JsonBox.h>

#include "item.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"

Item::Item(std::string id, std::string name, std::string description) : Entity(id)
{
	this->name = name;
	this->description = description;
}

Item::Item(std::string id, JsonBox::Value& v, EntityManager* mgr) : Entity(id)
{
	this->load(v, mgr);
}

void Item::load(JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	this->name = o["name"].getString();
	this->description = o["description"].getString();

	return;
}
```

The first constructor simply sets the name and description of the item
according to the given arguments, and also calls the `Entity`
constructor with the given id. The second calls the `Entity` constructor
before calling the `load()` function in order to load the `Item` from
the given JSON value. This will be a common pattern shared by all our
derived entities. You might wonder why we're bothering to create a
`load` function at all if its only ever called by one constructor, which
does nothing else! Well we want to ensure that each derived entity has a
way to load from JSON files, and C++ unfortunately doesn't let us create
(pure) virtual constructors so we have to force it some other way. Sadly
this means that we can still create derived classes which don't have a
constructor compatible with the `EntityManager`, but the compilation
error that occurs is pretty easy to understand so it isn't much of an
issue.

Note that the `JsonBox::Value` will not be an entire JSON file, but
instead just the value corresponding to a single key extracted by the
`EntityManager`. By converting the value into a `JsonBox::Object` we can
access that value like an `std::map` and extract all the information we
want, namely the item name and description. We then convert the returned
values into strings (because `o["name"]` returns a `JsonBox::Value`),
which are assigned to the `Item`.

Note that here we are simply assuming the passed `JsonBox::Value` has a
certain structure, which in general use is not guaranteed to be the
case. To reduce complexity I haven't included any error handling, but
really you should check here for correct input, as JsonBox doesn't do
that for you.

There's no use being able to load `Item`s if you haven't made any
though! You'll need to create at least one JSON file containing the
items and then load them using the `EntityManager`. I used a single
`items.json` file.

```json
{
	"item_gold_coin": {
		"name": "Gold Coin",
		"description": "A small disc made of lustrous metal"
	},

	"item_iron_key": {
		"name": "Iron Key",
		"description": "A heavy iron key with a simple cut"
	}
}
```

Now that the `Item` class is complete, we an implement two more
entities; `Weapon` and `Armor`. These are pretty much identical to
`Item` but each have an additional member variable corresponding to
either the amount of damage they deal or the amount of incoming damage
they reduce. The follow the same pattern as `Item`, but instead of
calling the `Entity` constructor they call `Item`s.

```cpp
/* armor.hpp */
#ifndef ARMOR_HPP
#define ARMOR_HPP

#include <string>
#include <JsonBox.h>

#include "item.hpp"

class EntityManager;

class Armor : public Item
{
	public:

	int defense;

	// Constructors
	Armor(std::string id, std::string name, std::string description, int defense);
	Armor(std::string id, JsonBox::Value& v, EntityManager* mgr);

	// Load the armor from the Json value
	void load(JsonBox::Value& v, EntityManager* mgr);
};

#endif /* ARMOR_HPP */
```

```cpp
/* armor.cpp */
#include <string>
#include <JsonBox.h>

#include "armor.hpp"
#include "item.hpp"
#include "entity_manager.hpp"

Armor::Armor(std::string id, std::string name, std::string description, int defense) :
	Item(id, name, description)
{
	this->defense = defense;
}

Armor::Armor(std::string id, JsonBox::Value& v, EntityManager* mgr) : Item(id, v, mgr)
{
	this->load(v, mgr);
}

void Armor::load(JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	this->defense = o["defense"].getInteger();

	return;
}
```

Additionally instead of using the `getString` function on the
`JsonBox::Value` we've used `getInteger`, because `defense` is an `int`
and not a `string`. JsonBox also provides `getFloat`, `getDouble`,
`getBoolean`, and `getArray`, the last of which we will use later.

`Weapon` is pretty much identical but with `defense` replaced with
`damage`, so I won't waste space putting it here. For their JSON files I
used separate `weapon.json` and `armor.json` files. (The last weapon is
not for use by the player, but by a rat enemy that we will be creating
later.)

```json
{
	"weapon_dagger": {
		"name": "Dagger",
		"damage": 2,
		"description": "A small blade, probably made of iron. Keep the sharp end away from your body."
	},

	"weapon_rat_claw": {
		"name": "_rat_claw",
		"damage": 1,
		"description": ""
	}
}
```

```json
{
	"armor_leather": {
		"name": "Leather Armor",
		"defense": 1,
		"description": "Armor made from tanned animal hide. It smells a bit funny, but it'd do the job."
	}
}
```

It's time now to start `main.cpp` I think, where we'll just create an
instance of `EntityManager` and load the items. Before you do, make sure
you add those explicit function template instantiations!

```cpp
/* main.cpp */
#include <cstdlib>
#include <ctime>
#include <JsonBox.h>

#include "item.hpp"
#include "weapon.hpp"
#include "armor.hpp"
#include "entity_manager.hpp"

// Keeps track of items, weapons, creatures etc.
EntityManager entityManager;

int main()
{
	// Load the entities
	entityManager.loadJson<Item>("items.json");
	entityManager.loadJson<Weapon>("weapons.json");
	entityManager.loadJson<Armor>("armor.json");

	// Seed the random number generator with the system time, so the
	// random numbers produced by rand() will be different each time
	std::srand(std::time(nullptr));

	return 0;
}
```

We've also seeded the random number generator, which we'll be using
later. Usually the `rand` function returns the same set of random
numbers every time the program is run (doesn't sound very random does
it?), by giving it a different *seed* each time we can guarantee that
the random numbers will be different. C++ now has its own more powerful
[random number
generator](http://en.cppreference.com/w/cpp/header/random) which we
could use instead of C's `rand` function, but its needlessly complicated
when we don't actually care how random our random numbers are.

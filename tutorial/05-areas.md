# Areas

After all that code we still don't have any actual gameplay! Sorry about
that, we have to do a lot of setting up at the beginning, but things
will get more interesting soon.

The user interface for this game is going to be pretty simple; the
player enters a room and is given a short description of what they can
see before being given a numbered list of actions for them to choose
from. They choose one of these options by entering a number, and move on
to the next room (or fight a monster, open a treasure chest, etc.).

Such a system is so simple we could easily rewrite it to suit our needs
every time we needed to ask the player something, but things quickly get
complicated when we remember that players aren't very predictable; you
might ask for a number between 1 and 3, but you can be sure that someone
will give you the answer `4` or perhaps even `"watermelon"`. We need to
cope with cases like that, which will quickly become tedious when we
have to do it every time we want the player to make a choice. A solution
is the `Dialogue` class.

```cpp
/* dialogue.hpp */
#ifndef DIALOGUE_HPP
#define DIALOGUE_HPP

#include <string>
#include <vector>
#include <iostream>
#include <JsonBox.h>

// Gameplay is expressed using dialogues, which present a piece of
// information and some responses, and the ask the user to pick one. If
// they do not pick a valid one then the dialogue loops until they do
class Dialogue
{
	private:

	// Initial piece of information that the dialogue displays
	std::string description;

	// A vector of choices that will be outputted. No numbering is
	// necessary, the dialogue does that automatically
	std::vector<std::string> choices;

	public:

	// Run the dialogue
	int activate()
	{
		// Output the information
		std::cout << description << std::endl;

		// Output and number the choices
		for(int i = 0; i < this->choices.size(); ++i)
			std::cout << i+1 << ": " << this->choices[i] << std::endl;

		// Repeatedly read input from stdin until a valid option is
		// chosen
		int userInput = -1;
		while(true)
		{
			std::cin >> userInput;
			// 'Valid' means within the range of numbers outputted
			if(userInput >= 0 && userInput <= this->choices.size())
			{
				return userInput;
			}
		}
	}

	// Note that the vector is not passed by reference. Whilst that would
	// be more efficient, it forces us to create a vector outside of the
	// constructor. By passing by value we can call the constructor using
	// an initialisation list such as
	// Dialogue my_dialogue("Hello", {"Choice1", "Choice"});
	Dialogue(std::string description, std::vector<std::string> choices)
	{
		this->description = description;
		this->choices = choices;
	}

	// Create a dialogue from a JSON value
	Dialogue(JsonBox::Value& v)
	{
		JsonBox::Object o = v.getObject();
		description = o["description"].getString();
		for(auto choice : o["choices"].getArray())
			choices.push_back(choice.getString());
	}

	Dialogue() {}

	void addChoice(std::string choice)
	{
		this->choices.push_back(choice);
	}

	unsigned int size()
	{
		return this->choices.size();
	}
};

#endif /* DIALOGUE_HPP */
```

Each `Dialogue` has a `description` string and an `std::vector` of
`choices`. These are set by the constructor, but extra choices can be
added using the `addChoice` function. Additionally, the number of
choices can be queried using `size`. When the `activate` function is
called, the `description` and each of the choices are printed and
labelled numerically, starting from 1. (The description isn't labelled.)
The function then loops until the user inputs a sensible response,
namely a number between 0 and the number of choices, inclusive. By
accepting 0 as a possible answer, we gain an option that is not
displayed but always accepted regardless of the dialogue (though the
rest of the program can ignore it); we will use this to pause the game
and open the menu. The menu will be used to display the player's items
and let them manage their equipment.

Each room of the dungeon will have an associated `Dialogue`,
which---like the rest of the game---will be defined in a JSON file. The
syntax for the JSON representation of a `Dialogue` is pretty
simple---you can infer it from the constructor---but here's an example.

```json
{
	"description": "There is a cooked mackerel here",
	"choices": ["Eat the mackerel", "Save the mackerel for later"]
}
```

To actually represent the dungeon, we'll need an `Area` class. Each room
in the dungeon will be represented by one or more areas, where only one
area can be active at a time. (For example, there might be one area for
before the player presses a switch, and one for afterwards, even though
they are both in the same physical room.) Each area will contain a
`Dialogue`, but also an `Inventory` of items that would be lying around
and an array of `Door`s connecting it to other areas. Later on we'll
also introduce `Creature`s for the player to fight and interact with.

```cpp
/* area.hpp */
#ifndef AREA_HPP
#define AREA_HPP

#include <vector>
#include <string>
#include <JsonBox.h>

#include "entity.hpp"
#include "inventory.hpp"
#include "dialogue.hpp"

class EntityManager;
class Door;

// Movement is achieved through the use of areas, which are contained
// units of space consisting of an inventory, a list of creatures and
// a dialogue
class Area : public Entity
{
	public:

	// Dialogue is run whenever the area is entered
	Dialogue dialogue;

	// Items contained within the area. Not split into individual containers
	// for simplicity
	Inventory items;

	// Links between rooms. Every door should have this as one of its area
	// pointers
	std::vector<Door*> doors;

	// Constructors
	Area(std::string id, Dialogue dialogue, Inventory items);
	Area(std::string id, JsonBox::Value& v, EntityManager* mgr);

	// Load the area from the given Json value
	void load(JsonBox::Value& v, EntityManager* mgr);

	// Return a Json object representing the area
	JsonBox::Object getJson();
};

#endif /* AREA_HPP */
```

The `Area` class is quite simple, but notice that it inherits from
`Entity`. Because we'll be defining each area in a JSON file, they'll
have an id and will be managed by the entity manager. With that in mind,
the constructor and `load` declarations should make sense, with the only
new part being the `getJson` function (although this has the same job as
the equivalent function in `Inventory`). We've also forward declared the
`Door` class and used it to construct an `std::vector` of doors; in this
case we could include `door.hpp` for reasons you'll see below, but we
don't need to so we'll forward declare instead.

### Doors ###

Now's the time to say a few things about how `Door` will work, and why
we have a separate `Door` class at all. What we'd like to do is be able
to join each area together so that the player can move between them. An
initial solution to this might be to store a list of pointers to `Area`s
which are the areas the current area is connected to. But we can't store
pointers in JSON files, so we have to store the ids and then convert
them to areas. That's fine, but when loading the areas we will instantly
encounter an id that does not yet have an associated room, and so which
cannot be turned into a pointer. We can solve this problem by reading
`areas.json` twice; first to create each `Area` without the connections,
and the second to add those connections. This isn't very nice though,
because now the constructor doesn't actually construct the `Area`. We
can fix this by making the `Area` store the ids of other areas instead
of pointers, but now another problem presents itself; how do we describe
the connections to the player? Each `Area` only has an id (which is
hardly player friendly) and a description---which is far too lengthy to
use---and so we would have no choice but to just list the doors
numerically. A terrible idea I'm sure you'll agree, as it would be
completely unusable for the player.

An alternate solution---and the one we'll be using---is to use a
separate `Door` class which contains a pair of `Area` ids that it
connects. This allows us to give all sorts of properties to the
connections, such as descriptions or locks, and has the added bonus of
affecting the connection in both areas when only when is changed (e.g.
opened)! By using ids we've kept things to a single pass of
`areas.json`, although now we have to load a separate file instead. We
also have to make sure that we load `doors.json` before `areas.json`,
because each `Area` has pointers to `Door`s.

```cpp
/* door.hpp */
#ifndef DOOR_HPP
#define DOOR_HPP

#include <string>
#include <utility>

#include "entity.hpp"

class Item;
class EntityManager;

class Door : public Entity
{
	public:

	// Door description e.g. large wooden door, rusted iron gate
	std::string description;

	// < 0 is open
	// 0 is unlocked but closed
	// > 0 is locked and needs key to open
	int locked;

	// If the player has the required key then they can unlock the door.
	Item* key;

	std::pair<std::string, std::string> areas;

	Door(std::string id, std::string description, std::pair<std::string, std::string> areas,
		int locked, Item* key = nullptr);
	Door(std::string id, JsonBox::Value& v, EntityManager* mgr);

	void load(JsonBox::Value& v, EntityManager* mgr);
};

#endif /* DOOR_HPP */
```

Once again the `Door` is an entity, and so inherits from the `Entity`
class. Each `Door` also has a description, a status flag to denote
whether it is locked or open, a pointer to an `Item` which acts as a key
for the door (it could be `nullptr` if no key is required), and an
`std::pair` of strings which contain the ids of the `Area`s the `Door`
connects.

```cpp
/* door.cpp */
#include <string>
#include <utility>

#include "door.hpp"
#include "item.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"

Door::Door(std::string id, std::string description, std::pair<std::string, std::string> areas,
	int locked, Item* key) : Entity(id)
{
	this->description = description;
	this->areas = areas;
	this->locked = locked;
	this->key = key;
}

Door::Door(std::string id, JsonBox::Value& v, EntityManager* mgr) : Entity(id)
{
	this->load(v, mgr);
}

void Door::load(JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	this->description = o["description"].getString();
	this->locked = o["locked"].getInteger();
	if(o.find("key") != o.end())
	{
		this->key = mgr->getEntity<Item>(o["key"].getString());
	}
	JsonBox::Array a = o["areas"].getArray();
	if(a.size() == 2)
	{
		this->areas.first = a[0].getString();
		this->areas.second = a[1].getString();
	}

	return;
}
```

`Door` has the usual constructor definitions and also a `load` function
which behaves much like any other we've created. A typical JSON file for
doors would be

```json
{
	"door_01_02": {
		"description": "sturdy wooden door",
		"areas": ["area_01", "area_02"],
		"locked": 1,
		"key": "item_iron_key"
	}
}
```

The `load` function therefore reads a string for the description and an
integer for the locked status, before loading the key as a string and
looking it up in the entity manager if it was specified in the JSON
file. Finally the two id strings are loaded into the `std::pair`.

```cpp
/* area.cpp */
#include <vector>
#include <string>
#include <JsonBox.h>

#include "area.hpp"
#include "door.hpp"
#include "entity.hpp"
#include "inventory.hpp"
#include "dialogue.hpp"
#include "entity_manager.hpp"

Area::Area(std::string id, Dialogue dialogue, Inventory items) : Entity(id)
{
	this->dialogue = dialogue;
	this->items = items;
	for(auto creature : creatures)
	{
		this->creatures.push_back(*creature);
	}
}

Area::Area(std::string id, JsonBox::Value& v, EntityManager* mgr) : Entity(id)
{
	this->load(v, mgr);
}

void Area::load(JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();

	// Build the dialogue
	// This is an optional parameter because it will not be saved
	// when the area is modified
	if(o.find("dialogue") != o.end())
		this->dialogue = Dialogue(o["dialogue"]);

	// Build the inventory
	this->items = Inventory(o["inventory"], mgr);

	// Attach doors
	if(o.find("doors") != o.end())
	{
		this->doors.clear();
		for(auto door : o["doors"].getArray())
		{
			Door* d = nullptr;
			// Each door is either an array of the type [id, locked] or
			// a single id string.
			if(door.isString())
			{
				d = mgr->getEntity<Door>(door.getString());
			}
			else
			{
				d = mgr->getEntity<Door>(door.getArray()[0].getString());
				d->locked = door.getArray()[1].getInteger();
			}
			this->doors.push_back(d);
		}
	}

	return;
}

JsonBox::Object Area::getJson()
{
	JsonBox::Object o;
	// We don't need to save the dialogue because it doesn't change

	// Save the inventory
	o["inventory"] = this->items.getJson();

	// Save the doors
	a.clear();
	for(auto door : this->doors)
	{
		JsonBox::Array d;
		d.push_back(door->id);
		d.push_back(door->locked);
		a.push_back(d);
	}
	o["doors"] = a;

	return o;
}
```

Finally we have `area.cpp`. When loading the area we first check to see
if a `"dialogue"` key is present in the JSON value, and if it is we
create a new `Dialogue` using the appropriate `Dialogue` constructor. We
check for the key's existence instead of assuming it's there (as we do
when loading the `Inventory`) because the `Dialogue` is not a compulsory
key in the JSON file. This is because we're going to have two separate
files which contain `Area` definitions. The first will contain all of
the area used in the game, and will be loaded normally like `items.json`
and `weapons.json` are. The second will contain only those areas which
the player has visited, and will contain the changes that the player
made to those areas. This way we can easily support multiple save games
by loading the first file and then applying the changes in the second.
Since there'll be no way for a player to directly influence a
`Dialogue`, we don't bother including them in the second file, and hence
why they're optional and aren't included in the `getJson` function.

The `"doors"` key is also optional (it might be a bit odd to have a room
with doors but we can't ignore the possibility), but additionally each
`Door` has two JSON forms. The first is simply a string id referring to
the `Door` itself, and the second is that of an array with two elements.
The first is the string id of the `Door`, and the second is an
overriding `locked` status which will be used to remember whether a door
has been opened or unlocked by the player. When saving the `Area` using
the `getJson` function we will use the second form (preventing doors
from mysteriously locking again when the game is loaded) but because the
locked status is defined in `doors.json` we don't want to force the
override to exist, and so allow for the first form.

Since `Area` and `Door` are kinds of entity, don't forget to add the
template specialisations and instantiations for them and make sure you
load an `areas.json` file (after `doors.json`) in `main.cpp` which
contains the areas, such as this one.

```json
{
	"area_01": {
		"dialogue": {
			"description": "You are in room 1",
			"choices": []
		},
		"doors": ["door_01_02"],
		"inventory": {
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
		},
		"creatures": []
	},

	"area_02": {
		"dialogue": {
			"description": "You are in room 2",
			"choices": []
		},
		"doors": ["door_01_02"],
		"inventory": {
			"items": [
				["item_gold_coin", 100]
			],
			"weapons": [],
			"armor": []
		},
		"creatures":
		[
			"creature_rat"
		]
	}
}
```

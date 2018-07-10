# Player

The player can do everything that an ordinary creature can do, and so we
will create a `Player` class that inherits from the `Creature` class but
has a few player-specific properties. For example, ordinary `Creature`s
do not need to be able to level up (the `xp` value is just the amount
they will give to the player upon defeat, in this case) but `Player`s
do. To that end we will add a `level` variable as well as appropriate
`xpToLevel` and `levelUp` functions, which will calculate how much
experience the player needs to level up, and will actually perform the
levelling itself, respectively.

By using JSON files we've made it very easy to extend the game, but
we've also gained the ability to easily save and load game data. Whilst
we don't want to modify the game files themselves, we can easily create
a similar file that contains all the changes the player has made to the
game world. We can then load this file using functions much like the
usual JSON load functions, and use it to overwrite the original loaded
game files, thereby incorporating all the changes that the player has
made and creating a very simple save/load system! Because only the
player can make these changes, we will add these loading functions to
the `Player` class. This also allows us to have a separate save game per
character, simply by naming the save file with the same name as the
player.

```cpp
/* player.hpp */
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <unordered_set>
#include <string>
#include <JsonBox.h>

#include "creature.hpp"

class EntityManager;

class Player : public Creature
{
	public:

	// Name of the player's class
	// Class may be Fighter, Rogue etc
	std::string className;

	// Level of the player
	unsigned int level;

	// Ids of areas visited by the player
	std::unordered_set<std::string> visitedAreas;

	// Constructors
	Player(std::string name, int hp, int strength, int agility, double evasion,
		unsigned int xp, unsigned int level, std::string className);
	Player();
	Player(JsonBox::Value& saveData, JsonBox::Value& areaData, EntityManager* mgr);

	// Calculates the total experience required to reach a certain level
	unsigned int xpToLevel(unsigned int level);

	// Level the player to the next level if it has enough experience
	// to do so, returning true if it could level up and false otherwise.
	bool levelUp();

	// Create a Json object representation of the player
	JsonBox::Object toJson();

	// Save the player to a file named after them
	void save(EntityManager* mgr);

	// Attempt to load all data from the JSON value
	void load(JsonBox::Value& saveData, EntityManager* mgr);
	void loadArea(JsonBox::Value& areaData, EntityManager* mgr);
};

#endif /* PLAYER_HPP */
```

Two additional things to note are the `className` and `visitedAreas`
variables. Like many RPGs, ours will contain a *class system* which is
used to assign a role to the player, in this case either that of a
*Rogue* or a *Fighter*. The player will be able to choose their class
(not the same as the C++ `class`, sorry for the confusing terminology)
when they first start the game, and this choice will affect which of
their attributes increases the fastest when they level up. In our very
simple system the Fighter will have their strength increase faster than
their agility, and the Rogue will have their agility increase faster
than their strength. This small choice is very easy to implement and
adds a bit of depth to the game. The `visitedAreas` variable is tied to
the saving system and will store the ids of each `Area` the player has
visited (and hence possible affected). Each of these `Area`s is assumed
to have been modified by the player, and so will be saved to the save
file. We use the `std::unordered_set` class to store the ids because it
ensures that each id is only stored once, and it also doesn't care about
the order the ids are entered (and neither should we because JSON
doesn't either).

First we will look at the saving and loading functions.

```cpp
/* player.cpp */
#include <unordered_set>
#include <cmath>
#include "JsonBox.h"

#include "area.hpp"
#include "player.hpp"
#include "creature.hpp"
#include "entity_manager.hpp"

Player::Player(std::string name, int hp, int strength, int agility, double evasion,
	unsigned int xp, unsigned int level, std::string className) :
	Creature("player", name, hp, strength, agility, evasion, xp)
{
	this->level = level;
	this->className = className;
}

Player::Player() : Player::Player("", 0, 0, 0, 0.0, 0, 1, "nullid")
{
}

Player::Player(JsonBox::Value& saveData, JsonBox::Value& areaData, EntityManager* mgr) : Player::Player()
{
	this->load(saveData, mgr);
	this->loadArea(areaData, mgr);
}

JsonBox::Object Player::toJson()
{
	JsonBox::Object o = Creature::toJson();

	o["className"] = JsonBox::Value(this->className);
	o["level"] = JsonBox::Value(int(this->level));

	return o;
}

void Player::save(EntityManager* mgr)
{
	// Construct JSON representation of the player
	// and save it to a file
	JsonBox::Value v(this->toJson());
	v.writeToFile(this->name + ".json");

	// Construct a JSON object containing the areas
	// the player has visited
	JsonBox::Object o;
	for(auto area : this->visitedAreas)
	{
		o[area] = mgr->getEntity<Area>(area)->getJson();
	}
	JsonBox::Value v2(o);
	// Write the object to a file similar to the player data
	v2.writeToFile(this->name + "_areas.json");

	return;
}

// Attempt to load all data from the JSON value
void Player::load(JsonBox::Value& saveData, EntityManager* mgr)
{
	// Load data shared with Creature
	Creature::load(saveData, mgr);

	// Load optional variables
	JsonBox::Object o = saveData.getObject();

	this->className = o["className"].getString();
	this->level = o["level"].getInteger();

	return;
}

void Player::loadArea(JsonBox::Value& areaData, EntityManager* mgr)
{
	// Load the area
	JsonBox::Object o = areaData.getObject();
	for(auto area : o)
	{
		std::string key = area.first;
		mgr->getEntity<Area>(key)->load(area.second, mgr);
		this->visitedAreas.insert(key);
	}

	return;
}
```

The constructors are quite self-explanatory, and just initialise the new
member variables. The third constructor takes two `JsonBox::Value`s
instead of one however, and calls two functions; the first is the actual
save data for the player (their `hp`, `inventory`, and so on), whereas
the second is for saving and loading the modified areas, as mentioned
above. The `toJson` functions is also very simple, and just calls the
`Creature` version of `toJson` before appending the new variables on the
end.

The `save` function takes care of both the standard and area saving. To
save the modified areas, the function iterates over each of the
`visitedAreas` and creates JSON representations of them, which are then
added to a new JSON object using their id as the key. By using
`std::unordered_set` we have assured that all the ids are unique, and so
no overwriting will occur here. Finally the JSON object is written to a
file named the same as the player but with an `"_areas"` suffix appended
to it. Finally, the `loadArea` function does the reverse and iterates
over all the areas saved in the `"player-name_areas.json"` file, adding
each of them to the the `visitedAreas` list. It also uses the
`EntityManager` to call the `load` function on each new area; this
overwrites the original `Area` (which will have already been loaded by
the `EntityManager` when the program started) with the changes made by
the player, essentially loading the save.

Next we have the two levelling functions.

```cpp
/* player.cpp */
// Calculates the total experience required to reach a certain level
unsigned int Player::xpToLevel(unsigned int level)
{
	return (unsigned int)(1.5 * std::pow(this->level, 3));
}

// Level the player to the next level if it has enough experience
// to do so, returning true if it could level up and false otherwise.
bool Player::levelUp()
{
	// Can't level up if there's not enough experience
	if(this->xp < xpToLevel(this->level+1))
	{
		return false;
	}

	// Advance to the next level
	++level;

	// Variables to keep track of stat changes, and their associated
	// multipliers, which depend on the class. The multiplier affects
	// how much that stat increases each level, and is higher if the
	// class specialises in that stat
	// [hp, strength, agility]
	unsigned int statIncreases[3] = {0, 0, 0};
	float statMultipliers[3] = {0, 0, 0};
	statMultipliers[0] = 13.0;
	statMultipliers[1] = this->className == "Fighter" ? 8.0 : 6.0;
	statMultipliers[2] = this->className == "Rogue" ? 8.0 : 6.0;

	// Compute the stat increases for each stat
	for(int i = 0; i < 3; ++i)
	{
		float base = std::tanh(this->level / 30.0) * ((this->level % 2) + 1);
		statIncreases[i] += int(1 + statMultipliers[i] * base);
	}

	// Adjust all of the stats accordingly
	this->hp += statIncreases[0];
	this->maxHp += statIncreases[0];
	this->strength += statIncreases[1];
	this->agility += statIncreases[2];

	// Tell the user that they grew a level, what the increases were
	// and what their stats are now
	std::cout << this->name << " grew to level " << level << "!\n";
	std::cout << "Health   +" << statIncreases[0] << " -> " << this->maxHp << std::endl;
	std::cout << "Strength +" << statIncreases[1] << " -> " << this->strength << std::endl;
	std::cout << "Agility  +" << statIncreases[2] << " -> " << this->agility << std::endl;
	std::cout << "----------------\n";

	return true;
}
```

In our RPG, the experience required to advance to level `n` is
independent of class, and equal to `1.5 n^3`. (This is far from a
perfect formula, and most systems are [far more
complicated](http://bulbapedia.bulbagarden.net/wiki/Experience).) When
the player levels up we first compute a *stat multiplier* for each
attribute (hit points, strength, agility). The hit points multiplier is
always 13.0, whereas the strength and agility multipliers are either 8.0
or 6.0, depending on the player's class. Each multiplier `k` is then
fed into the slightly scary formula

<figure>
    <img src="/07-experience-formula.svg" alt="Stat multiplier formula
    given by 1 + k * tanh(n / 30), all multiplied by n mod 2 + 1, then
    floored.">
    <figcaption>Stat increase per level in terms of the multiplier
    `k`</figcaption>
</figure>

This function and its cumulative form look like

<figure>
    <img src="/increase-experience-graph.svg" alt="Attribute increases">
    <figcaption>Attribute increases per level</figcaption>
</figure>

<figure>
    <img src="/cumulative-experience-graph.svg" alt="Total attributes">
    <figcaption>Total attribute at given level</figcaption>
</figure>

As you can see, the attribute increases spike at every other level but
still slowly increases as the player's level goes up. This gives us a
levelling system that seems a little bit random, but is actually quite
regular. Each stat is then increased by the calculated value, and
finally the new attributes and level are reported to the player.

Don't add any functionality to load the player as an entity like with
the `Creature`s, we'll be doing that in a different way to handle the
save files.

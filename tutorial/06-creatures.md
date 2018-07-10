# Creatures

We've almost got a playable game, I promise! I think that jumping
backwards and forwards is quite a confusing way to learn, hence why
we're building all the individual components separately and then joining
them all together at the end. Anyway, in this section we'll begin to
think about how fighting will work in this game, and we'll be creating a
`Creature` class accordingly.

The battle system in this game will be very simple; every fight will
involve two sides---the player and the enemies---who will take turns
attacking each other. Unlike in some RPGs a turn will not consist of all
the enemies attacking and then the player attacking, but rather every
creature involved will attack at a separate time independent of their
side. To determine the order in which each creature attacks, we will use
an *agility* value; the higher their agility, the earlier they attack.
Once all creatures have attacked once, the turn starts again.

When a creature attacks, they will not be guaranteed to hit and will
instead have a chance to do damage that decreases as the defender's
*evasion* increases. If the attacker hits the defender the amount of
damage done will be determined by what weapon the attacker has equipped,
how much *strength* the attacker has, and what armor the defender has
equipped. It will also be modified by a small random factor to add a
little bit of excitement into the mix! The amount of damage the attacker
does will be subtracted from the defender's *hit points*, and when their
*hit points* are reduced to zero the defender will die.

With that in mind, the `Creature` class should have `hp`, `strength`,
`agility`, `evasion`, `equippedWeapon`, and `equippedArmor` member
variables. To allow for healing and restoration of hit points we should
also have a `maxHp` value, and of course each `Creature` should also
have a `name`. Finally, no RPG is complete without experience points and
loot drops, so we'll also give each `Creature` an `xp` value and an
`inventory`. It'll also be important to know which `Area` the `Creature`
is in, and we should add a function to allow `Creature`s to move from
one `Area` to another.

```cpp
/* creature.hpp */
#ifndef CREATURE_HPP
#define CREATURE_HPP

#include <string>
#include <cstdlib>
#include <JsonBox.h>

#include "entity.hpp"
#include "inventory.hpp"

class Area;
class EntityManager;
class Weapon;
class Armor;
class Door;

class Creature : public Entity
{
	public:

	// Name of the creature
	std::string name;

	// Creature stats
	int hp;
	int maxHp;
	int strength;
	int agility;
	double evasion;
	unsigned int xp;

	// Items that the creature possesses
	Inventory inventory;

	// Currently equipped weapon. Used as a pointer to an atlas entry,
	// but not necessary. nullptr denotes that no weapon is equipped
	Weapon* equippedWeapon;

	// Currently equipped armor
	Armor* equippedArmor;

	// Area the creature resides in. Used for player motion but also could
	// be used for enemy AI
	std::string currentArea;

	// Constructors
	Creature(std::string id, std::string name, int hp, int strength, int agility, double evasion,
		unsigned int xp);
	Creature(std::string id, JsonBox::Value& v, EntityManager* mgr);

	// Equip a weapon by setting the equipped weapon pointer. Currently
	// a pointless function (simple enough to be rewritten each time)
	// but handy if dual wielding is ever added, or shields etc
	void equipWeapon(Weapon* weapon);

	// Equip the armor into it's correct slot. A slightly more useful
	// function!
	void equipArmor(Armor* armor);

	// Convert internal area id into a pointer
	Area* getAreaPtr(EntityManager* mgr);

	// Go through a door
	// 0 = Door is locked
	// 1 = Door unlocked using key
	// 2 = Door is open
	int traverse(Door* door);

	// Create a JSON object containing the creature data
	virtual JsonBox::Object toJson();

	// Attempt to load all data from the JSON value
	virtual void load(JsonBox::Value& v, EntityManager* mgr);
};

#endif /* CREATURE_HPP */
```

Note that we've forward-declared each class that we're only storing a
pointer to, but we've had to `#include "inventory.hpp"` because we are
storing an actual instance of `Inventory` instead of a pointer. The
usual constructors and JSON-related functions are present, but we've got
a few more functions as well to make dealing with the `Creature` a
little easier, most notably the `getAreaPtr` function. So far we've
almost exclusively stored pointers to entities instead of their ids, but
here we've stored the id instead, and have added a function to convert
it to a pointer when necessary. Honestly there isn't much of a reason
for this besides making things a little bit simpler later on, so feel
free to change it back to the usual method.

```cpp
/* creature.cpp */
#include <string>
#include <cstdlib>
#include <JsonBox.h>

#include "creature.hpp"
#include "entity.hpp"
#include "inventory.hpp"
#include "weapon.hpp"
#include "armor.hpp"
#include "door.hpp"
#include "area.hpp"
#include "entity_manager.hpp"

Creature::Creature(std::string id, std::string name, int hp, int strength, int agility, double evasion,
	unsigned int xp) : Entity(id)
{
	this->name = name;
	this->hp = hp;
	this->maxHp = hp;
	this->strength = strength;
	this->agility = agility;
	this->evasion = evasion;
	this->equippedArmor = nullptr;
	this->equippedWeapon = nullptr;
	this->xp = xp;
}

Creature::Creature(std::string id, JsonBox::Value& v, EntityManager* mgr) :
	Creature(id, "", 0, 0, 0, 0, 0)
{
	this->load(v, mgr);
}

void Creature::equipWeapon(Weapon* weapon)
{
	this->equippedWeapon = weapon;

	return;
}

void Creature::equipArmor(Armor* armor)
{
	this->equippedArmor = armor;

	return;
}

Area* Creature::getAreaPtr(EntityManager* mgr)
{
	return mgr->getEntity<Area>(this->currentArea);
}

int Creature::traverse(Door* door)
{
	int flag = 2;
	// Open the door if it is shut
	if(door->locked == 0)
	{
		door->locked = -1;
		flag = 2;
	}
	else if(door->locked > 0)
	{
		// Unlock and open the door if the creature has the key
		if(this->inventory.count(door->key))
		{
			door->locked = -1;
			flag = 1;
		}
		// Creature does not have key so door remains locked
		else
		{
			return 0;
		}
	}
	if(door->areas.first == this->currentArea)
	{
		this->currentArea = door->areas.second;
	}
	else if(door->areas.second == this->currentArea)
	{
		this->currentArea = door->areas.first;
	}

	return flag;
}

JsonBox::Object Creature::toJson()
{
	JsonBox::Object o;
	o["name"] = JsonBox::Value(this->name);
	o["hp"] = JsonBox::Value(this->hp);
	o["hp_max"] = JsonBox::Value(this->maxHp);
	o["strength"] = JsonBox::Value(this->strength);
	o["agility"] = JsonBox::Value(this->agility);
	o["evasion"] = JsonBox::Value(this->evasion);
	o["xp"] = JsonBox::Value(int(this->xp));
	o["inventory"] = JsonBox::Value(this->inventory.getJson());
	o["equipped_weapon"] = JsonBox::Value(this->equippedWeapon == nullptr ? "nullptr" : this->equippedWeapon->id);
	o["equipped_armor"] = JsonBox::Value(this->equippedArmor == nullptr ? "nullptr" : this->equippedArmor->id);

	return o;
}

void Creature::load(JsonBox::Value& v, EntityManager* mgr)
{
	JsonBox::Object o = v.getObject();
	this->name = o["name"].getString();
	this->hp = o["hp"].getInteger();
	if(o.find("hp_max") != o.end())
	{
		this->maxHp = o["hp_max"].getInteger();
	}
	else
	{
		this->maxHp = hp;
	}
	this->strength = o["strength"].getInteger();
	this->agility = o["agility"].getInteger();
	this->evasion = o["evasion"].getDouble();
	this->xp = o["xp"].getInteger();

	if(o.find("inventory") != o.end())
	{
		this->inventory = Inventory(o["inventory"], mgr);
	}
	if(o.find("equipped_weapon") != o.end())
	{
		std::string equippedWeaponName = o["equipped_weapon"].getString();
		this->equippedWeapon = equippedWeaponName == "nullptr" ? nullptr : mgr->getEntity<Weapon>(equippedWeaponName);
	}
	if(o.find("equipped_armor") != o.end())
	{
		std::string equippedArmorName = o["equipped_armor"].getString();
		this->equippedArmor = equippedArmorName == "nullptr" ? nullptr : mgr->getEntity<Armor>(equippedArmorName);
	}

	return;
}
```

Most of the functions here are just variations on ones we've seen before
(though there's a lot more data involved here), but `traverse` is more
interesting. The `traverse` function requires a `Door` that the
`Creature` is attempting to travel through. First it checks the `locked`
property of the `door`. If the `door` is closed but unlocked or locked
but the `Creature` has the key then it is opened. Once the door is
opened the `Creature` is moved to the other `Area` linked to by the
`door`. It makes sense to make `traverse` a member function of
`Creature` and not `Door` because the `Creature` is the one doing the
action (though it's perfectly possible to do it the other way around).

Before moving on, don't forget to load the creatures from a JSON file
similarly to the other entities we've created, and also add the template
specialisations and instantiations to `entity_manager.cpp`. Since the
`Creature` class makes use of `Weapon`s and `Armor`, load
`creatures.json` after those.

```json
{
	"creature_rat": {
		"name": "Rat",
		"hp": 3,
		"strength": 5,
		"agility": 3,
		"evasion": 0.015625,
		"xp": 1,
		"equipped_weapon": "weapon_rat_claw"
	}
}
```

### Creatures and Areas ###

We've said that we want a battle system, but how is the player going to
encounter all those other `Creature`s in the first place? Since they're
entities that appear in the game world, it makes sense to add them to
the `Area` class like everything else. This isn't very hard to do so
I'll leave it mostly to you, but there is something you have to take
into account that's easy to miss.

Note that `Creature`s are the only entities that are both *modifiable*
(`Area`s also are) and *multi-instance* (`Item`s also are). Modifying an
`Area` was fine because each `Area` was only ever used once, and using
the same `Item` multiple times was fine because they were never
modified. `Creature`s however will be reused many times (imagine having
to create a separate JSON entry every time a Rat appeared) but will also
take damage, and so will be modified. If we store pointers to their
positions in the Entity Manager, every Rat will be identical and when
one Rat takes damage, they all will! Obviously this would be disastrous,
so instead of storing pointers to `Creature`s in the `Area` we should
store actual `Creature`s. The `Creature` constructor can still use
pointers though, because we can leave the duplication up to the `Area`
class. Hence

```cpp
std::vector<Creature> creatures;

// But...

Area(std::string id, Dialogue dialogue, Inventory items,
	std::vector<Creature*> creatures);
```

To build the member `creatures` list, iterate over the argument
`creatures` list and add use `*` to dereference the creature and add it
to the list. Now for the JSON changes. These aren't much harder, you
just have to account for the presence of a `"creatures"` key, e.g. for
`load`

```cpp
// Build the creature list
this->creatures.clear();
for(auto creature : o["creatures"].getArray())
{
	// Create a new creature instance indentical to the version
	// in the entity manager
	Creature c(*mgr->getEntity<Creature>(creature.getString()));
	this->creatures.push_back(c);
}
```

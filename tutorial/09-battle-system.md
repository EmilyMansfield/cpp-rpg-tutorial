# Battle System

We spent quite a long time discussing how a battle system worked a few
tutorials ago, but we didn't atually write one. Until now! This tutorial
will be the last of the series, and once we're done you should have a
very simple but still playable---and decently fun, I hope---RPG. Since
we've already decided how battles will be fought, we can jump right into
the technical details. This is good, because they actually require a
decent bit of thought.

Our aim is to construct a system where each participant (player or
enemy) attacks in sequence depending on their agility. At the very least
then, we'll need a list of participants that we can easily sort into
agility order. This list will need to contain variables of type
`Creature*`, because it has to store both a pointer to the player (of
class `Player`) and to the enemies (of class `Creature`). Note that we
don't lose any information this way, because we can still tell if a
`Creature*` points to a `Player` by checking the id, which will be
`"player"` if and only if the pointer points to a `Player` class.

We'll also need some way of describing a general action that a
participant can make during a turn. In our system we will just have
*attack* and *defend* (though *use*, *cast*, *throw*, *flee*, etc. would
probably be wanted later), and to make matters even simpler *defend*
won't even do anything! Since all actions involve participant A doing
something to participant B, it makes sense for one of these
actions---called *battle events*---to have a `source` participant, a
`target` participant, and an event `type`. By using a `BattleEvent`
class we can easily combine the necessary variables together and also
add a handy `run` function to actually make the `source` perform the
`type` on the `target`.

```cpp
/* battle.hpp */
class Creature;

// Possible event types, should equate to what the player
// can do in a battle
enum class BattleEventType { ATTACK, DEFEND };

class BattleEvent
{
	public:

	// Creature that initiated the event, e.g. the attacker
	Creature* source;
	// Creature being affected, e.g. the one being attacked
	Creature* target;
	// Type of event, e.g. attack, or defense
	BattleEventType type;

	// Constructor
	BattleEvent(Creature* source, Creature* target, BattleEventType type);

	// Convert the event type to the corresponding function and call it
	// on the source and target
	int run();
};
```

Now we'll still need some kind of list of participants, but because the
player needs to be able to select a target, it actually makes more sense
to have a list that has a fixed and numerically accessible order. In
order words, we want an `std::vector`. Since we like classes so much
we'll create a `Battle` class that will handle each battle using `run`
and `nextTurn` functions. `run` will be called once when the battle is
started (we don't use the constructor for this so that the `Battle` can
be set up in advance if desired), and `nextTurn` will be called at the
start of each combat turn (i.e. after every participant has attacked the
same number of times).

```cpp
/* battle.hpp */
class Battle
{
	private:

	// All the creatures that are participating in the fight.
	// We assume the player is a Creature with id "player".
	// A vector is used because we need to get the nth element
	// for use with a Dialogue
	std::vector<Creature*> combatants;

	// Actions that the player can take in the battle
	Dialogue battleOptions;

	// Remove a creature from the combatants list, and report that it's dead
	void kill(Creature* creature);

	// Run the next turn for the enemies and the player.
	// Computes what the enemies should do and asks for the player's
	// action, then compiles an event queue of the actions before
	// proceeding through the queue and running each action.
	void nextTurn();

	public:

	// Constructor
	Battle(std::vector<Creature*>& combatants);

	// Run the battle until either the player dies, or all the opposing
	// combatants do
	void run();
};
```

The `BattleEvent` member functions definitions are some of the simplest
yet.

```cpp
/* battle.cpp */
BattleEvent::BattleEvent(Creature* source, Creature* target, BattleEventType type)
{
	this->source = source;
	this->target = target;
	this->type = type;
}

int BattleEvent::run()
{
	switch(type)
	{
		case BattleEventType::ATTACK:
			return source->attack(target);
		case BattleEventType::DEFEND:
			return 0;
		default:
			return 0;
	}
}
```

The return value of `run` can be used to pass useful information back to
the `Battle`, in the case of attack and defend actions the amount of
damage dealt is returned so that it can be reported back to the player;
this means that we need to write an `attack` function in the `Creature`
class that does just that! Heading back to `creature.cpp` then (don't
forget the prototype in `creature.hpp`) we'll create the `attack`
function, which will calculate if an attack hits and if it does, how
much damage it deals. It will also subtract the damage to save us doing
that in the `Battle`.

```cpp
/* creature.hpp */
// Note that 'x in range [a, b]' in the comments means that
// a <= x <= b. This is called interval notation, and is a
// common mathematical shorthand. Using ( or ) instead of [ or ]
// means that there's no = on the inequality on that side of the x.
int Creature::attack(Creature* target)
{
	// Damage done
	int damage = 0;

	if(double(std::rand()) / RAND_MAX  > target->evasion)
	{
		// Calculate attack based on strength and weapon damage
		int attack = this->strength + (this->equippedWeapon == nullptr ? 0 : this->equippedWeapon->damage);
		// Calculate defense based on agility and armor defense
		int defense = target->agility + (target->equippedArmor == nullptr ? 0 : target->equippedArmor->defense);
		// 1/32 chance of a critical hit
		if(std::rand() % 32 == 0)
		{
			// Ignore defense and do damage in range [attack/2, attack]
			damage = attack / 2 + std::rand() % (attack / 2 + 1);
		}
		else
		{
			// Normal hit so factor in defense
			int baseDamage = attack - defense / 2;
			// Do damage in range [baseDamage/4, baseDamage/2]
			damage = baseDamage / 4 + std::rand() % (baseDamage / 4 + 1);
			// If the damage is zero then have a 50% chance to do 1 damage
			if(damage < 1)
			{
				damage = std::rand() % 2;
			}
		}
		// Damage the target
		target->hp -= damage;
	}

	return damage;
}
```

The comments in the code describe the mechanics of the `attack` function
and how it calculates damage, but in short the weapon damage and
strength of the attacker are compared against the agility and armor
defense of the target. The greater the difference the greater the
average damage dealt will be, but also the greater the range of damage
will be. If an attack does less than 1 point of damage then there is a
50% chance that the attack will do 0 damage or 1 damage (this stops
battles from lasting forever), and each attack has a 1/32 chance of
dealing a *critical hit*, which ignores the target's agility and armor
to deal more damage. This is all assuming the attack hits, which has a
chance equal to `1-target->evasion` of occuring. If you haven't seen
`RAND_MAX` before, it is a macro that evaluates to the largest integer
that `std::rand()` will generate. By casting the value returned by
`rand()` to a `double` and dividing by `RAND_MAX`, we obtain a `double`
in the range `[0.0,1.0]` which has the highest possible precision
achievable using `rand()`. It's a neat trick!

Moving back to `battle.cpp`, we can begin to implement the `Battle`
member functions. First up is the constructor.

```cpp
/* battle.cpp */
Battle::Battle(std::vector<Creature*>& combatants)
{
	this->combatants = combatants;

	// Construct the menu
	this->battleOptions = Dialogue("What will you do?",
	{
		"Attack",
		"Defend"
	});

	// Store the unique creature names and whether there is
	// only one or more of them. This code assumes that the
	// creatures have not already been assigned unique names,
	// which should be the case as a battle cannot be left
	// and then resumed again
	std::map<std::string, int> names;
	for(auto com : this->combatants)
	{
		// Skip the player, who shouldn't be renamed
		if(com->id == "player") continue;
		// If the name hasn't been recorded and the creature
		// isn't the player, record the name
		if(names.count(com->name) == 0)
		{
			names[com->name] = 0;
		}
		// If there is already one creature, record there are being
		// more than one. We don't want the actual number, simply
		// that there's more and so we should label them.
		else if(names[com->name] == 0)
		{
			names[com->name] = 1;
		}
	}

	// Creature unique names for the combatants
	for(auto& com : this->combatants)
	{
		std::string newName = com->name;
		// If the name is marked as being shared by more than
		// one creature
		if(names.count(com->name) > 0 && names[com->name] > 0)
		{
			// Append (1) to the end of the name, and then increase the
			// number for the next creature
			newName += " (" + std::to_string(names[com->name]) + ")";
			names[com->name] += 1;
		}
		// Change the creature name to the new one, which might just be
		// the same as the original
		com->name = newName;
	}
}
```

The `std::vector` passed to the constructor should contain pointers to
unique creatures---such as those stored in each `Area`---but although
each creature is unique (in the sense that no two pointers point to the
same location in memory), they do not have unique names. Because this is
a text-based game, the names are the only way to distinguish between two
creatures, and so without unique names the player will not be able to
tell two `"Rat"`s apart. As such, the bulk of the constructor is
dedicated to creating unique names for the different participants; if
there are two `"Rat"`s in the battle, then they will be renamed `"Rat
(1)"` and `"Rat (2)"`, for example.

To do this, we first construct an `std::map` which maps each of the
(non-unique) names of the participants to a number; 0 if there is only
one creature with that name, and 1 otherwise. This tells us whether each
name should be labelled or not. We skip the player because in our game
the player cannot attack themselves; if you expanded the game to allow
for that (perhaps the player can heal themselves with magic) you should
have a more robust system that deals with the player and a creature
sharing a name.

Once the `std::map` is constructed, we take each participant and check
if their name should be labelled or not. If they should be, we label
them with the corresponding number in the `std::map` and then increase
that number, ensuring that it's always equal to the next label
necessary.

After the constructor has been called and the unique names have been
created, the battle can be started using the `run` function.

```cpp
/* battle.cpp */
void Battle::run()
{
	std::vector<Creature*>::iterator player;
	std::vector<Creature*>::iterator end;
	do
	{
		// Continue the battle until either the player dies,
		// or there is only the player left
		player = std::find_if(this->combatants.begin(), this->combatants.end(),
			[](Creature* a) { return a->id == "player"; });
		end = this->combatants.end();

		this->nextTurn();
	}
	while(player != end && this->combatants.size() > 1);

	return;
}
```

OK, this part will be unfamiliar if you haven't seen iterators before.
An iterator is similar to a pointer in that it commonly refers to a
entry in some data structure (in this case and `std::vector` containing
`Creature` pointers), but they're actually objects with some additional
properties and restrictions. Here we're making use of them because
they're the return type of the `std::find_if` function (which is
included in the `algorithm` header), which takes an starting iterator,
an ending iterator, and a *lambda*. `std::find_if` then scans all the
elements in the data structure between the starting and ending iterator
(in this case the beginning and end of the combatants list) and passes
them to the lambda function. `std::find_if` returns an iterator to the
first element it scanned where the lambda returned `true`.

The [syntax for a
lambda](http://en.cppreference.com/w/cpp/language/lambda) is a little
weird and can get quite complicated, but here we have the simplest kind
of lambda, one that has an empty *capture list* `[]`. Refer to the link
for the capture list syntax, but essentially the capture list acts like
an additional function argument list that is usable when the actual
function argument list is restricted. In this case, `std::find_if`
demands that the lambda has a single argument (which is the element in
the data structure it is currently scanning) and we don't want to pass
anything else, so we leave the capture list empty. After that the lambda
is just like a normal function, which in this case simply returns `true`
if the scanned `Creature` is the player, and `false` otherwise. So
actually, all this piece of code does is find the player in the
combatant list!

So why did we go through all that when we could have just used a simple
`for` loop and pointer combination? Actually I just wanted to show you
something new, there wasn't really a reason!

Anyway, this `do-while` loop just finds the player, starts the next
turn, and continues until the player is no longer in the list, i.e. has
died, or the list of combatants is reduced to 1 (in which case the
player will either be dead and there be one enemy left, or the player is
the last participant alive). Looking closely though, aren't we checking
to see if the player is at the end of the combatants list, not that they
aren't in it? Of course we aren't, and that's because the `end` iterator
(as given by `this->combatants.end()`) does not point to the last
element in a data structure, it actually points to the element after the
last one. This allows iterators to contain existence information without
any extra complications. See, they're nice and useful when you get used
to them!

Now for the big one, the `nextTurn` function.

```cpp
/* battle.cpp */
void Battle::nextTurn()
{
	// Queue of battle events. Fastest combatants will be
	// at the start of the queue, and so will go first,
	// whereas slower ones will be at the back
	std::queue<BattleEvent> events;

	// Sort the combatants in agility order
	std::sort(combatants.begin(), combatants.end(), [](Creature* a, Creature* b) { return a->agility > b->agility; });

	// Iterate over the combatants and decide what they should do,
	// before adding the action to the event queue.
	for(auto com : this->combatants)
	{
		if(com->id == "player")
		{
			// Create the target selection dialogue
			Dialogue targetSelection = Dialogue("Who?", {});
			// Created every turn because some combatants may die
			for(auto target : this->combatants)
			{
				if(target->id != "player")
				{
					targetSelection.addChoice(target->name);
				}
			}

			// Ask the player for their action (attack or defend)
			int choice = this->battleOptions.activate();

			switch(choice)
			{
				default:
				case 1:
				{
					// Player is attacking, so ask for the target.
					// Dialogue returns the number of the choice but with
					// the player removed, so we have to do some fancy
					// arithmetic to find the actual location of the target
					// and then convert that to a pointer
					int position = targetSelection.activate();
					for(int i = 0; i < position; ++i)
					{
						if(this->combatants[i]->id == "player") ++position;
					}
					Creature* target = this->combatants[position-1];
					// Add the attack command to the event queue
					events.push(BattleEvent(com, target, BattleEventType::ATTACK));
					break;
				}
				case 2:
				{
					// Player is defending, so do nothing
					events.push(BattleEvent(com, nullptr, BattleEventType::DEFEND));
					break;
				}
			}
		}
		else
		{
			// Simple enemy AI where enemy constantly attacks player
			Creature* player = *std::find_if(this->combatants.begin(), this->combatants.end(),
				[](Creature* a) { return a->id == "player"; });

			events.push(BattleEvent(com, player, BattleEventType::ATTACK));
		}
	}
```

First we sort the combatants into agility order using the `std::sort`
function and another lambda. `std::sort` needs starting and ending
iterators like `std::find_if`, but its lambda takes two arguments and
must return `true` if the first is "less than" the second, whatever that
might mean. Does that mean less agility though? We're using an
`std::queue` to store our `BattleEvent`s, which means the events
processed first will be at the start of the queue and hence will occur
first. This means that the combatants with higher agility need to be
placed earlier in the combatants list, and so actually "less than" means
**greater** agility, not less.

Once the combatants have been sorted we iterate over them (in decreasing
agility order) and allow them to choose their actions. Standard
`Creature`s will just blindly attack the player (defending does nothing
anyway), but when it's the player's turn we need to ask them what they
want to do. After creating a `Dialogue` and populating it with all the
combatants who aren't the player, we ask the player what action they
want to take; attack or defend. Defending of course does nothing, but if
they attack we use the `Dialogue` to ask them which combatant they're
attacking.

Now we don't really want to let the player attack themselves, so we
omitted the player from the dialogue. But the player hasn't been removed
from the combatant list, so the number they entered into the `Dialogue`
is not necessarily the actual location of the combatant in the data
structure. To get around that, we use a `for` loop to scan the
combatants before the one chosen by the player and if the player is
encountered, we skip them by extending the length of the loop.

The second part of the function involves processing the events
themselves.

```cpp
	/* battle.cpp - nextTurn continued*/
	// Take each event from the queue in turn and process them,
	// displaying the results
	while(!events.empty())
	{
		// Take event from the front of the queue
		BattleEvent event = events.front();
		switch(event.type)
		{
			case BattleEventType::ATTACK:
			{
				// The event can't be run if either the source or the
				// target were slain previously in this turn, so we
				// must check that they're valid first
				auto a = this->combatants.begin();
				auto b = this->combatants.end();
				if(std::find(a, b, event.source) == b || std::find(a, b, event.target) == b)
				{
					break;
				}
				std::cout << event.source->name
					<< " attacks "
					<< event.target->name
					<< " for "
					<< event.run()
					<< " damage!\n";
				// Delete slain enemies
				if(event.target->hp <= 0)
				{
					this->kill(event.target);
				}
				break;
			}
			case BattleEventType::DEFEND:
				std::cout << event.source->name
					<< " defends!\n";
				break;
			default:
				break;
		}
		events.pop();
	}
}
```

We made the `BattleEvent` class so we didn't have to deal with
processing the events inside the `nextTurn` function, but we still need
to tell the player what happened and deal with combatant deaths. Instead
of iterating over the events (we can't do that because they're in an
`std::queue`) we repeatedly take the event from the front of the
`std::queue`, deal with it, then delete it. Dealing with defend actions
is easy, to deal with attack actions we first check that the `source`
and `target` of the event still exist and that neither was killed
earlier on. `std::find` returns an iterator to element that matches its
third argument, and remember that it will return an ending iterator if
the element wasn't found. If both still exist then we run the event and
print how much damage was dealt before cleaning up all the slain enemies
using the `kill` function.

```cpp
/* battle.cpp */
void Battle::kill(Creature* creature)
{
	// Find the creature's position in the combatants vector
	auto pos = std::find(this->combatants.begin(), this->combatants.end(), creature);

	// Don't try and delete the creature if it doesn't exist
	if(pos != this->combatants.end())
	{
		std::cout << creature->name << " is slain!\n";

		// Health == 0 is used in main as a condition to check if the creature is
		// dead, but this function could be called when the creature is not killed
		// by reducing their health to zero (by a death spell, for example), so we
		// ensure the creature's health is 0 and is marked as dead
		creature->hp = 0;
		// Remove the creature
		this->combatants.erase(pos);
	}

	return;
}
```

Nothing too complicated here, we use the `std::find` function to get an
iterator to the creature we want to delete and then use the `erase`
member function (part of `std::vector`) to delete the creature after
setting its health to zero. Unfortunately `erase` requires an iterator
and not a pointer, which is why we have to do the conversion using
`std::find`.

Almost there! All that's left now is to incorporate this system into
`main`, and we'll be done! Back to `main.cpp` then, add this directly
before we check to see if the player's dead or not.

```cpp
		/* main.cpp */
		// If the area has any creatures in it, start a battle with them
		if(areaPtr->creatures.size() > 0)
		{
			// Create a vector of pointers to the creatures in the area
			std::vector<Creature*> combatants;
			std::cout << "You are attacked by ";
			for(int i = 0; i < areaPtr->creatures.size(); ++i)
			{
				Creature* c = &(areaPtr->creatures[i]);
				combatants.push_back(c);
				std::cout << c->name << (i == areaPtr->creatures.size()-1 ? "!\n" : ", ");
			}
			// Add the player to the combatant vector
			combatants.push_back(&player);
			// Run the battle
			Battle battle(combatants);
			battle.run();

			// If the player is still alive, grant them some experience, assuming
			// that every creature was killed
			if(player.hp > 0)
			{
				// Or use std::accumulate, but that requires an additional header
				unsigned int xp = 0;
				for(auto creature : areaPtr->creatures) xp += creature.xp;
				std::cout << "You gained " << xp << " experience!\n";
				player.xp += xp;
				// Remove the creatures from the area
				areaPtr->creatures.clear();
				// Restart the loop to force a save, then the game will carry on
				// as usual
				continue;
			}
			// Otherwise player is dead, so end the program
			else
			{
				std::cout << "\t----YOU DIED----\n    Game Over\n";
				return 0;
			}
		}
```

After checking that there are creatures to fight, we create an
`std::vector` containing pointers to each of the creatures in the `Area`
(which we already know are unique). We also tell the player that they've
started a battle by printing the names of each the creatures (note that
this is before they've been assigned a unique name. A better system than
the one here would be to count the totals of each and then say "`3 Rats,
2 Goblins, and a Bat`" for example.) and then separating them with
commas. We then add the player into the combatants list, and create the
`Battle`. Since our system has no way to flee, we can safely assume that
once the battle is over, either all the creatures are dead and so they
can be removed from the `Area`, or the player is dead in which case it
doesn't matter because, well, the player is dead!

OK, give it a compile and run, fingers crossed you get something akin to
the absolutely riveting game shown below.

```bash
> $ ./rpg
What's your name?
Gentoo
You are in room 1
1: Go through the sturdy wooden door
2: Search
2

You find:
Nothing
You are in room 1
1: Go through the sturdy wooden door
2: Search
0
Menu
====
1: Items
2: Equipment
3: Character
3
Character
=========
Gentoo the Rogue
Health:   15 / 15
Strength: 4
Agility:  5
Level:    1 (0 / 1)
----------------
You are in room 1
1: Go through the sturdy wooden door
2: Search
0
Menu
====
1: Items
2: Equipment
3: Character
2
Equipment
=========
Armor: Nothing
Weapon: Nothing

1: Equip Armor
2: Equip Weapon
3: Close
2
1: Dagger (1) - A small blade, probably made of iron. Keep the sharp end away from your body.
Equip which item?
1
----------------
You are in room 1
1: Go through the sturdy wooden door
2: Search
1
You go through the sturdy wooden door.
You are attacked by Rat!
What will you do?
1: Attack
2: Defend
1
Who?
1: Rat
1
Gentoo attacks Rat for 2 damage!
Rat attacks Gentoo for 0 damage!
What will you do?
1: Attack
2: Defend
1
Who?
1: Rat
1
Gentoo attacks Rat for 2 damage!
Rat is slain!
You are in room 2
1: Go through the sturdy wooden door
2: Search
```

Did it work? Hopefully it did and you now have a fully
functioning---albeit simple---RPG game. If you do, keep going, you're
not done! There's a whole bunch of stuff for you to implement next, such
as adding more areas/enemies/items, improving the battle system
(fleeing, magic, others fighting with the player, better AI), making the
area system more powerful (currently only doors are possible without
editing the source code, how about switches, NPCs, shops, or even a
scripting system using something like [Lua](http://www.lua.org/) or
[Ruby](https://www.ruby-lang.org/)?), adding more depth to the levelling
system (more classes and attributes, let the player allocate points into
their attributes instead of doing it automatically), or even adding
graphics (2D with [SFML](http://sfml-dev.org/) or
[SDL](https://www.libsdl.org/), 3D with
[OpenGL](https://www.opengl.org/)) or multiplayer!

I hope you've enjoyed the tutorial!

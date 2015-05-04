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

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <utility>
#include <cstdlib>
#include <ctime>
#include <unordered_set>
#include "JsonBox.h"

#include "entity_manager.hpp"
#include "item.hpp"
#include "weapon.hpp"
#include "armour.hpp"
#include "inventory.hpp"
#include "creature.hpp"
#include "dialogue.hpp"
#include "area.hpp"
#include "battle.hpp"

// New character menu
Creature startGame();

// Character information menu, displays the items the player has, their
// current stats etc.
void dialogueMenu(Creature& player);

// Keeps track of items, weapons, creatures etc.
EntityManager entityManager;

// Areas that the player has been to
std::unordered_set<std::string> visitedAreas;

int main(void)
{
	Creature player;

	// Load the entities
	entityManager.loadJson<Item>("items.json");
	entityManager.loadJson<Weapon>("weapons.json");
	entityManager.loadJson<Armour>("armour.json");
	entityManager.loadJson<Creature>("creatures.json");
	entityManager.loadJson<Area>("areas.json");

	// Seed the random number generator with the system time, so the
	// random numbers produced by rand() will be different each time
	srand(time(NULL));

	player = startGame();

	// Set the current area to be the first area in the atlas, essentially
	// placing the player there upon game start
	Area* currentArea = entityManager.getEntity<Area>("area_01");

	// Play the game until a function breaks the loop and closes it
	while(1)
	{
		// Mark the current player as visited
		visitedAreas.insert(currentArea->id);

		// Autosave the game
		player.save();
		JsonBox::Object o;
		for(auto area : visitedAreas)
		{
			o[area] = entityManager.getEntity<Area>(area)->getJson();
		}
		JsonBox::Value v(o);
		v.writeToFile(player.name + "_areas.json");

		// If the player has died then inform them as such and close
		// the program
		if(player.health <= 0)
		{
			std::cout << "\t----YOU DIED----\n    Game Over\n";
			return 0;
		}

		// If the area the player is in has any creatures inside it,
		// then begin a battle with the last creature in the list
		// before moving on the next one. This makes the creature
		// list act like a stack
		if(currentArea->creatures.size() > 0)
		{
		    for(int i = currentArea->creatures.size() - 1; i >= 0; --i)
		    {
			    Battle(&player, currentArea->creatures[i]).run();
			    // Remove the creature from the area. This is fine to do
			    // because if the player wins the creature will not respawn,
			    // and if the creature wins the player isn't around to see it
			    // (This does break the 'non-mutable' feature of the atlases,
			    // but doing so saves a lot of memory, as we don't need to keep
			    // two versions of each area)
			    currentArea->creatures.pop_back();
		    }
        }

		// Activate the current area's dialogue
		int result = currentArea->dialogue.activate();

		// These could be moved inside of the area code using an event
		// style system, but that allows for much less flexibility with
		// what happens in each area. Since we're defining the areas in
		// code anyway, sticking with this isn't too much of a problem,
		// and it keeps things easy to understand
		if(result == 0)
		{
			// Open the menu
			dialogueMenu(player);
			continue;
		}
		if(currentArea->id == "area_01")
		{
			switch(result)
			{
				// Move to area 1
				case 1:
					currentArea = entityManager.getEntity<Area>("area_02");
					break;
				// Search the area
				case 2:
					currentArea->search(player);
					break;
				default:
					break;
			}
		}
		else if(currentArea->id == "area_02")
		{
			switch(result)
			{
				// Move to area 0
				case 1:
					currentArea = entityManager.getEntity<Area>("area_01");
					break;
				// Search the area
				case 2:
					currentArea->search(player);
					break;
				default:
					break;
			}
		}
	}

	return 0;
}

// Create a new character or load an existing one
Creature startGame()
{
	// Ask for a name and class
	// Name does not use a dialogue since dialogues only request options,
	// not string input. Could be generalised into its own TextInput
	// class, but not really necessary
	std::cout << "What's your name?" << std::endl;
	std::string name;
	std::cin >> name;

	// Check for existence then open using JsonBox if it exists
	std::ifstream f((name + ".json").c_str());
	if(f.good())
	{
		f.close();
		// Load the player
		JsonBox::Value v;
		v.loadFromFile(name + ".json");
		Creature player = Creature("player", v, &entityManager);

		// Load the area
		v.loadFromFile(name + "_areas.json");
		JsonBox::Object o = v.getObject();
		for(auto area : o)
		{
			std::string key = area.first;
			entityManager.getEntity<Area>(key)->load(key, area.second, &entityManager);
			visitedAreas.insert(key);
		}

		// Return the player
		return player;
	}
	else
	{
		f.close();
		int result = Dialogue(
			"Choose your class",
			{"Fighter", "Rogue"}).activate();

		switch(result)
		{
			// Fighter class favours health and strength
			case 1:
				return Creature("player", name, 35, 20, 10, 5, 10.0, 1, "Fighter");

			// Rogue class favours dexterity and hit rate
			case 2:
				return Creature("player", name, 30, 5, 10, 20, 15.0, 1, "Fighter");

			// Default case that should never happen, but it's good to be safe
			default:
				return Creature("player", name, 30, 10, 10, 10, 10.0, 1, "Adventurer");
		}
	}
}

void dialogueMenu(Creature& player)
{
	// Output the menu
	int result = Dialogue(
		"Menu\n====",
		{"Items", "Equipment", "Character"}).activate();

	switch(result)
	{
		// Print the items that the player owns
		case 1:
			std::cout << "Items\n=====\n";
			player.inventory.print();
			std::cout << "----------------\n";
			break;
		// Print the equipment that the player is wearing (if they are
		// wearing anything) and then ask if they want to equip a weapon
		// or some armour
		case 2:
		{
			std::cout << "Equipment\n=========\n";
			std::cout << "Head: "
				<< (player.equippedArmour[Armour::Slot::HEAD] != nullptr ?
					player.equippedArmour[Armour::Slot::HEAD]->name : "Nothing")
				<< std::endl;
			std::cout << "Torso: "
				<< (player.equippedArmour[Armour::Slot::TORSO] != nullptr ?
					player.equippedArmour[Armour::Slot::TORSO]->name : "Nothing")
				<< std::endl;
			std::cout << "Legs: "
				<< (player.equippedArmour[Armour::Slot::LEGS] != nullptr ?
					player.equippedArmour[Armour::Slot::LEGS]->name : "Nothing")
				<< std::endl;
			std::cout << "Weapon: "
				<< (player.equippedWeapon != nullptr ?
					player.equippedWeapon->name : "Nothing")
				<< std::endl;

			int result2 = Dialogue(
				"",
				{"Equip Armour", "Equip Weapon", "Close"}).activate();

			// Equipping armour
			if(result2 == 1)
			{
				int userInput = 0;

				// Cannot equip armour if they do not have any
				// Print a list of the armour and retrieve the amount
				// of armour in one go
				int numItems = player.inventory.printArmour(true);
				if(numItems == 0) break;

				while(!userInput)
				{
					// Choose a piece of armour to equip
					std::cout << "Equip which item?" << std::endl;
					std::cin >> userInput;
					// Equipment is numbered but is stored in a list,
					// so the number must be converted into a list element
					if(userInput >= 1 && userInput <= numItems)
					{
						int i = 1;

						for(auto it : player.inventory.armour)
						{
							if(i++ == userInput)
							{
								// Equip the armour if it is found
								player.equipArmour(it.first);
								break;
							}
						}
					}
				}
			}
			// Equip a weapon, using the same algorithms as for armour
			else if(result2 == 2)
			{
				int userInput = 0;
				int numItems = player.inventory.printWeapons(true);

				if(numItems == 0) break;

				while(!userInput)
				{
					std::cout << "Equip which item?" << std::endl;
					std::cin >> userInput;
					if(userInput >= 1 && userInput <= numItems)
					{
						int i = 1;

						for(auto it : player.inventory.weapons)
						{
							if(i++ == userInput)
							{
								player.equipWeapon(it.first);
								break;
							}
						}
					}
				}
			}
			std::cout << "----------------\n";
			break;
		}
		// Output the character information, including name, class (if
		// they have one), stats, level, and experience
		case 3:
			std::cout << "Character\n=========\n";
			std::cout << player.name;
			if(player.className != "") std::cout << " the " << player.className;
			std::cout << std::endl;

			std::cout << "HP: " << player.health << " / " << player.maxHealth << std::endl;
			std::cout << "Str: " << player.str << std::endl;
			std::cout << "End: " << player.end << std::endl;
			std::cout << "Dex: " << player.dex << std::endl;
			std::cout << "Lvl: " << player.level << " (" << player.exp;
			std::cout <<  " / " << player.expToLevel(player.level+1) << ")" << std::endl;
			std::cout << "----------------\n";
			break;
		default:
			break;
	}

	return;
}

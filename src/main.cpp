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
#include "armor.hpp"
#include "inventory.hpp"
#include "creature.hpp"
#include "player.hpp"
#include "dialogue.hpp"
#include "area.hpp"

// New character menu
Player startGame();

// Character information menu, displays the items the player has, their
// current stats etc.
void dialogueMenu(Player& player);

// Keeps track of items, weapons, creatures etc.
EntityManager entityManager;

// Areas that the player has been to
std::unordered_set<std::string> visitedAreas;

int main(void)
{
	// Load the entities
	entityManager.loadJson<Item>("items.json");
	entityManager.loadJson<Weapon>("weapons.json");
	entityManager.loadJson<Armor>("armor.json");
	entityManager.loadJson<Creature>("creatures.json");
	entityManager.loadJson<Door>("doors.json");
	entityManager.loadJson<Area>("areas.json");

	// Seed the random number generator with the system time, so the
	// random numbers produced by rand() will be different each time
	srand(time(NULL));

	Player player = startGame();

	// Set the current area to be the first area in the atlas,
	// placing the player there upon game start
	player.currentArea = "area_01";

	// Play the game until a function breaks the loop and closes it
	while(1)
	{
		// Mark the current player as visited
		visitedAreas.insert(player.currentArea);

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
		if(player.hp <= 0)
		{
			std::cout << "\t----YOU DIED----\n    Game Over\n";
			return 0;
		}

		// If the area the player is in has any creatures inside it,
		// then begin a battle with the last creature in the list
		// before moving on the next one. This makes the creature
		// list act like a stack
		// if(player.getAreaPtr(&entityManager)->creatures.size() > 0)
		// {
		// 	for(int i = player.getAreaPtr(&entityManager)->creatures.size() - 1; i >= 0; --i)
		// 	{
		// 		Battle(&player, player.getAreaPtr(&entityManager)->creatures[i]).run();
		// 		// Remove the creature from the area. This is fine to do
		// 		// because if the player wins the creature will not respawn,
		// 		// and if the creature wins the player isn't around to see it
		// 		// (This does break the 'non-mutable' feature of the atlases,
		// 		// but doing so saves a lot of memory, as we don't need to keep
		// 		// two versions of each area)
		// 		player.getAreaPtr(&entityManager)->creatures.pop_back();
		// 	}
		// }

		// Add the search and movement options to the dialogue
		Dialogue roomOptions = player.getAreaPtr(&entityManager)->dialogue;
		for(auto door : player.getAreaPtr(&entityManager)->doors)
		{
			roomOptions.addChoice("Go through the " + door->description);
		}
		roomOptions.addChoice("Search");
		// Activate the current area's dialogue
		int result = roomOptions.activate();

		if(result == 0)
		{
			dialogueMenu(player);
		}
		else if(result <= player.getAreaPtr(&entityManager)->dialogue.size())
		{
			// Add more events here
		}
		else if(result < roomOptions.size())
		{
			Door* door = player.getAreaPtr(&entityManager)->doors.at(result-player.getAreaPtr(&entityManager)->dialogue.size()-1);
			int flag = player.traverse(door);

			switch(flag)
			{
				default:
				case 0:
					std::cout << "The " << door->description << " is locked." << std::endl;
					break;
				case 1:
					std::cout << "You unlock the " << door->description << " and go through it." << std::endl;
					break;
				case 2:
					std::cout << "You go through the " << door->description << "." << std::endl;
					break;
			}
		}
		else
		{
			player.getAreaPtr(&entityManager)->search(player);
		}
	}

	return 0;
}

// Create a new character or load an existing one
Player startGame()
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
		Player player = Player(v, &entityManager);

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
			// Fighter class favours strength
			case 1:
				return Player(name, 15, 5, 4, 1.0/64.0, 0, 1, "Fighter");

			// Rogue class favours agility
			case 2:
				return Player(name, 15, 4, 5, 1.0/64.0, 0, 1, "Rogue");

			// Default case that should never happen, but it's good to be safe
			default:
				return Player(name, 15, 4, 4, 1.0/64.0, 0, 1, "Adventurer");
		}
	}
}

void dialogueMenu(Player& player)
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
		// or some armor
		case 2:
		{
			std::cout << "Equipment\n=========\n";
			std::cout << "Armor: "
				<< (player.equippedArmor != nullptr ?
					player.equippedArmor->name : "Nothing")
				<< std::endl;
			std::cout << "Weapon: "
				<< (player.equippedWeapon != nullptr ?
					player.equippedWeapon->name : "Nothing")
				<< std::endl;

			int result2 = Dialogue(
				"",
				{"Equip Armor", "Equip Weapon", "Close"}).activate();

			// Equipping armor
			if(result2 == 1)
			{
				int userInput = 0;

				// Cannot equip armor if they do not have any
				// Print a list of the armor and retrieve the amount
				// of armor in one go
				int numItems = player.inventory.printArmor(true);
				if(numItems == 0) break;

				while(!userInput)
				{
					// Choose a piece of armor to equip
					std::cout << "Equip which item?" << std::endl;
					std::cin >> userInput;
					// Equipment is numbered but is stored in a list,
					// so the number must be converted into a list element
					if(userInput >= 1 && userInput <= numItems)
					{
						int i = 1;

						for(auto it : player.inventory.armor)
						{
							if(i++ == userInput)
							{
								// Equip the armor if it is found
								player.equipArmor(it.first);
								break;
							}
						}
					}
				}
			}
			// Equip a weapon, using the same algorithms as for armor
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

			std::cout << "Health:   " << player.hp << " / " << player.maxHp << std::endl;
			std::cout << "Strength: " << player.strength << std::endl;
			std::cout << "Agility:  " << player.agility << std::endl;
			std::cout << "Level:    " << player.level << " (" << player.xp;
			std::cout <<  " / " << player.xpToLevel(player.level+1) << ")" << std::endl;
			std::cout << "----------------\n";
			break;
		default:
			break;
	}

	return;
}
# Tying It Together

Finally, we are finished with the individual components of the game (at
least up to the complexity suitable for this tutorial), and can move on
and actually create something playable! Now then, most of this section
will deal with integrating all the classes we've built into `main.cpp`.

After the game has started and the preliminary setup is complete
(entities loaded and prng seeded), the player needs to be able to select
their save file, or create one if they haven't played before. The
`startGame` function will do this for us by first asking for the name of
the player's character. It will then attempt to open a JSON file with
that name (remember each `Player` is saved to a JSON file named as they
are). If it can't open the file it will ask the player for their class
and will create a new `Player`, otherwise it will load the existing one
from the file.

```cpp
/* main.cpp */
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
		JsonBox::Value saveData;
		JsonBox::Value areaData;
		saveData.loadFromFile(name + ".json");
		areaData.loadFromFile(name + "_areas.json");
		Player player = Player(saveData, areaData, &entityManager);

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
```

We've made use of a few more headers here---specifically `iostream`,
`fstream` (for `std::ifstream` which we use to test the existence of the
JSON file before opening it properly with JsonBox), `"dialogue.hpp"`,
and `"player.hpp"`---so make sure to include them and also declare a
prototype for the `startGame` function. A very simple

```cpp
/* main.cpp */
Player player = startGame();
```

in `main` and the player can create their character! Now we can create
the *game loop*, which is where all the actual gameplay will take place.
All game loops share a very similar structure; they read input from the
user, process the input, update the player and environment accordingly,
relay those changes back to the user, and then start again. The amount
of time spent in each stage depends on the game (a chess game will
barely use the first stage but will dedicate a lot of time to the third,
whereas an FPS would spend a lot of time in the last stage), and here we
care mostly about understanding and processing the user's input.

Here is `main` in its entirety.

```cpp
/* main.cpp */
int main()
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
		player.visitedAreas.insert(player.currentArea);

		// Pointer to to the current area for convenience
		Area* areaPtr = player.getAreaPtr(&entityManager);

		// Autosave the game
		player.save(&entityManager);

		// If the player has died then inform them as such and close
		// the program
		if(player.hp <= 0)
		{
			std::cout << "\t----YOU DIED----\n    Game Over\n";
			return 0;
		}

		// Add the search and movement options to the dialogue
		Dialogue roomOptions = areaPtr->dialogue;
		for(auto door : areaPtr->doors)
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
		else if(result <= areaPtr->dialogue.size())
		{
			// Add more events here
		}
		else if(result < roomOptions.size())
		{
			Door* door = areaPtr->doors.at(result-areaPtr->dialogue.size()-1);
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
			std::cout << "You find:" << std::endl;
			areaPtr->items.print();
			player.inventory.merge(&(areaPtr->items));
			areaPtr->items.clear();
		}
	}

	return 0;
}
```

After creating (or loading) the player we position them in the starting
area, and start the game loop. Firstly we add the `Area` the player is
in to their list of visited areas so that any changes they make to the
area will be recorded in their save file. Instead of implementing a save
menu or save location mechanic, we just automatically save the game on
the next line. We don't want the game continuing if the player has run
out of hit points, so next we check that they're still alive and end the
program if they aren't. Now that we know they're alive, they can start
to move their character around the world.

Each iteration of the game loop will be another action that the player
can take; attack an enemy, go through a door, and so on. We therefore
present to them the `Dialogue` of the `Area` that they're currently in,
but make sure to add options for all the `Door`s that are in the `Area`,
because the `Area` class didn't handle that itself. We also add an
option to search the `Area`, which will move any items in the `Area`'s
inventory into the player's. That's the first data presentation stage
done (ours is split into a before and after stages), now we read the
input from the user.

Thanks to the wonders of the `Dialogue` class (it's so small but it's so
useful!), parsing the user's input is extremely easy. And what's more,
because `Area` didn't add the `Door`s to its dialogue we automatically
know how many non-door options the `Area` has, as well as how many door
options. This makes determining the type of action that the user is
performing very simple. (Although we still have to leave it up to the
`Area` to deal with non-door and non-search actions.)

If the player chose to go through a door, we use the `traverse` member
function to move them to the next area (if possible) and report back how
their traversal attempt went. If they tried searching, we print a list
of all the items in the `Area` and move them to the player's inventory.
Finally, note that we've allowed the program to understand an input of
0, even though that wouldn't be an option on the `Dialogue`. We briefly
mentioned this in a previous section, but now we're putting it into
practise by calling a `dialogueMenu` function which will open a menu
displaying some useful information about the player. Before we define
that, try commenting it out and running the program. Hopefully you'll
have a nice little RPG game!

```bash
> $ ./rpg
What's your name?
Gentoo
Choose your class
1: Fighter
2: Rogue
2
You are in room 1
1: Go through the sturdy wooden door
2: Search
2
You find:
Gold Coin (5) - A small disc made of lustrous metal
Iron Key (1) - A heavy iron key with a simple cut
Dagger (1) - A small blade, probably made of iron. Keep the sharp end away from your body.
Leather Armor (1) - Armor made from tanned animal hide. It smells a bit funny, but it'd do the job.
You are in room 1
1: Go through the sturdy wooden door
2: Search
1
You unlock the sturdy wooden door and go through it.
You are in room 2
1: Go through the sturdy wooden door
2: Search
```

Did it work? If it still didn't work after fixing compilation errors,
compare yours to the
[source](https://github.com/Piepenguin1995/cpp-rpg-tutorial). We're not
done yet though, there's still that menu to create, for starters. It's a
bit of a long function, but it's really quite simple.

```cpp
/* main.cpp */
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
				int numItems = player.inventory.print<Armor>(true);
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
						player.equipArmor(player.inventory.get<Armor>(userInput-1));
					}
				}
			}
			// Equip a weapon, using the same algorithms as for armor
			else if(result2 == 2)
			{
				int userInput = 0;
				int numItems = player.inventory.print<Weapon>(true);

				if(numItems == 0) break;

				while(!userInput)
				{
					std::cout << "Equip which item?" << std::endl;
					std::cin >> userInput;
					if(userInput >= 1 && userInput <= numItems)
					{
						player.equipWeapon(player.inventory.get<Weapon>(userInput-1));
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
```

The menu has three separate submenus to it, accessed via a `Dialogue`;
`"Items"`, `"Equipment"`, and `"Character"`. The first simply prints a
list of all the items that the player. The second is more involved, and
as well as printing all the armour and weapons that the player has
equipped, it also asks the player what armor or weapon they would like
to equip. Entering 0 again will cancel the action, and take them out of
the menu. The third and final submenu prints a nicely formatted
information page about the player character, listing their various
attributes and the amount required to level up.

It's at this point we really have nothing more than a walking simulator,
to be honest it was a bit of a stretch to call this an RPG. For that, we
need the battle system!

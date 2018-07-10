# Introduction

In this tutorial we will be creating a text-based RPG game using C++. It will be a classic dungeon-crawl game, where the player explores a vast network of rooms while fighting monsters and gathering loot.

We will be using the C++11 standard quite heavily---so please ensure you have a complier that supports it---as well as making use of the Standard Template Library and templates in general. If you aren't experienced with these don't worry, I'll explain them the first time they're used.

The code for this tutorial is available on [Github](https://github.com/Piepenguin1995/cpp-rpg-tutorial) and is licensed according to the MIT license so you're free to do pretty much whatever you want with it, though it's mostly there for use as a reference.

### JsonBox ###

We'll also be making use of JSON files in order to store the data used by the game; these aren't especially complicated to understand or manipulate, but you will have to install the [JsonBox](https://github.com/anhero/JsonBox) library, for which you'll need [CMake](http://www.cmake.org/).

On *nix systems installation is simple, just download the JsonBox code and run

```bash
cmake . && make
```

then copy the `include` folder into the same folder as where you'll put your source code files and copy `libJsonBox.a` into wherever your put your library files. Then when you compile, make sure to link `libJsonBox.a` and include the headers. This'll depend on your compiler.

For Windows refer to [this tutorial](http://www.cmake.org/runningcmake/), but essentially you run `cmake` and configure it to generate build files for your chosen IDE (i.e. Visual Studio), then use those to build a `.dll` file for JsonBox.

#### Without Building the Library ####

If you run into trouble installing the library then you can include it directly by downloading JsonBox and copying both the `include` and `src` folders into your project, then replacing all instances of `#include <JsonBox/something.h>` with `#include "JsonBox/something.h"` so the compiler knows where to find the header files. Your project layout should then look like

```bash
project/
  |-> src/
    |-> Array.cpp
    |-> other-jsonbox-files
    |-> main.cpp
    |-> other-tutorial-files
    |-> JsonBox/
      |-> Array.h
      |-> other-jsonbox-headers
```

Now you can compile the project as usual, and you won't have to link the library (although compilation times will be much higher).

### Overarching Design ###

Before we get into writing any code, the first thing to do is consider how the game's code will be structured. For small projects, coding as you go along can be a viable strategy but for more complicated programs, especially games, taking a step back before diving in is crucial.

The quick and dirty way would probably involve creating a bunch of disjoint classes, say one for the player, one for the enemies, one for the items, and so on, and then bundling them all together in a big game loop state machine which controlled the gameplay itself. Whilst this might work (and actually we're going to do something similar) it isn't likely to turn out too nicely, especially that big loop.

Ideally we'd decouple the game from the game engine, so that whilst the mechanics of the game are all handled by the program, they're handled in such an way as to make adding an additional creature or area of the game world require very little work on the part of the programmer. This is the approach we'll be taking, by defining things as abstractly as possible in the code, and then implementing the game itself in JSON files, which the program will load and read at runtime. This has the added bonus of not having to recompile the code every time you want to make a small change to the game.

So that we know what we're working towards, let's examine a small JSON file.

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

The file begins with `{`, which defines the start of a new JSON object. This object then contains a series of *keys* (in this case only one) written as a string, which have associated *values*. To denote a key, we put a `:` at the end of a string, and then start a new JSON value. A JSON value can be any of the standard data types in C++ as well as a string (but not a key), an array, or even another JSON object. Arrays are much more flexible in JSON than in C++, as they can contain any combination of values, as well as any type of value besides a JSON object. For example, the following is a perfectly valid array.

```json
[
	["foo", 7],
	["bar", 3.5, "true"],
	8
]
```

We will call anything defined by data in a JSON file an *entity*, and all such entities will be derived from a base `Entity` class which will establish their common functionality. We will use an `EntityManager` class to handle the individual entities, including loading them from their JSON files and giving the rest of the program access to them.

Using this entity system we will create `Item`, `Weapon`, `Armor`, `Creature`, `Area`, `Door`, and `Player` entities. Items, weapons, and armor will be collectable and useable by the player, and will increase their effectiveness in battle against different creatures as they progress through the dungeon that makes up the game world. This dungeon will be constructed from various different areas---which are like physical rooms but a bit more general---each of which will contain creatures to fight, items to collect, and doors to step through into the next area.

### Formatting ###

Finally a word about the formatting used in this tutorial and across the rest of the site. Any code will be placed inside code boxes like the JSON snippets are above. If that code forms an entire file and is not a snippet, it will have line numbers, otherwise the line will be unnumbered. Additionally, all code blocks containing code for you to type will have a comment at the top telling you which file they should go in, with the exception of JSON files. (Official JSON does not support comments, and it's pretty clear from context anyway.)

```cpp
/* main.cpp */
// This is the entire file main.cpp
int main()
{
	int foo = 10;
	return foo;
}
```

```cpp
/* main.cpp */
// This is a snippet from main.cpp
int foo = 10;
return foo;
```

When it comes to describing code in text instead of in boxes, I will refer to all C++ keywords, types, classes, objects, and so on, in `code formatting`. For example, later in this tutorial we'll be using an area system made of `Area`s to move the player character of type `Player`. I'll be a little loose with this, and may occasionally call the player the `Player`, but generally when referring to the code version of a term I'll use code formatting and will not otherwise. Important terms will also be *italicised* the first time they are used; I will rarely---if ever---use italics for emphasis.

Oh, and whilst the code boxes are set up to allow you to copy and paste, **please** don't do that and instead type it out yourself.  It's amazing how much simply writing things out improves your understanding of them! If instead you actually want a completed game that you don't understand, just download the source code and be done with it. Anyway with that over, let's begin!

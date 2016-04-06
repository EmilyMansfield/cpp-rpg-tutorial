# cpp-rpg-tutorial

Source code for my [C++ RPG tutorial](http://www.penguinprogrammer.co.uk/rpg-tutorial-2). The master branch is
(i.e. should be) up to date with the tutorial itself, and the other branches are for development and new additions.
Code is distributed according to the MIT license, so basically do what you want as long as you preserve the copyright
notices and give credit, other than that feel free to fork and use!

This project uses JsonBox for JSON manipulation, which the license file `JsonBox-license.txt` applies to. JsonBox should
be installed separately.

## Building the code

The following script will clone the required repositories and build the project's source on *nix systems.
You'll need to have `git`, `clang` and `cmake` installed for the script to work

```bash
# Clone jukebox & this tutorial
git clone https://github.com/anhero/JsonBox.git
git clone https://github.com/Piepenguin1995/cpp-rpg-tutorial.git

# Build JsonBox
cd JsonBox
cmake . && make

# Copy the include files and the library file to the tutorial folder
cd ..
cp -r JsonBox/include cpp-rpg-tutorial
cp JsonBox/Export.h cpp-rpg-tutorial/include
cp JsonBox/libJsonBox.a cpp-rpg-tutorial

# Build the source using clang
cd cpp-rpg-tutorial/src
clang++ -std=c++11 main.cpp area.cpp armor.cpp battle.cpp creature.cpp door.cpp entity_manager.cpp inventory.cpp item.cpp player.cpp weapon.cpp ../libJsonBox.a -I ../include/ -rpath ../ -o ../rpg.out

# Run the game
cd ..
./rpg.out
```

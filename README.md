# ardutainment

An entertainment system for Arduinos.

## What's included

* Jumping game - a game where you try to avoid obstacles by jumping
* Starship - navigate a space ship through asteroids and enemy space ships while firing lasers
* Fruit capture - capture the fruit and destroy the rocks
* Music - 2 classic tunes (Beethoven's "Ode to Joy" and Mozart's "Eine Kleine Nachtmusik") are also included for your enjoyment

## How to use

The hardware that you will require:

* An Arduino (only tested on UNO rev 3)
* 3 push buttons, with a pull down resistor for each one
* A 16x2 LCD compatible with the LiquidCrystal library
* A piezo (optional)
* A bunch of jumper wires to put everything together (you might also need some resistors for the LCD and piezo)

This hardware is included with the Arduino starter kit, so many Arduino users will already have it.

The ardutainment system is not quite plug-and-play. You'll need to modify the pin numbers in the file pins.h to correspond to your physical setup (unless if you use the exact same one as me). The pins you will need are:

* An RS pin for the LCD
* An EN pin for the LCD
* 4 data pins for the LCD
* A pin for the piezo
* A pin for each button (3 buttons are required), which is attached to a pull-down resistor (that is, 0 if not pressed)
* An analog pin that is not connected to anything (this is used for random number generation)

Afterwards, you should can compile and upload from the Arduino IDE!


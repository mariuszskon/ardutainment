#ifndef SHARED_H
#define SHARED_H
#include <LiquidCrystal.h>

/* == PINS == */
// LCD
#define RS_PIN 7
#define EN_PIN 8
#define D4_PIN 9
#define D5_PIN 10
#define D6_PIN 11
#define D7_PIN 12

// PIEZO
#define PIEZO_PIN 6

// BUTTONS
#define BUTTON0_PIN 2
#define BUTTON1_PIN 3
#define BUTTON2_PIN 4

// UNCONNECTED ANALOG
#define UNCONNECTED_ANALOG A0

/* == END PINS == */

/* == NOTES == */

#define NOTE_C  262
#define NOTE_CS 277
#define NOTE_D  294
#define NOTE_DS 311
#define NOTE_E  330
#define NOTE_F  349
#define NOTE_FS 370
#define NOTE_G  392
#define NOTE_GS 415
#define NOTE_A  440
#define NOTE_AS 466
#define NOTE_B  494
#define REST      0

/* == END NOTES == */

/* == MISC == */

#define LCD_FULL_BLOCK 255

/* == END MISC */

/* == MACRO FUNCTIONS == */

#define UNUSED(var) (void)var

/* == END MACRO FUNCTIONS == */

LiquidCrystal lcd(RS_PIN, EN_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN);

#endif

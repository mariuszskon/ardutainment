/*
 * ARDUTAINMENT SYSTEM VERSION 1
 */

#include "shared.h"
#include "fruit_capture.h" // this needs to be done here due to the "helpful" features of the Arduino IDE

#define SELECTION_OPTIONS 4

void setup() {
    pinMode(PIEZO_PIN, OUTPUT);
    pinMode(BUTTON0_PIN, INPUT);
    pinMode(BUTTON1_PIN, INPUT);
    pinMode(BUTTON2_PIN, INPUT);

    srand(analogRead(UNCONNECTED_ANALOG));

    lcd.begin(16, 2);
    lcd.print("ARDUTAINMENT v1");
    lcd.setCursor(0, 1);
    lcd.print("By Mariusz S");
    int intro_notes[5] = {NOTE_C, NOTE_D, NOTE_E, NOTE_E, NOTE_E};
    int intro_times[5] = {125,    125,    500,    500,    500};
    playMusic(intro_notes, intro_times, 5);
    delay(1000);

    const char *options[SELECTION_OPTIONS] = {"JUMPING GAME", "STARSHIP", "FRUIT CAPTURE", "MUSIC"};
    void (*playable_functions[SELECTION_OPTIONS])() = {jg_play, star_play, fc_play, music_toy_play};

    int selection = select_screen(options, SELECTION_OPTIONS);

    (*playable_functions[selection])();
}

void loop() {
    
}


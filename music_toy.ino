const int MUSIC_TOY_UP = BUTTON0_PIN;
const int MUSIC_TOY_SELECT = BUTTON1_PIN;
const int MUSIC_TOY_DOWN = BUTTON2_PIN;

const int MUSIC_AVAILABLE = 2;

void music_ode_to_joy() {
    lcd.clear();
    lcd.print("BEETHOVEN");
    lcd.setCursor(0, 1);
    lcd.print("ODE TO JOY");

    int main_bit_notes[12] = {NOTE_E, NOTE_E, NOTE_F, NOTE_G, NOTE_G, NOTE_F, NOTE_E, NOTE_D, NOTE_C, NOTE_C, NOTE_D, NOTE_E};
    int main_bit_times[12] = {250,    250,    250,    250,    250 ,   250,    250,    250,    250,    250,    250,    250};

    int ending_times[3] = {375, 125, 500};

    int ending1_notes[3] = {NOTE_E, NOTE_D, NOTE_D};
    int ending2_notes[3] = {NOTE_D, NOTE_C, NOTE_C};

    int fancy_bit_notes[16] = {NOTE_D, NOTE_E, NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_E, NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_E, NOTE_D, NOTE_C, NOTE_D, NOTE_G};
    int fancy_bit_times[16] = {500,    250,    250,    250,    125,    125,    250,    250,    250,    125,    125,    250,    250,    250,    250,    500};
    
    playMusic(main_bit_notes, main_bit_times, 12);
    playMusic(ending1_notes, ending_times, 3);

    playMusic(main_bit_notes, main_bit_times, 12);
    playMusic(ending2_notes, ending_times, 3);

    playMusic(fancy_bit_notes, fancy_bit_times, 16);

    playMusic(main_bit_notes, main_bit_times, 12);
    playMusic(ending2_notes, ending_times, 3);
}

void music_nachtmusik() {
    lcd.clear();
    lcd.print("MOZART");
    lcd.setCursor(0, 1);
    lcd.print("NACHTMUSIK");

    int intro_pt1_notes[2] = {NOTE_C, NOTE_G / 2};
    int intro_pt1_times[2] = {750,    250};

    int intro_pt2_notes[5] = {NOTE_C, NOTE_G / 2, NOTE_C, NOTE_E, NOTE_G};
    int intro_pt2_times[5] = {250,    250,        250,    250,    1000};

    int intro_pt3_notes[2] = {NOTE_F, NOTE_D};
    int intro_pt3_times[2] = {750,    250};

    int intro_pt4_notes[5] = {NOTE_F, NOTE_D, NOTE_B / 2, NOTE_D, NOTE_G / 2};
    int intro_pt4_times[5] = {250,    250,    250,        250,    1000};

    int middle_bit_intro_notes[2] = {NOTE_C, NOTE_C};
    int middle_bit_intro_times[2] = {500, 500};

    int middle_bit_notes[11] = {REST, NOTE_E, NOTE_D, NOTE_C, NOTE_C, NOTE_B / 2, NOTE_B / 2, REST, NOTE_D, NOTE_E, NOTE_B / 2};
    int middle_bit_times[11] = {250,  250,    250,    250,    250,    250,        500,        250,  250,    250,    250};

    int middle_bit_break_notes[3] = {NOTE_D, NOTE_C, NOTE_C};
    int middle_bit_break_times[3] = {250,    250,    500};

    int fancy_bit_notes[19] = {NOTE_C, NOTE_C, NOTE_C, NOTE_B / 2, NOTE_A / 2, NOTE_B / 2, NOTE_C, NOTE_C, NOTE_E, NOTE_D, NOTE_C, NOTE_D, NOTE_E, NOTE_E, NOTE_G, NOTE_F, NOTE_E, NOTE_F, NOTE_G};
    int fancy_bit_times[19] = {250,    250,    125,    125,        125,        125,        250,    250,    125,    125,    125,    125,    250,    250,    125,    125,    125,    125,    1000};

    int calm_bit_notes[14] = {NOTE_G, NOTE_A, NOTE_G, NOTE_F, NOTE_F, NOTE_F, NOTE_F, NOTE_E, NOTE_E, NOTE_E, NOTE_E, NOTE_D, NOTE_D, NOTE_D};
    int calm_bit_times[14] = {1000,   1000,   250,    250,    250,    250,    250,    250,    250,    250,    250,    250,    250,    250};

    int ending_notes[7] = {NOTE_C, NOTE_B / 2, NOTE_A / 2, NOTE_B / 2, NOTE_C, NOTE_G / 2, NOTE_C};
    int ending_times[7] = {250,    250,        250,        250,        500,    500,        500};

    playMusic(intro_pt1_notes, intro_pt1_times, 2);
    playMusic(intro_pt1_notes, intro_pt1_times, 2);
    playMusic(intro_pt2_notes, intro_pt2_times, 5);

    playMusic(intro_pt3_notes, intro_pt3_times, 2);
    playMusic(intro_pt3_notes, intro_pt3_times, 2);
    playMusic(intro_pt4_notes, intro_pt4_times, 5);

    playMusic(middle_bit_intro_notes, middle_bit_intro_times, 2);
    playMusic(middle_bit_notes, middle_bit_times, 11);
    playMusic(middle_bit_break_notes, middle_bit_break_times, 3);
    playMusic(middle_bit_notes, middle_bit_times, 11);

    playMusic(fancy_bit_notes, fancy_bit_times, 19);

    playMusic(calm_bit_notes, calm_bit_times, 14);

    playMusic(ending_notes, ending_times, 7);
}

void music_toy_play() {
    lcd.clear();
    lcd.print("MUSIC TOY");
    int intro_notes[5] = {NOTE_C, NOTE_D, NOTE_E, NOTE_G, NOTE_A};
    int intro_times[5] = {250,    250,    250,    250,    250};
    playMusic(intro_notes, intro_times, 5);

    void (*music_players[MUSIC_AVAILABLE])() = {music_ode_to_joy, music_nachtmusik};
    const char *music_names[MUSIC_AVAILABLE] = {"ODE TO JOY", "NACHTMUSIK"};

    for (;;) {
        int selection = select_screen(music_names, MUSIC_AVAILABLE);
        (*music_players[selection])();
    }
}


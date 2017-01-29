#define JG_BUTTON BUTTON0_PIN

#define JG_INITIAL_TICK_TIME 500
#define JG_MAX_AIR_TIME 5
#define JG_REST_SPACES 4
#define JG_DIFFICULTY2_SCORE 100
#define JG_DIFFICULTY3_SCORE 500

void jg_draw(int score, int difficulty, bool blocks[], bool on_ground) {
    // draw stuff
    lcd.clear();

    lcd.setCursor(4, 0);
    for (int i = 0; i < difficulty; i++) {
        lcd.write('!');
    }

    lcd.setCursor(13, 0);
    lcd.print(score);

    // print the blocks
    lcd.setCursor(0, 1);
    for (int i = 0; i < 16; i++) {
        if (blocks[i]) {
            lcd.write(LCD_FULL_BLOCK);
        } else {
            lcd.write(' ');
        }
    }

    lcd.setCursor(2, on_ground); // bottom row if on ground (1), else top row (0)
    if (on_ground) {
        lcd.write(score % 2); // since score += 1 each tick, we can alternate the player frames (custom characters 0 and 1) easily
    } else {
        lcd.write((score % 2) + 2); // similar to above, but 2 or 3
    }
}

void jg_generate_blocks(int *spaces_to_fill, bool *block_mode, bool blocks[]) {
    if (*spaces_to_fill == 0) {
        *block_mode = !*block_mode;
        if (*block_mode == 1) { // 1 == blocks
            *spaces_to_fill = rand_between(1, JG_MAX_AIR_TIME);
        } else { // guarantee the player a reasonable break sometimes (0 == free spaces / non-blocks)
            *spaces_to_fill = JG_REST_SPACES;
        }
    }
    blocks[15] = *block_mode;
    *spaces_to_fill -= 1;
}

void jg_tick(int *score, int *on_ground, int *air_time, bool blocks[], int *spaces_to_fill, bool *block_mode, int *difficulty) {
    if (!*on_ground) *air_time -= 1;
    if (*air_time == 0) *on_ground = 1;

    // shift blocks left
    for (int i = 0; i < 15; i++) {
        blocks[i] = blocks[i + 1];
    }

    jg_generate_blocks(spaces_to_fill, block_mode, blocks);

    // check for collision
    if (blocks[2] == 1) { // if a block is horizontally where the player
        if (*on_ground) {
            jg_draw(*score, *difficulty, blocks, *on_ground); // redraw to move player down and move blocks where they should be
            lcd.setCursor(2, 1);
            lcd.write('*'); // "crash" character
            lcd.setCursor(4, 1);
            lcd.print("GAME OVER");

            int lose_notes[4] = {NOTE_G, NOTE_E, NOTE_D, NOTE_C};
            int lose_times[4] = {250,    300,    400,    1000};
            playMusic(lose_notes, lose_times, 4);

            while (1) {} // freeze
        }
    }

    if (*score == 999) {
        lcd.setCursor(4, 1);
        lcd.print("YOU WIN!");

        int win_notes[7] = {NOTE_C, NOTE_C, NOTE_D, NOTE_D, NOTE_G, NOTE_A, NOTE_B};
        int win_times[7] = {375,    125,    375,    125,    250,    250,    1000};
        while (1) { // freeze (while playing music)
            playMusic(win_notes, win_times, 7);
            delay(1000);
        }
    } else if (*score > JG_DIFFICULTY3_SCORE) {
        *difficulty = 3;
    } else if (*score > JG_DIFFICULTY2_SCORE) {
        *difficulty = 2;
    }

    *score += 1;
    jg_draw(*score, *difficulty, blocks, *on_ground);
}

bool jg_input(int *on_ground, int *air_time) {
    if (*on_ground && digitalRead(JG_BUTTON)) {
        // JUMP
        *on_ground = 0;
        *air_time = JG_MAX_AIR_TIME;
        tone(PIEZO_PIN, NOTE_C / 2, 100);
        return 1;
    } else {
        return 0;
    }
}

void jg_play() {
    int score = 0;

    unsigned int tick_time = JG_INITIAL_TICK_TIME;
    unsigned long prev_tick = millis();

    int on_ground = 1;
    int air_time = 0;
    bool blocks[16] = {0};
    int spaces_to_fill = 0;
    bool block_mode = 0; 
    int difficulty = 1;
    bool redraw = 0;

    uint8_t player_ground_frame0[8] = {
        B00000,
        B00100,
        B01010,
        B00101,
        B01110,
        B10100,
        B01011,
        B10000,
    };
    
    uint8_t player_ground_frame1[8] = {
        B00000,
        B00100,
        B01010,
        B10100,
        B01110,
        B00101,
        B00110,
        B01010,
    };
    
    uint8_t player_air_frame0[8] = {
        B00100,
        B01010,
        B10101,
        B01110,
        B00100,
        B11011,
        B00000,
        B00000,
    };
    
    uint8_t player_air_frame1[8] = {
        B00100,
        B01010,
        B00100,
        B01110,
        B10101,
        B01010,
        B10001,
        B00000,
    };

    lcd.createChar(0, player_ground_frame0);
    lcd.createChar(1, player_ground_frame1);
    lcd.createChar(2, player_air_frame0);
    lcd.createChar(3, player_air_frame1);
    lcd.clear();
    lcd.print("JUMPING GAME");
    int intro_notes[5] = {NOTE_C, NOTE_D, NOTE_E, NOTE_A, NOTE_AS};
    int intro_times[5] = {250,    250,    125,    125,    1000};
    playMusic(intro_notes, intro_times, 5);

    for (;;) {
        redraw = jg_input(&on_ground, &air_time);
        if (redraw) jg_draw(score, difficulty, blocks, on_ground);

        // delay without delay() - allows getting input at any time
        unsigned long current_time = millis();
        if (current_time - prev_tick >= tick_time) {
            jg_tick(&score, &on_ground, &air_time, blocks, &spaces_to_fill, &block_mode, &difficulty);
            prev_tick = millis();
            tick_time = JG_INITIAL_TICK_TIME / difficulty; // more difficult = faster
        }
    }
}

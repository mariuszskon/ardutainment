#define STARSHIP_SWITCH_LANE_BUTTON BUTTON0_PIN
#define STARSHIP_LASER_BUTTON BUTTON1_PIN

#define STARSHIP_INITIAL_TICK_TIME 350
#define STARSHIP_LANE_SWITCH_COOLDOWN 350
#define STARSHIP_LASER_COOLDOWN 3 * STARSHIP_INITIAL_TICK_TIME

#define STAR_WIN_SCORE 999

#define STARSHIP_LASER_SPEED 2

#define STARSHIP_MAX_ASTEROIDS 5
#define STARSHIP_MAX_LASERS 30
#define STARSHIP_MAX_EVIL_SHIPS 2

#define STARSHIP_PLAYER_CHAR_ID 0
#define STARSHIP_LASER_CHAR_ID 1
#define STARSHIP_ASTEROID_CHAR_ID 2
#define STARSHIP_EVIL_SHIP_CHAR_ID 3

#define STARSHIP_PLAYER_X 1
#define STARSHIP_LASER_LEFT 0
#define STARSHIP_LASER_RIGHT 1

struct StarshipFlyingThing {
    int x;
    int y;
    bool defined;
};

struct StarshipLaser {
    bool direction;
    int x;
    int y;
    bool defined;
};

struct StarshipGameState {
    int score;
    unsigned int tick_time;
    unsigned long prev_tick;
    bool gameover;
};

struct StarshipPlayerData {
    bool lane;
    unsigned long last_switched;
    unsigned long last_fired;
};

void star_draw(bool lane, struct StarshipFlyingThing asteroids[], struct StarshipLaser lasers[], struct StarshipFlyingThing evils[]) {
    // reduce commands sent to LCD by precomputing where things are
    int scene[16][2]; // col, row (x, y)
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 2; j++) {
            scene[i][j] = ' '; // set all to space at first as it looks blank on LCD screen
        }
    }
    
    // draw the asteroids
    for (int i = 0; i < STARSHIP_MAX_ASTEROIDS; i++) {
        if (asteroids[i].defined) {
            scene[asteroids[i].x][asteroids[i].y] = STARSHIP_ASTEROID_CHAR_ID;
        }
    }

    // draw the lasers
    for (int i = 0; i < STARSHIP_MAX_LASERS; i++) {
        if (lasers[i].defined) {
            scene[lasers[i].x][lasers[i].y] = STARSHIP_LASER_CHAR_ID;
        }
    }

    // draw the evil ships
    for (int i = 0; i < STARSHIP_MAX_EVIL_SHIPS; i++) {
        if (evils[i].defined) {
            scene[evils[i].x][evils[i].y] = STARSHIP_EVIL_SHIP_CHAR_ID;
        }
    }

    // draw the player
    scene[STARSHIP_PLAYER_X][lane] = STARSHIP_PLAYER_CHAR_ID;

    // actually write it to the LCD screen
    lcd.clear();
    bool reset_cursor = 0;
    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 16; col++) {
            if (scene[col][row] == ' ') {
                // drawing a blank space wastes LCD commands - skip it
                reset_cursor = 1;
                continue;
            }
            if (reset_cursor) {
                lcd.setCursor(col, row);
                reset_cursor = 0;
            }
            lcd.write(scene[col][row]);
        }
        reset_cursor = 1; // for the next row
    }
}

bool star_fire_laser(struct StarshipLaser lasers[], bool direction, int x, int y) {
    int free_space = -1;
    for (int i = 0; i < STARSHIP_MAX_LASERS; i++) {
        if (!lasers[i].defined) {
            free_space = i;
            break;
        }
    }

    if (free_space != -1) {
        tone(PIEZO_PIN, NOTE_A * 2, 125);
        lasers[free_space].direction = direction;
        lasers[free_space].x = x;
        lasers[free_space].y = y;
        lasers[free_space].defined = 1;

        return 1; // we fired successfully
    }

    return 0; // we did not fire successfully
}

bool star_input(struct StarshipPlayerData *player, struct StarshipLaser lasers[]) {
    bool redraw = 0;
    unsigned long current_time = millis();
    if (current_time - player->last_switched >= STARSHIP_LANE_SWITCH_COOLDOWN) {
        if (digitalRead(STARSHIP_SWITCH_LANE_BUTTON)) {
            player->lane = !player->lane;
            player->last_switched = current_time;
            redraw = 1;
        }
    }

    if (current_time - player->last_fired >= STARSHIP_LASER_COOLDOWN) {
        if (digitalRead(STARSHIP_LASER_BUTTON)) {
            player->last_fired = current_time;
            redraw = star_fire_laser(lasers, STARSHIP_LASER_RIGHT, STARSHIP_PLAYER_X + 1, player->lane);
        }
    }
    
    return redraw;
}

void star_draw_explosion(int x, int y) {
    lcd.setCursor(x, y);
    lcd.write('*');
}

void star_gameover(struct StarshipGameState *gamestate, struct StarshipPlayerData *player) {
    star_draw_explosion(STARSHIP_PLAYER_X, player->lane);
    lcd.setCursor(STARSHIP_PLAYER_X + 2, 0);
    lcd.print("GAME OVER");
    lcd.setCursor(STARSHIP_PLAYER_X + 2, 1);
    lcd.print("SCORE ");
    lcd.print(gamestate->score);
    int lose_notes[6] = {NOTE_G, REST, NOTE_E, NOTE_E, REST, NOTE_C};
    int lose_times[6] = {1000,   125,  125,    125,    125,  1000};
    playMusic(lose_notes, lose_times, 6);
    while (1) {}
}

void star_win() {
    lcd.setCursor(STARSHIP_PLAYER_X + 2, 0);
    lcd.print("YOU WIN!");
    lcd.setCursor(STARSHIP_PLAYER_X + 2, 1);
    lcd.print("SCORE ");
    lcd.print(STAR_WIN_SCORE);
    int win_notes[5] = {NOTE_C, NOTE_D, NOTE_E, NOTE_G, NOTE_A};
    int win_times[5] = {500,    125,    125,    125,    1875};
    playMusic(win_notes, win_times, 5);
    while (1) {}
}

void star_generate_asteroid(struct StarshipFlyingThing asteroids[]) {
    for (int i = 0; i < STARSHIP_MAX_ASTEROIDS; i++) {
        if (!asteroids[i].defined) {
            struct StarshipFlyingThing asteroid;
            asteroid.x = 15;
            asteroid.y = rand_between(0, 2);
            asteroid.defined = 1;
            asteroids[i] = asteroid;
            break;
        }
    }

}

void star_generate_evil_ship(struct StarshipFlyingThing evils[]) {
    for (int i = 0; i < STARSHIP_MAX_EVIL_SHIPS; i++) {
        if (!evils[i].defined) {
            struct StarshipFlyingThing evilship;
            evilship.x = 15;
            evilship.y = rand_between(0, 2);
            evilship.defined = 1;
            evils[i] = evilship;
            break;
        }
    }
}

void star_evil_ship_fire_laser(struct StarshipFlyingThing evils[], struct StarshipLaser lasers[]) {
    int id = rand_between(0, STARSHIP_MAX_EVIL_SHIPS);
    if (evils[id].defined) {
        star_fire_laser(lasers, STARSHIP_LASER_LEFT, evils[id].x - 1, evils[id].y);
    }
}

void advance_flying_things(struct StarshipFlyingThing asteroids[], struct StarshipFlyingThing evils[]) {
    // advance existing asteroids left
    for (int i = 0; i < STARSHIP_MAX_ASTEROIDS; i++) {
        if (asteroids[i].defined) {
            asteroids[i].x -= 1;
            if (asteroids[i].x < 0) {
                // asteroid flew out of screen, destroy it
                asteroids[i].defined = 0;
            }
        }
    }

    // advance evil ships left
    for (int i = 0; i < STARSHIP_MAX_EVIL_SHIPS; i++) {
        if (evils[i].defined) {
            evils[i].x -= 1;
            if (evils[i].x < 0) {
                evils[i].defined = 0;
            }
        }
    }
    
}

struct StarshipFlyingThing *select_at_position(struct StarshipFlyingThing *flying_things[], int x, int y) {
    struct StarshipFlyingThing *result = NULL;
    for (int i = 0; i < STARSHIP_MAX_ASTEROIDS + STARSHIP_MAX_EVIL_SHIPS; i++) {
        if (flying_things[i]->defined) {
            if (flying_things[i]->x == x) {
                if (flying_things[i]->y == y) {
                    result = flying_things[i];
                    break;
                }
            }
        }
    }
    return result;
}

bool star_flying_thing_laser_collision_detection(struct StarshipFlyingThing *flying_things[], struct StarshipLaser *laser) {
    struct StarshipFlyingThing *target;
    if (laser->defined) {
        target = select_at_position(flying_things, laser->x, laser->y);
        if (target != NULL) {
            laser->defined = 0;
            target->defined = 0;
            return 1;
        }
    }
    return 0;
}

bool star_player_laser_collision_detection(struct StarshipGameState *gamestate, struct StarshipPlayerData *player, struct StarshipLaser *laser) {
    if (STARSHIP_PLAYER_X == laser->x) {
        if (player->lane == laser-> y) {
            gamestate->gameover = 1;
            return 1;
        }
    }
    return 0;
}

bool star_laser_collision_detection(struct StarshipGameState *gamestate, struct StarshipPlayerData *player, struct StarshipFlyingThing *flying_things[], struct StarshipLaser *laser) {
    return star_flying_thing_laser_collision_detection(flying_things, laser) || star_player_laser_collision_detection(gamestate, player, laser);
}

void star_laser_physics(struct StarshipGameState *gamestate, struct StarshipPlayerData *player, struct StarshipLaser lasers[], struct StarshipFlyingThing *flying_things[]) {
    // advance existing lasers based on direction of fire
    for (int i = 0; i < STARSHIP_MAX_LASERS; i++) {
        if (lasers[i].defined) {
            if (star_laser_collision_detection(gamestate, player, flying_things, &lasers[i])) break; // check for a collision in the current location first
            for (int j = 0; j < STARSHIP_LASER_SPEED; j++) {
                if (lasers[i].direction == STARSHIP_LASER_RIGHT) {
                    lasers[i].x += 1;
                } else {
                    lasers[i].x -= 1;
                }
    
                if (lasers[i].x < 0 || lasers[i].x > 15) {
                    // laser flew out of screen, destroy it
                    lasers[i].defined = 0;
                } else {
                    if (star_laser_collision_detection(gamestate, player, flying_things, &lasers[i])) break;
                }
            }
        }
    }
}

void star_player_flying_thing_collision_detection(struct StarshipGameState *gamestate, struct StarshipPlayerData *player, struct StarshipFlyingThing *flying_things[]) {
    for (int i = 0; i < STARSHIP_MAX_ASTEROIDS + STARSHIP_MAX_EVIL_SHIPS; i++) {
        if (flying_things[i]->defined) {
            if (flying_things[i]->x == STARSHIP_PLAYER_X && flying_things[i]->y == player->lane) {
                // crashed!
                gamestate->gameover = 1;
            }
        }
    }
}

void star_tick(struct StarshipGameState *gamestate, struct StarshipPlayerData *player, struct StarshipFlyingThing asteroids[], struct StarshipLaser lasers[], struct StarshipFlyingThing evils[]) {
    struct StarshipFlyingThing *flying_things[STARSHIP_MAX_ASTEROIDS + STARSHIP_MAX_EVIL_SHIPS];
    for (int i = 0; i < STARSHIP_MAX_ASTEROIDS; i++) {
        flying_things[i] = &asteroids[i];
    }
    for (int i = 0; i < STARSHIP_MAX_EVIL_SHIPS; i++) {
        flying_things[i + STARSHIP_MAX_ASTEROIDS] = &evils[i];
    }

    star_laser_physics(gamestate, player, lasers, flying_things);

    advance_flying_things(asteroids, evils);

    if (rand_between(0, 2)) {
        star_generate_asteroid(asteroids);
    } else {
        if (rand_between(0, 5) == 0) {
            star_generate_evil_ship(evils);
        }
    }
    if (rand_between(0, 4) == 0) {
        star_evil_ship_fire_laser(evils, lasers);
    }

    
    star_player_flying_thing_collision_detection(gamestate, player, flying_things);
    
    gamestate->score += 1;
}

void star_play() {
    struct StarshipGameState gamestate;
    gamestate.score = 0;
    gamestate.tick_time = STARSHIP_INITIAL_TICK_TIME;
    gamestate.prev_tick = millis();
    gamestate.gameover = 0;

    struct StarshipPlayerData player;
    player.lane = 0;
    player.last_switched = 0;
    player.last_fired = 0;

    struct StarshipFlyingThing asteroids[STARSHIP_MAX_ASTEROIDS];
    for (int i = 0; i < STARSHIP_MAX_ASTEROIDS; i++) {
        asteroids[i].defined = 0;
    }

    struct StarshipLaser lasers[STARSHIP_MAX_LASERS];
    for (int i = 0; i < STARSHIP_MAX_LASERS; i++) {
        lasers[i].defined = 0;
    }

    struct StarshipFlyingThing evils[STARSHIP_MAX_EVIL_SHIPS];
    for (int i = 0; i < STARSHIP_MAX_EVIL_SHIPS; i++) {
        evils[i].defined = 0;
    }

    uint8_t player_starship_image[8] = {
        B00000,
        B11000,
        B01110,
        B00111,
        B00111,
        B01110,
        B11000,
        B00000
    };

    uint8_t laser_image[8] = {
        B00000,
        B00000,
        B00000,
        B11111,
        B11111,
        B00000,
        B00000,
        B00000
    };

    /*uint8_t asteroid_image[8] = {
        B00000,
        B11111,
        B01111,
        B11111,
        B11110,
        B01110,
        B11111,
        B00000
    };*/

    uint8_t asteroid_image[8] = {
        B00000,
        B00110,
        B01111,
        B11101,
        B10111,
        B11110,
        B01100,
        B00000
    };

    uint8_t evil_ship_image[8] = {
        B00000,
        B00011,
        B01110,
        B11100,
        B11100,
        B01110,
        B00011,
        B00000
    };

    lcd.createChar(STARSHIP_PLAYER_CHAR_ID, player_starship_image);
    lcd.createChar(STARSHIP_LASER_CHAR_ID, laser_image);
    lcd.createChar(STARSHIP_ASTEROID_CHAR_ID, asteroid_image);
    lcd.createChar(STARSHIP_EVIL_SHIP_CHAR_ID, evil_ship_image);

    lcd.clear();
    lcd.print("STARSHIP");
    int intro_notes[6] = {NOTE_C, NOTE_C, NOTE_C, NOTE_G, NOTE_G, NOTE_G};
    int intro_times[6] = {100,    100,    100,    300,    300,    600};
    playMusic(intro_notes, intro_times, 6);

    for (;;) {
        bool redraw = star_input(&player, lasers);
        if (redraw) star_draw(player.lane, asteroids, lasers, evils);

        unsigned long current_time = millis();

        if (current_time - gamestate.prev_tick >= gamestate.tick_time) {
            star_tick(&gamestate, &player, asteroids, lasers, evils);
            gamestate.prev_tick = millis();
            star_draw(player.lane, asteroids, lasers, evils);
        }

        if (gamestate.gameover) star_gameover(&gamestate, &player);
        if (gamestate.score == STAR_WIN_SCORE) star_win();
    }
}


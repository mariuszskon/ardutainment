FCGameState::FCGameState() {
    previous_tick = millis();
    tick_time = FC_TICK_TIME;
    score = 0;
    game_over = false;
}

bool FCGameState::tick_ready() {
    return millis() - previous_tick >= tick_time;
}

void FCGameState::reset_tick() {
    previous_tick = millis();
}

FCInputCache::FCInputCache() {
    switch_lane = false;
    capture = false;
    attack = false;
    last_switch_lane = 0;
    last_capture = 0;
    last_attack = 0;
}

void FCInputCache::update() {
    if (digitalRead(FC_SWITCH_LANE) && millis() - last_switch_lane >= FC_TICK_TIME) {
        last_switch_lane = millis();
        switch_lane = true;
    }
    if (digitalRead(FC_CAPTURE) && millis() - last_capture >= FC_TICK_TIME) {
        last_capture = millis();
        capture = true;
    }
    if (digitalRead(FC_ATTACK) && millis() - last_attack >= FC_TICK_TIME) {
        last_attack = millis();
        attack = true;
    }
}

bool FCInputCache::take_switch_lane() {
    bool result = switch_lane;
    switch_lane = false;
    return result;
}

bool FCInputCache::take_capture() {
    bool result = capture;
    capture = false;
    return result;
}

bool FCInputCache::take_attack() {
    bool result = attack;
    attack = false;
    return result;
}

FCEntity::FCEntity() {
    type = -1;
    x = -1;
    y = -1;
    defined = false;
}

void FCEntity::update(FCInputCache& input_cache) {
    (*input)(*this, input_cache);
    (*physics)(*this);
}

void FCEntity::init(int new_type, int new_x, int new_y, void (*new_input)(FCEntity& entity, FCInputCache& input_cache), void (*new_physics)(FCEntity& entity)) {
    type = new_type;
    x = new_x;
    y = new_y;
    input = new_input;
    physics = new_physics;
    defined = true;
}

FCEntityPool::FCEntityPool() {
    for (int i = 0; i < FC_MAX_ENTITIES; i++) {
        entities[i].defined = false;
    }
}

FCEntity* FCEntityPool::make_new(int new_type, int new_x, int new_y, void (*new_input)(FCEntity& entity, FCInputCache& input_cache), void (*new_physics)(FCEntity& entity)) {
    for (int i = 0; i < FC_MAX_ENTITIES; i++) {
        if (!entities[i].defined) {
            entities[i].init(new_type, new_x, new_y, new_input, new_physics);
            return &entities[i];
        }
    }
    return nullptr;
}

FCEntity* FCEntityPool::get_at(int x, int y) {
    for (int i = 0; i < FC_MAX_ENTITIES; i++) {
        if (entities[i].defined) {
            if (entities[i].x == x && entities[i].y == y) {
                return &entities[i];
            }
        }
    }
    return nullptr;
}

void FCEntityPool::update_defined(FCInputCache& input_cache) {
    for (int i = 0; i < FC_MAX_ENTITIES; i++) {
        if (entities[i].defined) {
            entities[i].update(input_cache);
        }
    }
}

void FCInputCollection::button_y_switch(FCEntity& entity, FCInputCache& input_cache) {
    if (input_cache.take_switch_lane()) entity.y = !entity.y;
}

void FCPhysicsCollection::fruit_physics(FCEntity& entity) {
    move_left(entity);
    kill_on_outside(entity);
}

FCWorld::FCWorld() {
    player = pool.make_new(FC_PLAYER_ID, 1, 0, FCInputCollection::button_y_switch, FCPhysicsCollection::null_physics);
}

void FCWorld::entity_generator() {
    if (rand_between(0, 5) == 0) {
        if (rand_between(0, 2)) {
            pool.make_new(FC_FRUIT_ID, 15, rand_between(0, 2), FCInputCollection::null_input, FCPhysicsCollection::fruit_physics);
        } else {
            pool.make_new(FC_ROCK_ID, 15, rand_between(0, 2), FCInputCollection::null_input, FCPhysicsCollection::move_left);
        }
    }
}

void FCWorld::crash_at(int x, int y) {
    pool.make_new(FC_CRASH_ID, x, y, FCInputCollection::null_input, FCPhysicsCollection::null_physics);
}

void FCWorld::check_rock_collision(FCGameState& gamestate) {
    for (int i = 0; i < 2; i++) {
        FCEntity* target = pool.get_at(player->x + 1, i); // checks one space in front of the player - run BEFORE movement
        if (target != nullptr) {
            if (target->type == FC_ROCK_ID) {
                crash_at(player->x, i);
                gamestate.game_over = true;
            }
        }
    }
}

void FCWorld::world_input(FCGameState& gamestate, FCInputCache& input_cache) {
    if (input_cache.take_capture()) {
        tone(PIEZO_PIN, NOTE_G, 125);
        FCEntity* target = nullptr;
        for (int x = player->x + 1; x < 16; x++) {
            target = pool.get_at(x, player->y);
            if (target != nullptr) break;
        }

        if (target != nullptr) {
            target->kill();
            if (target->type == FC_FRUIT_ID) {
                gamestate.score += 1;
            } else if (target->type == FC_ROCK_ID) {
                crash_at(player->x, player->y);
                gamestate.game_over = true;
            }
        }
    } else if (input_cache.take_attack()) {
        tone(PIEZO_PIN, NOTE_B / 2, 125);
        FCEntity* target;
        for (int x = player->x + 1; x < 16; x++) {
            target = pool.get_at(x, player->y);
            if (target != nullptr) break;
        }

        if (target != nullptr) target->kill();
    }
}

void FCWorld::tick(FCGameState& gamestate, FCInputCache& input_cache) {
    check_rock_collision(gamestate);
    world_input(gamestate, input_cache);
    pool.update_defined(input_cache);
    entity_generator();
}

void fc_render(FCGameState& gamestate, FCWorld& world) {
    char scene[2][16];
    for (int row = 0; row < 2; row++) {
        for (int col = 0; col < 16; col++) {
            scene[row][col] = ' ';
        }
    }

    for (int i = 0; i < FC_MAX_ENTITIES; i++) {
        if (world.pool.entities[i].defined) {
            int row = world.pool.entities[i].y;
            int col = world.pool.entities[i].x;
            int type = world.pool.entities[i].type;
            if (scene[row][col] != FC_CRASH_ID) { // make sure crash is always visible - never overriden
                scene[row][col] = type;
            }
        }
    }

    char score_chars[2];
    two_digits_to_chars(gamestate.score, score_chars);

    lcd.clear();
    for (int row = 0; row < 2; row++) {
        lcd.setCursor(0, row);
        for (int col = 0; col < 16; col++) {
            lcd.write(scene[row][col]);
        }
    }

    if (scene[!world.player->y][world.player->x] != FC_CRASH_ID) { // don't override crash
        lcd.setCursor(world.player->x, !world.player->y); // draw the pole that supports the player
        lcd.write(FC_NON_PLAYER_ID);
    }

    lcd.setCursor(0, 0);
    lcd.write(score_chars[0]);
    lcd.setCursor(0, 1);
    lcd.write(score_chars[1]);
}

void fc_game_over() {
    lcd.setCursor(3, 0);
    lcd.print("GAME OVER");
    int lose_notes[4] = {NOTE_E, NOTE_D, NOTE_C, NOTE_C};
    int lose_times[4] = {500,    250,    125,    1125};
    playMusic(lose_notes, lose_times, 4);

    while (true) {}
}

void fc_win() {
    lcd.setCursor(3, 0);
    lcd.print("YOU WIN!");
    int win_notes[4] = {NOTE_G, NOTE_G, NOTE_E, NOTE_A};
    int win_times[4] = {250,    250,    125,    1375};
    playMusic(win_notes, win_times, 4);

    while (true) {}
}

void fc_tick(FCGameState& gamestate, FCWorld& world, FCInputCache& input_cache) {
    world.tick(gamestate, input_cache);
    
}

void fc_init_chars() {
    uint8_t player_image[8] = {
        B01100,
        B01100,
        B01100,
        B01110,
        B01111,
        B01110,
        B01100,
        B01100
    };

    uint8_t pole_image[8] = {
        B01100,
        B01100,
        B01100,
        B01100,
        B01100,
        B01100,
        B01100,
        B01100
    };
    
    uint8_t fruit_image[8] = {
        B00100,
        B01110,
        B01110,
        B11111,
        B11111,
        B11111,
        B01110,
        B00000
    };

    uint8_t rock_image[8] = {
        B00000,
        B01110,
        B11111,
        B11110,
        B11111,
        B01111,
        B01110,
        B00000
    };

    lcd.createChar(FC_PLAYER_ID, player_image);
    lcd.createChar(FC_NON_PLAYER_ID, pole_image);
    lcd.createChar(FC_FRUIT_ID, fruit_image);
    lcd.createChar(FC_ROCK_ID, rock_image);
}

void fc_play() {
    fc_init_chars();
    FCInputCache input_cache;
    FCWorld world;
    FCGameState gamestate;

    lcd.clear();
    lcd.print("FRUIT CAPTURE");
    int intro_notes[5] = {NOTE_G, NOTE_E, NOTE_A, NOTE_D, NOTE_B};
    int intro_times[5] = {250,    250,    250,    250,    250};
    playMusic(intro_notes, intro_times, 5);
    
    for (;;) {
        input_cache.update();
        if (gamestate.tick_ready()) {
            gamestate.reset_tick();
            fc_tick(gamestate, world, input_cache);
            fc_render(gamestate, world);
            if (gamestate.game_over) fc_game_over();
            if (gamestate.score == 50) fc_win();
        }
    }
}


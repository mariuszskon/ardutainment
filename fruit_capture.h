#ifndef FRUIT_CAPTURE_H
#define FRUIT_CAPTURE_H

const int FC_TICK_TIME = 300;

const int FC_SWITCH_LANE = BUTTON0_PIN;
const int FC_CAPTURE = BUTTON1_PIN;
const int FC_ATTACK = BUTTON2_PIN;

const int FC_MAX_ENTITIES = 15;

const int FC_PLAYER_ID = 0;
const int FC_NON_PLAYER_ID = 1;
const int FC_FRUIT_ID = 2;
const int FC_ROCK_ID = 3;
const int FC_CRASH_ID = '*';

class FCGameState {
public:
    FCGameState();
    bool tick_ready();
    void reset_tick();
    int score;
    bool game_over;
private:
    unsigned long previous_tick;
    unsigned int tick_time;
};

class FCInputCache {
public:
    FCInputCache();
    bool take_switch_lane();
    bool take_capture();
    bool take_attack();
    void update();
private:
    bool switch_lane;
    bool capture;
    bool attack;
    unsigned long last_switch_lane;
    unsigned long last_capture;
    unsigned long last_attack;
};

class FCEntity {
public:
    FCEntity();
    int type;
    int x;
    int y;
    bool defined;
    void kill() {defined = false;}
    void update(FCInputCache& input_cache);
    void init(int new_type, int new_x, int new_y, void (*new_input)(FCEntity& entity, FCInputCache& input_cache), void (*new_physics)(FCEntity& entity));
private:
    void (*input)(FCEntity& entity, FCInputCache& input_cache);
    void (*physics)(FCEntity& entity);
};

class FCEntityPool {
public:
    FCEntityPool();
    FCEntity* make_new(int new_type, int new_x, int new_y, void (*new_input)(FCEntity& entity, FCInputCache& input_cache), void (*new_physics)(FCEntity& entity));
    FCEntity* get_at(int x, int y);
    void update_defined(FCInputCache& input_cache);
    FCEntity entities[FC_MAX_ENTITIES];
};

namespace FCInputCollection {
    void null_input(FCEntity& entity, FCInputCache& input_cache) {UNUSED(entity); UNUSED(input_cache);}
    void button_y_switch(FCEntity& entity, FCInputCache& input_cache);
}

namespace FCPhysicsCollection {
    void null_physics(FCEntity& entity) {UNUSED(entity);}
    void fruit_physics(FCEntity& entity);

    void move_left(FCEntity& entity) {entity.x -= 1;}
    void kill_on_outside(FCEntity& entity) {if (entity.x < 2) entity.kill();}
};

class FCWorld {
public:
    FCWorld();
    void tick(FCGameState& gamestate, FCInputCache& input_cache);
    FCEntityPool pool;
    FCEntity* player;
private:
    void entity_generator();
    void world_input(FCGameState& gamestate, FCInputCache& input_cache);
    void crash_at(int x, int y);
    void check_rock_collision(FCGameState& gamestate);
};

void fc_render(FCGameState& gamestate, FCWorld& world);

void fc_tick(FCGameState& gamestate, FCWorld& world);

void fc_init_chars();

void fc_play();

#endif

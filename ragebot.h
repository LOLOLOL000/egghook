#pragma once

/*
enum priority_t
{
	priority_lowest = 0,
	priority_default,
	priority_medium,
	priority_high,
};

struct point_t
{
    bool filled{ };
    bool body{ };
    bool limbs{ };
    bool center{ };
    bool lethal{ };
    bool predictive{ };

    int hitbox = -1;
    int chance_ticks;
    int damage{ };

    float hitchance{ };

    LagRecord* record{ };
    vec3_t position{ };

    __forceinline point_t()
    {
    }

    __forceinline point_t(int hitbox, bool center, int damage, LagRecord* record, const vec3_t& point_position)
    {
        this->filled = true;
        this->center = center;
        this->body = hitbox >= HITBOX_PELVIS && hitbox <= HITBOX_CHEST;
        this->limbs = hitbox >= HITBOX_L_THIGH && hitbox <= HITBOX_R_FOREARM;
        this->hitbox = hitbox;
        this->damage = damage;
        this->record = record;
        this->position = point_position;
    }

    __forceinline void reset()
    {
        filled = false;
        body = false;
        limbs = false;
        center = false;
        lethal = false;
        predictive = false;

        hitbox = -1;
        damage = 0;
        chance_ticks = 0;

        hitchance = 0.f;

        record = nullptr;
        position.clear();
    }
};

struct restore_record_t
{
    float duck{ };
    float lby{ };
    float sim_time{ };

    ang_t angles{ };
    vec3_t origin{ };
    vec3_t absorigin{ };
    vec3_t velocity{ };

    vec3_t bbmin{ };
    vec3_t bbmax{ };

    matrix3x4_t bonecache[128]{ };

    std::array< float, 24 > poses{ };
    bool filled{ false };

    __forceinline void reset()
    {
        if (!filled)
            return;

        duck = 0.f;
        lby = 0.f;
        sim_time = 0.f;

        angles.clear();
        origin.clear();
        absorigin.clear();
        velocity.clear();

        bbmin.clear();
        bbmax.clear();

        std::memset(bonecache, 0, sizeof(bonecache));

        poses = { };

        filled = false;
    }
};

class c_rage_bot
{
private:
    std::array< restore_record_t, 64 > backup{ };

public:
    bool working{ };
    bool firing{ };
    bool should_slide{ };
    bool force_accuracy{ };
    bool stopping{ };
    bool debug_aimbot = false;
    Player* target{ };

    bool should_stop(bool shoot_check = true);
    bool auto_stop();
    void start_stop();
    void on_predict_start();

    std::vector< int > get_hitboxes();
    std::vector< std::pair< vec3_t, bool > > get_multipoints(Player* player, int hitbox, matrix3x4_t* matrix);

    void store(Player* player);
    void set_record(Player* player, LagRecord* record, matrix3x4_t* matrix = nullptr);
    void restore(Player* player);

    void proceed_aimbot();

    int get_min_damage(Player* player);
};

extern c_rage_bot g_rage_bot;*/
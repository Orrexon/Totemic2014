#pragma once

#include <SFML\Graphics\Color.hpp>

const float POINTS_PER_SECOND = 100.f;
const float POINTS_TO_WIN = 5000.f;
const float TRAP_SECONDS_INTERVAL = 5.f;
const float TRAP_LOADS_SECONDS = 2.f;
const float TRAP_EXPLODES_SECONDS = 1.f;
const int TIMER_POS_X = 685;
const int TIMER_POS_Y = 22;
const int TIMER_WIDTH = 555;
const int TIMER_HEIGHT = 16;
const int FRAME_BORDER_WIDTH = 23; // 23
const float POINTS_PER_COIN = 2.f;
const float COIN_MULTIPLIER = 2.f;
const float COINS_TIMER_SECONDS = 5.f; // 5.f default
const int MAX_COINS = 4;
const int MAX_POWERUP = 2;
const float POWERUPS_SPAWN_RATE = 5.f; // 10.f default
const int FLOATING_SCORE_TEXT_MIN_SIZE = 20;
const int FLOATING_SCORE_TEXT_MAX_SIZE = 50;
const float FLOATING_SCORE_TEXT_POINTS_PER_FONTSIZE = 2.f; // 2 points to reach a fontsize of 2, capped at 10
const float FLOATING_SCORE_HOTSPOT_SUM = 100.f; // Amount of score shown in the floating text in the hotspot
const float POWERUP_STUN_TIME = 2.f; // Amount of time in seconds players is stunned by the lightning powerup
const int SPAWNZONE_COLOR_ALPHA = 100; // Not used..
const float SPAWNZONE_RADIUS = 250; // Not used..
const unsigned int SPAWNZONE_POINTCOUNT = 60; // Not used..
const float SPAWNZONE_STROKE_THICKNESS = 4; // Not used..
const int TOTEM_ALPHA_WHEN_BLOCK = 140; // The opacity of the totem when a player is behind it
const float COIN_GATHERED_SPEED = 10.f; // Speed of the coin traveling to the totem
const float SCORE_PER_COIN = POINTS_TO_WIN * 0.04; // Amount of score given by a coin; 4% of maxium score
const float RANDOM_PLAYER_SPAWN_TOLERANCE = 300; // How far must a gatherer be from a spawnpoint before it becomes "not occupied"
const float SHIELD_TIMER = 4.f; // The powerup shield is activated for 5 seconds
const float SHIELD_RADIUS = 140.f; // Radius of the powerup shield
const float SHIELD_FORCE_RADIUS_MIN = 50.f;
const float SHIELD_MAX_FORCE = 6000.f;
const float SHIELD_STUNNED_TIMER = 0.3f; // Amount of seconds a shieldstunned char cannot move
const float GATHERER_DEATH_ANIM_DURATION = 1.f;
const float DEFENDER_DEATH_ANIM_DURATION = 1.f;
const float GATHERER_WALK_ANIM_DURATION = 0.1f;
const float DEFENDER_WALK_ANIM_DURATION = 0.5f;
const sf::Color TIMER_BG_COLOR(69, 52, 17);
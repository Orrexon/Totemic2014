#pragma once

#include <SFML\Graphics\Color.hpp>

const float POINTS_PER_SECOND = 5.f;
const float POINTS_TO_WIN = 500.f;
const float TRAP_SECONDS_INTERVAL = 5.f;
const float TRAP_LOADS_SECONDS = 2.f;
const float TRAP_EXPLODES_SECONDS = 1.f;
const int TIMER_POS_X = 685;
const int TIMER_POS_Y = 22;
const int TIMER_WIDTH = 555;
const int TIMER_HEIGHT = 16;
const int FRAME_BORDER_WIDTH = 23;
const float POINTS_PER_COIN = 2.f;
const float COIN_MULTIPLIER = 2.f;
const float COINS_TIMER_SECONDS = 5.f;
const int MAX_COINS = 4;
const int MAX_POWERUP = 2;
const float POWERUPS_SPAWN_RATE = 10.f; // 10.f default
const int FLOATING_SCORE_TEXT_MIN_SIZE = 20;
const int FLOATING_SCORE_TEXT_MAX_SIZE = 50;
const float FLOATING_SCORE_TEXT_POINTS_PER_FONTSIZE = 2.f; // 2 points to reach a fontsize of 2, capped at 10
const float FLOATING_SCORE_HOTSPOT_SUM = 1.f;
const float POWERUP_STUN_TIME = 2.f; // In seconds
const float POWERUP_SHIELD_TIME = 3.f;
const float SHIELD_DEFLECTION_RADIUS = 60.f;
const float DEFLECTIONTIME = 3.f;
const sf::Color TIMER_BG_COLOR(69, 52, 17);
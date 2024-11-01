//
// Created by fran on 24/10/24.
//

#include "player.h"
#include "player_macros.h"
#include <sstream>

Player::Player(int player_id, int position_x, int position_y, bool right, State state)
    : player_id(player_id), position_x(position_x), position_y(position_y), right(right), state(state) {}

int Player::get_player_id() const { return player_id; }

int Player::get_position_x() const { return position_x; }

std::string Player::get_text() const {
  std::stringstream ss;
  ss << "PlayerId: " << player_id << " Position: (" << position_x << ", "
     << position_y << ")";
  return ss.str();
}

int Player::get_position_y() const { return position_y; }

void Player::move(bool is_right) {
  if (is_right) {
    position_x++;
  } else {
    position_x--;
  }
}

bool Player::is_dead() const { return state == DEAD; }

bool Player::is_jumping() const { return state == JUMPING; }

bool Player::is_falling() const { return state == FALLING; }

bool Player::is_playing_dead() const { return state == PLAYING_DEAD; }

bool Player::is_right() const { return right; }

bool Player::is_aiming_upwards() const { return state == AIMING_UPWARDS; }

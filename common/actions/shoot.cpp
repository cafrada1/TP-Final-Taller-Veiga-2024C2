#include "shoot.h"
#include "action.h"

Shoot::Shoot(int player_id, bool shooting)
    : Action(PICK_DROP), shooting(shooting), player_id(player_id) {}

bool Shoot::is_shooting() const { return shooting; }

int Shoot::get_player_id() const { return player_id; }
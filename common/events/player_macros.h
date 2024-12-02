#ifndef TALLER_TP_PLAYER_MACROS_H
#define TALLER_TP_PLAYER_MACROS_H

enum State {
  BLANK = 0x00,
  DEAD = 0x01,
  JUMPING = 0x02,
  FALLING = 0x03,
  PLAYING_DEAD = 0x04,
  FLAPPING = 0x05,
  AIMING_UPWARDS = 0x06,
  WALKING = 0x07,
  RECOIL = 0x08
};

#endif // TALLER_TP_PLAYER_MACROS_H
#ifndef DUCK_H
#define DUCK_H

#include "configurations.h"
#include "game_map.h"
#include "weapons/weapon.h"
#include <atomic>
#include "hitBox.h"


class GameMap;

class Duck {
private:
  int id;
  int posX, posY;
  int velX, velY;
  bool jumping;
  bool flapping;
  bool shooting;
  GameMap &map;
  std::unique_ptr<Weapon> weapon;
  bool isRight;
  bool aimingUpwards;
  State state;
  bool hasWeapon;
  bool hasHelmet;
  bool hasArmour;
  bool isOnPlatform;

public:
  Duck(std::atomic<int> id, int posX, int posY, GameMap &map);

  void moveRight();
  void moveLeft();
  void move(bool isRight);
  void jump();
  void flap();
  void stopMoving();
  void update();
  void resetJumpState();
  hitBox getBoundingBox() const;
  bool checkCollisionWithPlatform(const Structure &platform);
  void stopAiming();
  void shoot();
  void equipHelmet();
  void equipArmour();
  void equipWeapon(std::unique_ptr<Weapon> newWeapon);
  void takeDamage();
  void pickUp();
  void leave();
  void playDead();
  void aimUpwards();
  void standBack(int count);
  int getPositionX() const;
  int getPositionY() const;
  int getId() const;
  bool getDirection() const;
  State getState() const;
  bool isWearingHelmet() const;
  bool isWearingArmour() const;
  bool isJumping() const;
  bool isShooting() const;
  bool isAimingUpwards() const;
  const Weapon *getWeapon() const;
  bool isFalling() const;

  ~Duck();
};

#endif // DUCK_H

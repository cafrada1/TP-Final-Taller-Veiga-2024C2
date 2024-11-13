#include "duck.h"
#include <iostream>
#include "configurations.h"

#define CONFIG Configurations::configurations()

Duck::Duck(std::atomic<int> id, int posX, int posY, GameMap &map)
    : id(id), posX(posX), posY(posY), map(map) {
  velX = 0;
  velY = 0;
  jumping = false;
  flapping = false;
  shooting = false;
  isRight = true;
  weapon = nullptr;
  state = State::BLANK;
  hasHelmet = false;
  hasArmour = false;
}

void Duck::moveLeft() {
  velX = -CONFIG.getSpeedX();
  isRight = false;
  state = State::BLANK;
}

void Duck::moveRight() {
  velX = CONFIG.getSpeedY();
  isRight = true;
  state = State::BLANK;
}

void Duck::move(bool isRight) {
  if (isRight) {
    moveRight();
  } else {
    moveLeft();
  }
}

void Duck::jump() {
  if (!jumping) {
    velY = -CONFIG.getSpeedY();
    jumping = true;
    flapping = false;
    state = State::JUMPING;
  }
}

void Duck::flap() {
  if (jumping && velY > 0) {
    velY = -CONFIG.getFlappingSpeed();
    flapping = true;
    state = State::BLANK;
  }
}

void Duck::update() {
  posX += velX;
  posY += velY;

  if (map.checkCollisionsWithBorders(id)) {
    posX -= velX;
    posY -= velY;
    velX = 0;
    velY = 0;
  }

  if (jumping) {
    velY += CONFIG.getGravity();
    state = State::FALLING;
  }

  if (posY >= GROUNDLEVEL) {
    posY = GROUNDLEVEL;
    jumping = false;
    velY = 0;
  }
}

void Duck::shoot() {
  if (weapon) {
    weapon->shoot(this);
    shooting = true;
    state = State::BLANK;
  }
}

void Duck::equipHelmet() { hasHelmet = true; }

void Duck::equipArmour() { hasArmour = true; }

void Duck::equipWeapon(Weapon* newWeapon) {
  if (weapon) delete weapon;
  weapon = newWeapon;
}

void Duck::takeDamage() {
  if (hasHelmet) {
    hasHelmet = false;
  } else if (hasArmour) {
    hasArmour = false;
  } else {
    // muere
    state = State::DEAD;
  }
}

void Duck::pickUp() {
// para agarrar
}

void Duck::leave() {
  // para soltar
}

void Duck::playDead() {
   
}

int Duck::getPositionX() const { return posX; }

int Duck::getPositionY() const { return posY; }

int Duck::getId() const { return id; }

bool Duck::getDirection() const { return isRight; }

State Duck::getState() const { return state; }

Duck::~Duck() { delete weapon; }
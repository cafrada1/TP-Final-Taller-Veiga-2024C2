#include "client.h"

#include "common/actions/connection/join.h"
#include "common/actions/player/move.h"
#include "common/events/base/event.h"
#include "common/protocol/common/protocol_error.h"
#include <iostream>
#include <utility>

#define NO_CONECTADO 0

Client::Client(const char *hostname, const char *service_name)
    : commands(), events(),
      connection(Socket(hostname, service_name), Encoder(), &this->events,
                 &this->commands),
      game_code(NO_CONECTADO), player_id_1(SIN_ASIGNAR),
      player_id_2(SIN_ASIGNAR), game_mode(SIN_ESPECIFICAR) {}

void Client::run() { connection.start(); }

void Client::close() {
  if (!connection.is_closed()) {
    connection.close();
  }
  connection.join();
}

bool Client::is_closed() const { return connection.is_closed(); }

void Client::send_action(std::shared_ptr<Action> &action) {
  if (game_code == NO_CONECTADO && action->get_type() != JOIN_REQUEST &&
      action->get_type() != CREATE_REQUEST &&
      action->get_type() != REFRESH_REQUEST) {
    std::cout << "No se puede realizar la accion" << std::endl;
    return;
  }

  commands.push(action);
}

void Client::assign_player_ids(std::shared_ptr<Event> &event) {
  player_id_1 = event->get_player_id_1();
  player_id_2 = event->get_player_id_2();
}

int Client::get_game_code() const { return game_code; }

void Client::set_game_code(int code) { game_code = code; }

std::shared_ptr<Event> Client::read_event() {
  std::shared_ptr<Event> event = events.pop();
  switch (event->get_type()) {
  case CREATE_GAME:
    game_code = event->get_game_room().get_game_code();
    assign_player_ids(event);
    break;
  case JOIN_GAME:
    assign_player_ids(event);
    break;
  default:
    break;
  }
  return event;
}

int Client::get_player_id_1() const { return player_id_1; }

int Client::get_player_id_2() const { return player_id_2; }

bool Client::is_connected() const { return player_id_1 != SIN_ASIGNAR; }

std::shared_ptr<Event> Client::try_read() {
  std::shared_ptr<Event> event;
  events.try_pop(event);
  return event;
}
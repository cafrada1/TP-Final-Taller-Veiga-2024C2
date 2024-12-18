#include "game.h"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <SDL2pp/Font.hh>
#include <SDL2pp/SDL2pp.hh>
#include <string>

#define BACKGROUND_MUSIC_VOLUME SDL_MIX_MAXVOLUME / 15
#define BACKGROUND_MUSIC_CHANNEL 0
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define VICTORY_MUSIC_VOLUME SDL_MIX_MAXVOLUME / 10

#define FONT_PATH "../client/assets/sprites/font.ttf"
#define FONT_SIZE 24

#define BACKGROUND_MUSIC_PATH "../client/assets/audio/background-music.wav"
#define VICTORY_MUSIC_PATH "../client/assets/audio/challengeWin.wav"

// cppcheck-suppress uninitMemberVar
Game::Game(Client &client) try
    : client(client), sdl(SDL_INIT_VIDEO), font(FONT_PATH, FONT_SIZE),
      window("Duck Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
             WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN),
      renderer(window, -1, SDL_RENDERER_ACCELERATED),
      camera(renderer, SDL2pp::Rect{0, 0, WINDOW_WIDTH, WINDOW_HEIGHT}),
      mixer(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS,
            4096),
      backgroundMusic(BACKGROUND_MUSIC_PATH), victoryMusic(VICTORY_MUSIC_PATH),
      map(renderer), ducks(), crates(), bullets(), itemSpawns(), explosions(),
      throwables(), item(renderer), bullet(renderer), crate(renderer),
      explosion(renderer), throwable(renderer), stop(false), pause(false) {
} catch (std::exception &e) {
  throw std::exception();
}

int Game::start() {
  try {
    backgroundMusic.SetVolume(BACKGROUND_MUSIC_VOLUME);
    victoryMusic.SetVolume(VICTORY_MUSIC_VOLUME);

    mixer.PlayChannel(BACKGROUND_MUSIC_CHANNEL, backgroundMusic, -1);

    ActionHandler actionHandler(client);

    auto rate = 1000 / 60;
    int id1 = client.get_player_id_1() - 1;
    int id2 = client.get_player_id_2() - 1;

    while (true) {
      auto t1 = SDL_GetTicks();
      update_state();
      if (stop) {
        break;
      }
      if (pause) {
        pause = false;
        continue;
      }
      if (id2 >= 1) {
        actionHandler.processEvents(ducks[id1]->getWeapon(),
                                    ducks[id2]->getWeapon());
      } else {
        actionHandler.processEvents(ducks[id1]->getWeapon(),
                                    WeaponId::NO_WEAPON);
      }

      render(); // Renderizamos el juego

      /* IF BEHIND, KEEP WORKING */
      // Buscamos mantener un ritmo constante para ejecutar las funciones
      //  de update state, processEvents y render a una velocidad especifica
      //  rate

      auto t2 = SDL_GetTicks();    // Inicializamos 't2' con el tiempo actual en
                                   // milisegundos
      int rest = rate - (t2 - t1); // Cantidad de tiempo que debe esperarse
      // antes de la proxima iteracion. Al tiempo deseado entre iteraciones le
      // restamos la diferencia entre 't2' y 't1' que es el tiempo que se tardo
      // en actualizar y renderizar

      // Si 'rest' es menor a cero quiere decir que nos estamos retrasando en
      // comparacion con el ritmo deseado
      if (rest < 0) {
        auto behind = -rest; // Representa cuanto tiempo estamos atrasados
        auto lost = behind - behind % rate; // Representa cuanto tiempo perdimos
        t1 += lost; // Ajustamos 't1' para ponernos al dia con el tiempo perdido
        for (auto &duck : ducks) {
          duck->updateFrame(int(lost / rate));
        }
        // Aumentamos la cantidad de iteraciones del pato para reflejar las
        // iteraciones que se han perdido debido al retraso

        // Si 'rest' es mayor o igual a cero quiere decir que no nos estamos
        // quedando atras
      } else {
        SDL_Delay(rest); // Descansamos el valor 'rest' antes de la proxima
                         // iteracion para
                         // mantener un ritmo constante
      }

      t1 +=
          rate; // Aumentamos 't1' en 'rate' para programar la proxima iteracion
      for (auto &duck : ducks) {
        duck->updateFrame(); // Aumentamos la cantidad de iteraciones en cada
                             // pato
      }
    }
    return 0;

  } catch (const std::exception &e) {
    return 1;
  }
}

void Game::update_state() {
  std::shared_ptr<Event> event = client.try_read();
  if (event == nullptr) {
    return;
  }
  if (event->get_type() == BROADCAST) {
    update(*event);
  } else if (event->get_type() == MAP_LOAD) {
    map.load(*event);
    pause = true;
  } else if (event->get_type() == GAME_OVER) {
    end(*event);
  } else if (event->get_type() == SCORE) {
    showScores(*event);
  }
}

void Game::update(const Event &broadcast) {
  std::vector<SDL2pp::Rect> playerRects;
  // Cargado de los patos por primera vez
  if (ducks.size() != broadcast.get_players().size()) {
    ducks.clear();
    for (const auto &player : broadcast.get_players()) {
      std::shared_ptr duck =
          std::make_shared<Duck>(renderer, player.get_player_id());
      ducks.push_back(duck);
    }
  }
  for (const auto &player : broadcast.get_players()) {
    ducks[player.get_player_id() - 1]->update(player);
    if (player.get_state() != DEAD) {
      playerRects.push_back(SDL2pp::Rect(player.get_position_x(),
                                         player.get_position_y(),
                                         Duck::DUCK_WIDTH, Duck::DUCK_HEIGHT));
    }
  }
  camera.update(playerRects);

  bullets = broadcast.get_bullets();
  itemSpawns = broadcast.get_item_spawns();
  crates = broadcast.get_crates();
  explosions = broadcast.get_explosions();
  throwables = broadcast.get_throwables();
}

void Game::showScores(const Event &score) {
  pause = true;
  mixer.HaltChannel(BACKGROUND_MUSIC_CHANNEL);
  camera.reset();
  renderer.SetDrawColor(0, 0, 0, 255);
  renderer.Clear();
  std::list<std::string> names = score.get_names();
  std::list<int> scores = score.get_scores();
  Uint32 startTime = SDL_GetTicks();
  bool flash = true;
  while (SDL_GetTicks() - startTime < 5000) { // Flash for 5 seconds
    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Clear();
    if (flash) {
      auto nameIt = names.begin();
      auto scoreIt = scores.begin();
      for (int i = 0; nameIt != names.end() && scoreIt != scores.end();
           ++i, ++nameIt, ++scoreIt) {
        std::string scoreText =
            *nameIt + "        " + std::to_string(*scoreIt) + "    wins";
        SDL2pp::Texture scoreTexture(
            renderer,
            font.RenderText_Blended(scoreText, SDL_Color{255, 255, 255, 255}));
        renderer.Copy(scoreTexture, SDL2pp::NullOpt,
                      SDL2pp::Rect(200, 100 + i * 30, scoreTexture.GetWidth(),
                                   scoreTexture.GetHeight()));
      }
    }

    renderer.Present();
    SDL_Delay(500);
    flash = !flash;
  }
  mixer.PlayChannel(BACKGROUND_MUSIC_CHANNEL, backgroundMusic, -1);
}

void Game::showVictoryScreen(const Event &gameOver) {
  mixer.HaltChannel(BACKGROUND_MUSIC_CHANNEL);
  mixer.PlayChannel(BACKGROUND_MUSIC_CHANNEL, victoryMusic, 0);
  Uint32 startTime = SDL_GetTicks();
  bool flash = true;
  while (SDL_GetTicks() - startTime < 5000) {
    renderer.SetDrawColor(0, 0, 0, 255);
    renderer.Clear();
    if (flash) {
      std::string victoryText = gameOver.get_winner().get_name() + " wins!";
      SDL2pp::Texture victoryTexture(
          renderer,
          font.RenderText_Blended(victoryText, SDL_Color{255, 255, 255, 255}));
      renderer.Copy(victoryTexture, SDL2pp::NullOpt,
                    SDL2pp::Rect(237, 227.5, victoryTexture.GetWidth(),
                                 victoryTexture.GetHeight()));
    }
    renderer.Present();
    SDL_Delay(500);
    flash = !flash;
  }
}

void Game::end(const Event &gameOver) {
  stop = true;
  camera.reset();
  showVictoryScreen(gameOver);
}

void Game::render() {
  renderer.SetDrawColor(0, 0, 0, 255);
  renderer.Clear();
  map.render();
  for (const auto &itemSpawn : itemSpawns) { // Dibujo los items que spawnean
    item.render(itemSpawn);
  }
  for (auto &duck : ducks) {
    duck->render();
  }
  for (const auto &crateDTO : crates) { // Dibujo los crates
    crate.render(crateDTO);
  }
  for (const auto &bulletDTO : bullets) { // Dibujo las balas
    bullet.render(bulletDTO);
  }
  for (const auto &explosionDTO : explosions) { // Dibujo las explosiones
    explosion.render(explosionDTO);
  }
  for (const auto &throwableDTO :
       throwables) { // Dibujo los throwables (granadas, bananas)
    throwable.render(throwableDTO);
  }
  renderer.Present();
}
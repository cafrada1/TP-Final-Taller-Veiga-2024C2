#include "mainwindow.h"
#include "lobby_updater.h"
#include "client/graphic/game.h"
#include "client/graphic/updater_game.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    if (argc != 3) {
        throw std::runtime_error("invalid arguments quantity");
    }
    Client client(argv[1], argv[2]);
    MainWindow w(&client);
    LobbyUpdater lobbyUpdater(&client, &w);

    client.run();
    try {
        lobbyUpdater.start();
        w.show();

        app.exec();

        lobbyUpdater.close();
        //lobbyUpdater.join();
    }
    catch (const std::exception &e) {}

    auto sdl_game = Game(client);

    UpdaterGame updaterGame(client, sdl_game);
    updaterGame.start();

    sdl_game.start();

    updaterGame.close();
    client.close();
    updaterGame.join();

    return 0;
}

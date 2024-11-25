// mainwindow.cpp
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "common/actions/connection/join.h"
#include "common/actions/connection/refresh_games.h"
#include <QFile>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QPalette>
#include <QPixmap>
#include <QStyledItemDelegate>

MainWindow::MainWindow(Client *client, QWidget *parent)
        : QMainWindow(parent), client(client), ui(new Ui::MainWindow) {
    // Establecer un tamaño fijo para la ventana

    // Configurar la interfaz de usuario
    ui->setupUi(this);
    this->setWindowTitle("Duck Game");
    setupAudio();

    // Conectar la señal aboutToQuit a la ranura handleAboutToQuit
    connect(QApplication::instance(), &QApplication::aboutToQuit, this, &MainWindow::handleAboutToQuit);

    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow() {
    player->stop();
    delete player;
    delete audio;
    delete ui;
}

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        player->setPosition(0);
        player->play();
    }
}

void MainWindow::setupAudio() {
    player = new QMediaPlayer(this);
    audio = new QAudioOutput(this);
    player->setAudioOutput(audio);

    audio->setVolume(0.1);

    player->setSource(QUrl("qrc:/sound/menumusic.wav"));

    connect(player, &QMediaPlayer::mediaStatusChanged, this,
            &MainWindow::onMediaStatusChanged);

    // Reproducir
    player->play();
}

static std::vector<QColor> colors = {
        QColor(255, 255, 255),
        QColor(255, 0, 0),
        QColor(238, 158, 213),
        QColor(231, 111, 15),
        QColor(154, 154, 154),
        QColor(255, 255, 0)
};

void MainWindow::update_players_list(const Event &event) {
    ui->playerList->clear();
    QStringList players;
    std::vector<int> ids;
    for (auto &player_data: event.get_players_data()) {
        std::string player_string = "Name: " + player_data.get_name() + " Id: " + std::to_string(player_data.get_id());
        players.append(player_string.c_str());
        ids.emplace_back(player_data.get_id());
    }
    ui->playerList->addItems(players);

    for (int i = 0; i < ui->playerList->count(); i++) {
        QListWidgetItem *item = ui->playerList->item(i);
        item->setForeground(colors[ids[i] - 1]);
    }
    connect(ui->playerList, &QListWidget::itemClicked, this,
            [this](QListWidgetItem *item) {
                item->setSelected(false);
            }
    );
    connect(ui->playerList, &QListWidget::itemPressed, this,
            [this](QListWidgetItem *item) {
                item->setSelected(false);
            }
    );

    if (client->get_player_id_1() != 1) {
        ui->startGameButton->hide();
    }
}

void MainWindow::show_connected_players(const Event &event) {
    GameRoom game_room = event.get_game_room();
    std::string ss = "Servidor " + std::to_string(game_room.get_game_code()) +
                     " |  " + game_room.get_game_name() +
                     " | Jugadores conectados: " +
                     std::to_string(game_room.get_actual_players()) + "/" +
                     std::to_string(game_room.get_max_players());

    ui->playerListLabel->setText(ss.c_str());
    ui->lobbyTitle->setText(game_room.get_game_name().c_str());
    update_players_list(event);
}

void MainWindow::on_Volver_clicked() { ui->stackedWidget->setCurrentIndex(0); }

void MainWindow::on_BackButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_leaveLobbyButton_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::exit() {
    client->close();
    close();
}

void MainWindow::on_Exit_clicked() {
    exit();
}

void MainWindow::handleAboutToQuit() {
    if (client->is_closed()) {
        return;
    }
    if (client->is_connected()) {
        close();
        return;
    }
    exit();
}


void MainWindow::on_Create_clicked() {
    if (client->is_connected()) {
        ui->stackedWidget->setCurrentIndex(4);
        return;
    }
    if (ui->GameModeCreate->currentIndex() == 0) {
        ui->player2namecreate->hide();
    } else {
        ui->player2namecreate->show();
    }

    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_connectCreat_clicked() {
    std::shared_ptr<Action> action;
    std::string game_name = ui->LobbyName->text().toStdString();
    int max_players = ui->LimitPlayerQuantity->currentText().toInt();
    std::string player_name_1 = ui->player1namecreate->text().toStdString();
    std::string player_name_2;

    GameMode mode = UN_JUGADOR;
    if (ui->GameModeCreate->currentIndex() != 0) {
        mode = DOS_JUGADORES;
        player_name_2 = ui->player2namecreate->text().toStdString();
    }

    action = std::make_shared<Create>(mode, max_players, game_name, player_name_1, player_name_2);

    client->send_action(action);

    ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::on_GameModeCreate_activated(int index) {
    if (index == 0) {
        ui->player2namecreate->hide();
    } else {
        ui->player2namecreate->show();
    }
}

void MainWindow::on_startGameButton_clicked() {
    std::shared_ptr<Action> action = std::make_shared<Action>(START);
    client->send_action(action);
}

void MainWindow::on_Join_clicked() {
    if (client->is_connected()) {
        ui->stackedWidget->setCurrentIndex(4);
        return;
    }
    setupServerList();
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_GameModeJoin_activated(int index) {
    if (index == 0) {
        ui->Player2NameJoin->hide();
    } else {
        ui->Player2NameJoin->show();
    }
}

void MainWindow::on_refreshButton_clicked() {
    std::shared_ptr<Action> action = std::make_shared<RefreshGames>();
    client->send_action(action);
}

void MainWindow::on_Connect_clicked() {
    std::string player_name_1 = ui->Player1NameJoin->text().toStdString();
    std::string player_name_2;

    GameMode mode = UN_JUGADOR;
    if (ui->GameModeJoin->currentIndex() != 0) {
        mode = DOS_JUGADORES;
        player_name_2 = ui->Player2NameJoin->text().toStdString();
    }

    int game_code = client->get_game_code();
    std::shared_ptr<Action> action = std::make_shared<Join>(game_code, mode, player_name_1, player_name_2);

    client->send_action(action);

    ui->stackedWidget->setCurrentIndex(4);
}

void MainWindow::RefreshServerList(Event &event) {
    ui->serverList->clear();

    QStringList servers;

    std::list<GameRoom> game_rooms = event.get_games();
    for (auto &game: game_rooms) {
        std::string ss = "Servidor " + std::to_string(game.get_game_code()) +
                         " | " + game.get_game_name() +
                         " - Online (" + std::to_string(game.get_actual_players()) +
                         "/" + std::to_string(game.get_max_players()) +
                         " jugadores)";
        servers.append(ss.c_str());
    }

    // Agrega cada servidor al QListWidget
    ui->serverList->addItems(servers);

    // TODO: cambiarlo por un mapa al cliente
    connect(ui->serverList, &QListWidget::itemClicked, this,
            [this](QListWidgetItem *item) {
                ui->Player2NameJoin->hide();
                ui->stackedWidget->setCurrentIndex(3);
                client->set_game_code(item->text().split(" ")[1].toInt());
            });
}

void MainWindow::setupServerList() {
    std::shared_ptr<Action> action = std::make_shared<RefreshGames>();
    client->send_action(action);
}




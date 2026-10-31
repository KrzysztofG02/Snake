#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "game.h"

#include <SFML/Window/VideoMode.hpp>


using namespace std;



MainWindow::MainWindow(Game &game, QWidget *parent):
    QMainWindow{parent},
    mGame{game},
    ui{new Ui::MainWindow}
{
    ui->setupUi(this);

    GameSettings<unsigned short> const &settings = mGame.getGameSettings();

    this->setWindowTitle(Game::gameName);


    ui->boardLengthSlider->setValue(settings.getCellsPerSideCount());
    ui->gameIntervalSlider->setValue(settings.getUpdateInterval() / settings.updateIntervalSingleStep);
    ui->lineThicknessSlider->setValue(settings.getLineThickness());

    ui->boardLengthLabel->setText(QString::number(settings.getCellsPerSideCount()));
    ui->gameIntervalLabel->setText(QString::number(settings.getUpdateInterval().asMilliseconds()) + " ms");
    ui->lineThicknessLabel->setText(QString::number(settings.getLineThickness()));

    for(sf::VideoMode const &vm : sf::VideoMode::getFullscreenModes())
    {
        mWindowResolutions.emplace(vm.size);
    }

    for(sf::Vector2u const res : mWindowResolutions)
    {
        QString const resString = QString("%1 x %2").arg(res.x).arg(res.y);

        ui->resolutionComboBox->addItem(resString);
    }

    game.setResolution(*mWindowResolutions.cbegin());
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_startButton_clicked()
{
    emit startRequested();
}


void MainWindow::on_boardLengthSlider_valueChanged(int value)
{
    ui->boardLengthLabel->setText(QString::number(value));
    mGame.setCellPerSizeCount(value);
}

void MainWindow::on_gameIntervalSlider_valueChanged(int value)
{
    int const ms = value * mGame.getGameSettings().updateIntervalSingleStep.asMilliseconds();
    ui->gameIntervalLabel->setText(QString::number(ms) + " ms");
    mGame.setGameInterval(sf::milliseconds(ms));
}

void MainWindow::on_lineThicknessSlider_valueChanged(int value)
{
    ui->lineThicknessLabel->setText(QString::number(value));
    mGame.setLineThickness(value);
}

void MainWindow::on_resolutionComboBox_activated(int index)
{
    mGame.setResolution(*next(mWindowResolutions.begin(), index));
}


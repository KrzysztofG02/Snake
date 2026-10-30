#pragma once

#include <QMainWindow>

#include <SFML/System/Vector2.hpp>

#include <set>



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Game;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(Game &game, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();

    void on_boardLengthSlider_valueChanged(int value);

    void on_gameIntervalSlider_valueChanged(int value);

    void on_lineThicknessSlider_valueChanged(int value);

    void on_resolutionComboBox_activated(int index);

signals:
    void startRequested();

private:
    struct SfVector2uAreaComparator
    {
        bool operator()(sf::Vector2u const &a, sf::Vector2u const &b) const
        {
            unsigned long long const areaA = static_cast<unsigned long long>(a.x) * a.y;
            unsigned long long const areaB = static_cast<unsigned long long>(b.x) * b.y;

            if (areaA != areaB)
            {
                return areaA > areaB;
            }

            if (a.x != b.x)
            {
                return a.x > b.x;
            }

            return a.y > b.y;
        }
    };

    Game &mGame;
    Ui::MainWindow *ui;
    std::set<sf::Vector2u, SfVector2uAreaComparator> mWindowResolutions;
};
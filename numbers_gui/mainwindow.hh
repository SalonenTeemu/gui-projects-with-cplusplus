#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <gameboard.hh>
#include <numbertile.hh>
#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QLabel>
#include <QTimer>
#include <map>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    MainWindow(QWidget *parent = nullptr);

    // Destructor
    ~MainWindow();

private slots:
    // Slots for the buttons
    void on_close_button_clicked();
    void on_ready_button_clicked();
    void on_reset_button_clicked();
    void on_pause_button_clicked();

private:
    Ui::MainWindow* ui_;

    // Scene for the gameboard.
    QGraphicsScene* scene_;

    // Timer and attribute for the seconds.
    // Bool-attribute to check if the game is paused.
    QTimer* timer_;
    int seconds_ = 0;
    bool is_paused = false;

    // Value needed to win. Gotten from the goal_spinbox.
    int goal_value_;

    // Attribute to help determine, if the board is created for
    // the first time to avoid creating new Numbertile objects if
    // the board was already created once.
    bool startup_ = true;

    // Vectors for the Rectangle objects of the board and
    // labels, which show the values in the tiles.
    std::vector<std::vector<QGraphicsRectItem*>> tiles_;
    std::vector<std::vector<QLabel*>> labels_;

    // Create a new gameboard.
    GameBoard* gameboard_ = new GameBoard;

    // Const values regarding the geometry of the board,
    // the sizes of the tiles and the sizes of the images
    // in the push buttons. The TILE_SIZE value is changeable
    // with the SIZE value so that the board always stays the
    // same size no matter the amount of tiles.
    const int BOARD_X_AND_Y = 30;
    const int BOARD_WIDTH_AND_HEIGHT = 400;
    const int TILE_SIZE = (BOARD_WIDTH_AND_HEIGHT / SIZE) -1;
    const int IMAGE_SIZE = 50;

    // Map for the colors in the tiles. There are colors up to
    // the default win value, which is 2048. After that the
    // colors of the tiles are same as 2048 i.e. gold.
    // The color is searched by the value of the tile,
    // for example default empty tile (value 0) is white.
    const std::map<int, QColor> colors_ = {
    {0, Qt::white},                       {2, Qt::cyan},
    {4, Qt::yellow},                      {8, Qt::green},
    {16, Qt::blue},                       {32, Qt::red},
    {64, Qt::magenta},                    {128, QColorConstants::Svg::orange},
    {256, QColorConstants::Svg::pink},    {512, QColorConstants::Svg::bisque},
    {1024, QColorConstants::Svg::silver}, {2048, QColorConstants::Svg::gold},
    };

    // Creates the board. Initializes the tiles and labels for them.
    // Called once in the startup.
    void create_board();

    // Sets up the others widgets and connects them to their methods.
    void setup_widgets();

    // Sets up images from the Resources folder for the direction buttons.
    void setup_button_images();

    // Starts the game. Checks if it's a first start using the startup_ attribute.
    void start_game();

    // Updates the colors and labels of the tiles after a move.
    void update_gameboard();

    // Method for the move. Takes the move direction as a parameter,
    // which comes from one of the next 4 methods. Checks if the game
    // is won or lost after a move. If not continues and a new value is
    // added to the board. Updates the colors and labels.
    void move(std::string direction);

    // Helping methods for the move method. Method calls the move method with
    // their direction. Each method is called by their respective button.
    void up();
    void down();
    void left();
    void right();

    // Method when game is lost or won. Update the textbrowser with
    // victory or loss texts. Disable the driection buttons and stop the timer.
    // if bool value lost = true, game is lost, else it's won.
    void won_or_lost(bool lost = true);

    // Method for the timer. The timeout is set to 1 second.
    // Increases the seconds_ value by one.
    // Displays the current seconds and minutes.
    void on_timeout();
};
#endif // MAINWINDOW_HH

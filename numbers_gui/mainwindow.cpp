#include "mainwindow.hh"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui_(new Ui::MainWindow)
{
    ui_->setupUi(this);

    // Rename the window.
    setWindowTitle(tr("2048 Game"));

    // Create the timer.
    timer_ = new QTimer(parent);

    // Set up the widgets, button images and board.
    setup_widgets();
    setup_button_images();
    create_board();
}

MainWindow::~MainWindow()
{
    delete ui_;
    delete timer_;
}

void MainWindow::create_board()
{
    // Scene for graphicsView and rectangles.
    scene_ = new QGraphicsScene(this);

    ui_->graphicsView->setGeometry(BOARD_X_AND_Y, BOARD_X_AND_Y,
                                   BOARD_WIDTH_AND_HEIGHT, BOARD_WIDTH_AND_HEIGHT);
    ui_->graphicsView->setScene(scene_);
    scene_->setSceneRect(BOARD_X_AND_Y, BOARD_X_AND_Y,
                         BOARD_WIDTH_AND_HEIGHT, BOARD_WIDTH_AND_HEIGHT);

    // Set scroll bars to be always off.
    ui_->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui_->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Brush and pen for the board.
    QBrush whitebrush(Qt::white);
    QPen blackpen(Qt::black);
    blackpen.setWidth(2);

    qreal y_value = BOARD_X_AND_Y + 1;
    for (int y = 0; y < SIZE; ++y)
    {
        // Vectors of labels and tiles of a single row.
        std::vector<QGraphicsRectItem*> tiles_of_a_row = {};
        std::vector<QLabel*> labels_of_a_row = {};

        qreal x_value = BOARD_X_AND_Y + 1;

        for (int x = 0; x < SIZE; ++x)
        {
         // Add a tile to the scene.
         QGraphicsRectItem* tile = scene_->addRect(x_value, y_value, TILE_SIZE,
                                                   TILE_SIZE, blackpen, whitebrush);
         // Add a label to the tile and change it's appearance.
         QLabel* label = new QLabel(this);
         label->setAlignment(Qt::AlignCenter | Qt::AlignCenter);
         label->setGeometry(x_value, y_value, TILE_SIZE, TILE_SIZE);
         label->setText("");
         QFont font("Cantarell", 18, QFont::Bold);
         label->setFont(font);

         tiles_of_a_row.push_back(tile);
         labels_of_a_row.push_back(label);

         // Increase the x-value so that the next tiles is next to the previous.
         x_value += TILE_SIZE;
        }

        // Add the vectors to the main vector of the titles and labels.
        tiles_.push_back(tiles_of_a_row);
        labels_.push_back(labels_of_a_row);

        // Increase the y-value so that the next row starts under the previous.
        y_value += TILE_SIZE;
    }
}

void MainWindow::setup_widgets()
{
    // Set ranges and default values for the spinboxes and lcdnumber widgets.
    ui_->seed_spinbox->setValue(1);
    ui_->seed_spinbox->setRange(1, 99);

    ui_->goal_spinbox->setValue(11);
    ui_->goal_spinbox->setRange(2, SIZE * SIZE);

    ui_->second_lcdnumber->setStyleSheet("background-color: white");
    ui_->minute_lcdnumber->setStyleSheet("background-color: white");

    ui_->second_lcdnumber->display(0);
    ui_->minute_lcdnumber->display(0);

    timer_->setInterval(1000);
    connect(timer_, &QTimer::timeout, this, &MainWindow::on_timeout);

    // At the start, before the ready button has been clicked,
    // these buttons are disabled.
    ui_->up_button->setDisabled(true);
    ui_->left_button->setDisabled(true);
    ui_->right_button->setDisabled(true);
    ui_->down_button->setDisabled(true);
    ui_->pause_button->setDisabled(true);
    ui_->reset_button->setDisabled(true);

    ui_->textBrowser->setText("Select the seed value, the goal value "
                              "(default value 2^11 = 2048) and press ready.");

    // Connect each direction button to their method.
    connect(ui_->right_button, &QPushButton::clicked, this, &MainWindow::right);
    connect(ui_->left_button, &QPushButton::clicked, this, &MainWindow::left);
    connect(ui_->down_button, &QPushButton::clicked, this, &MainWindow::down);
    connect(ui_->up_button, &QPushButton::clicked, this, &MainWindow::up);
}

void MainWindow::setup_button_images()
{
    // Vector of arrow images for the direction buttons.
    const std::vector<std::string>
            arrows = {"up_arrow", "down_arrow", "left_arrow", "right_arrow"};

    // Defining where the images can be found and what kind of images they are.
    const std::string PREFIX(":/");
    const std::string SUFFIX(".png");

    // Converting images (png) to pixmaps.
    std::string filename1 = PREFIX + arrows.at(0) + SUFFIX;
    std::string filename2 = PREFIX + arrows.at(1) + SUFFIX;
    std::string filename3 = PREFIX + arrows.at(2) + SUFFIX;
    std::string filename4 = PREFIX + arrows.at(3) + SUFFIX;
    QPixmap image1(QString::fromStdString(filename1));
    QPixmap image2(QString::fromStdString(filename2));
    QPixmap image3(QString::fromStdString(filename3));
    QPixmap image4(QString::fromStdString(filename4));

    // Scaling the pixmaps.
    image1 = image1.scaled(IMAGE_SIZE, IMAGE_SIZE);
    image2 = image2.scaled(IMAGE_SIZE, IMAGE_SIZE);
    image3 = image3.scaled(IMAGE_SIZE, IMAGE_SIZE);
    image4 = image4.scaled(IMAGE_SIZE, IMAGE_SIZE);

    // Set the images to their respective buttons.
    ui_->up_button->setIcon(image1);
    ui_->down_button->setIcon(image2);
    ui_->left_button->setIcon(image3);
    ui_->right_button->setIcon(image4);
}

void MainWindow::start_game()
{
    // If it is a first time start, initialize tiles with nullptrs,
    // else set the current number tiles values to zero.
    if (startup_)
    {
        gameboard_->init_empty();
        gameboard_->fill(ui_->seed_spinbox->value());
        startup_ = false;
    }
    else
    {
        gameboard_->fill(ui_->seed_spinbox->value(), false);
    }
    // Update the gameboard.
    update_gameboard();
}

void MainWindow::update_gameboard()
{
    for (int y = 0; y < SIZE; ++y)
    {
        for (int x = 0; x < SIZE; ++x)
        {
            // Get the value of the tile.
            NumberTile* tile = gameboard_->get_item(std::pair(y, x));
            int value = tile->return_value();

            // If the tiles value is not zero, update
            // the text to the labels_ vector.
            if (!tile->is_empty())
            {
                QString value_as_qstring = QString::number(value);
                labels_.at(y).at(x)->setText(value_as_qstring);
            }
            // If it is zero, clear the possible current text.
            else
            {
                labels_.at(y).at(x)->setText("");
            }
            if (value <= 2048)
            {
                // Set the color from the colors_ map.
                tiles_.at(y).at(x)->setBrush(colors_.at(value));
            }
            else
            {
                // If the tiles value is over 2048,
                // set the color to same as 2048 i.e. gold.
                tiles_.at(y).at(x)->setBrush(colors_.at(2048));
            }
        }
    }
}

void MainWindow::move(std::string direction)
{
    // Check which button send the signal and update the gameboard
    // using that direction. Check possible loss or win situations
    // and update the gameboard.
    if (direction == "UP")
    {
        if (gameboard_->move(UP, goal_value_))
        {
            update_gameboard();
            won_or_lost(false);
        }
    }
    else if (direction == "DOWN")
    {
        if (gameboard_->move(DOWN, goal_value_))
        {
            update_gameboard();
            won_or_lost(false);
        }
     }
     else if (direction == "LEFT")
     {
        if (gameboard_->move(LEFT, goal_value_))
        {
           update_gameboard();
           won_or_lost(false);
        }
     }
     else if (direction == "RIGHT")
     {
        if (gameboard_->move(RIGHT, goal_value_))
        {
            update_gameboard();
            won_or_lost(false);
        }
     }
     if (gameboard_->is_full())
     {
        won_or_lost(true);
     }
     else
     {
        gameboard_->new_value(false);
     }
     update_gameboard();
}

void MainWindow::up()
{ 
    move("UP");
}

void MainWindow::down()
{
    move("DOWN");
}

void MainWindow::left()
{
    move("LEFT");
}

void MainWindow::right()
{
    move("RIGHT");
}

void MainWindow::won_or_lost(bool lost)
{
    if (lost == true)
    {
        ui_->textBrowser->setText("You lost!");
    }
    else
    {
        ui_->textBrowser->setText("Congratulations, You won!");
    }

    // Set the direction buttons and pause button
    // to disabled and stop the timer.
    ui_->up_button->setDisabled(true);
    ui_->left_button->setDisabled(true);
    ui_->right_button->setDisabled(true);
    ui_->down_button->setDisabled(true);
    ui_->pause_button->setDisabled(true);
    ui_->pause_button->setText("Pause");
    timer_->stop();
}

void MainWindow::on_timeout()
{
    seconds_ += 1;

    // Update the displays of the lcdnumber widgets.
    ui_->minute_lcdnumber->display(seconds_ / 60);
    ui_->second_lcdnumber->display(seconds_ % 60);
}

void MainWindow::on_close_button_clicked()
{
    // Close the Mainwindow.
    close();
}

void MainWindow::on_ready_button_clicked()
{
    // Set all unnecessary widgets during a game to disabled and
    // necessary widgets to enabled.
    // Start the timer and update the textbrowser.
    // Set the goal value gotten from the spinbox and start the game.
    ui_->reset_button->setEnabled(true);
    ui_->up_button->setEnabled(true);
    ui_->left_button->setEnabled(true);
    ui_->right_button->setEnabled(true);
    ui_->down_button->setEnabled(true);
    ui_->ready_button->setDisabled(true);
    ui_->seed_spinbox->setDisabled(true);
    ui_->goal_spinbox->setDisabled(true);
    ui_->pause_button->setEnabled(true);
    timer_->start(1000);
    ui_->textBrowser->setText("Game ongoing. Good Luck!");
    goal_value_ = pow(2, ui_->goal_spinbox->value());
    start_game();
}

void MainWindow::on_reset_button_clicked()
{
   // Set all unnecessary widgets not during a game to disabled and
   // necessary to enabled.
   // Stop the timer, update the lcdnumber widgets and set seconds_ to zero.
   // Update the textbrowser and fill the gameboard with zeros and update it.
   ui_->ready_button->setEnabled(true);
   ui_->up_button->setDisabled(true);
   ui_->left_button->setDisabled(true);
   ui_->right_button->setDisabled(true);
   ui_->down_button->setDisabled(true);
   ui_->seed_spinbox->setEnabled(true);
   ui_->goal_spinbox->setEnabled(true);
   ui_->pause_button->setDisabled(true);
   ui_->pause_button->setText("Pause");
   timer_->stop();
   ui_->second_lcdnumber->display(0);
   ui_->minute_lcdnumber->display(0);
   seconds_ = 0;


   ui_->textBrowser->setText("Select the seed value, goal value"
                               " and press ready.");
   gameboard_->fill_with_zeros();
   update_gameboard();
}


void MainWindow::on_pause_button_clicked()
{
    // Check if the game is already paused. If not,
    // pause it, else continue the game. Disable / Enable
    // buttons relating to the pause or continuing. Start / Stop
    // the timer and update the textBrowser message.
    if (is_paused == false)
    {
        is_paused = true;
        ui_->up_button->setDisabled(true);
        ui_->left_button->setDisabled(true);
        ui_->right_button->setDisabled(true);
        ui_->down_button->setDisabled(true);
        ui_->seed_spinbox->setDisabled(true);
        ui_->goal_spinbox->setDisabled(true);
        ui_->pause_button->setText("Continue");
        ui_->textBrowser->setText("Game paused. Press continue"
                                    " to resume the game or reset.");
        timer_->stop();
    }
    else
    {
        is_paused = false;
        ui_->up_button->setEnabled(true);
        ui_->left_button->setEnabled(true);
        ui_->right_button->setEnabled(true);
        ui_->down_button->setEnabled(true);
        ui_->seed_spinbox->setDisabled(true);
        ui_->goal_spinbox->setDisabled(true);
        ui_->pause_button->setText("Pause");
        ui_->textBrowser->setText("Game continued! Good luck!");
        timer_->start(1000);
    }
}

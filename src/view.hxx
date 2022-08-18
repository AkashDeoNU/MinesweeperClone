#pragma once

#include "model.hxx"
#include <iostream>

class View
{
public:
    /// View dimensions will use `int` coordinates.
    using Dimensions = ge211::Dims<int>;

    /// View positions will use `int` coordinates.
    using Position = ge211::Posn<int>;

    // Constructor
    explicit View(Model const& model);

    // Displays sprites on the screen. Called every so often by the draw()
    // function in Controller.
    void draw(ge211::Sprite_set& set);

    // Convert coordinates referring to Positions on the screen or the View
    // to Positions on a Board.
    View::Position board_to_screen(Model::Position);

    // Convert coordinates referring to Positions on a Board to coordinates
    // referring to Positions on the screen or the View.
    // It is the caller's responsibility to ensure the input to the
    // function actually lies on the Board displayed on the View.
    Model::Position screen_to_board(View::Position);

    // Generate the initial dimensions of the window. This is called only
    // once by the game engine. It uses the dimensions of the board in model
    // and a constant cell_size to generate the window dimensions.
    Dimensions initial_window_dimensions() const;

    // Generates the position of the reset button depending on the dimensions
    // of the window.
    Position get_reset_button_position();

    // Generates the position of the time counter depending on the dimensions
    // of the window.
    Position get_time_counter_position();

    // Generates the position of the flag counter depending on the dimensions
    // of the window.
    Position get_flag_counter_position();

private:
    Model const& model_;

    // Background sprite
    ge211::Rectangle_sprite background_;

    // Image sprites for each tile.
    ge211::Image_sprite covered_cell_ {"covered-cell.png"};
    ge211::Image_sprite eight_cell_ {"eight-cell.png"};
    ge211::Image_sprite empty_cell_ {"empty-cell.png"};
    ge211::Image_sprite five_cell_ {"five-cell.png"};
    ge211::Image_sprite flagged_cell_ {"flagged-cell.png"};
    ge211::Image_sprite four_cell_ {"four-cell.png"};
    ge211::Image_sprite mine_cell_ {"mine-cell.png"};
    ge211::Image_sprite one_cell_ {"one-cell.png"};
    ge211::Image_sprite seven_cell_ {"seven-cell.png"};
    ge211::Image_sprite six_cell_ {"six-cell.png"};
    ge211::Image_sprite three_cell_ {"three-cell.png"};
    ge211::Image_sprite two_cell_ {"two-cell.png"};
    ge211::Image_sprite default_smiley_ { "default-smiley.png"};
    ge211::Image_sprite win_smiley_ {"win-smiley.png"};
    ge211::Image_sprite lose_smiley_ {"lose-smiley.png"};

    // Text sprites
    ge211::Font dseg40{"DSEG14ClassicMini-Regular.ttf", 50};
    ge211::Text_sprite flag_counter_;
    ge211::Text_sprite time_counter;
};

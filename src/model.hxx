#pragma once

#include <ge211.hxx>
#include "board.hxx"
#include "cell.hxx"

class Model
{
public:

    // Model dimensions will use `int` coordinates, as board dimensions do.
    using Dimensions = Board::Dimensions;

    // Model positions will use `int` coordinates, as board positions do.
    using Position = Board::Position;

    // This is the default constructor. Creates a board of 16 rows x 30
    // columns. Sets the flag counter to 49. Time is set to 0.0.
    Model();

    // Creates a board with a different number of rows and columns.
    // Everything else is the same as the default constructor.
    Model(int width, int height);

    // Returns the contents of the board.
    std::unordered_map<Model::Position, Cell>
    get_board() const;

    // Returns the dimensions of the board. These dimensions are the same
    // passed into the constructor when initially creating the Model.
    Dimensions get_board_dimensions() const;

    // Uncovers a cell unless the cell is already uncovered, flagged, or the
    // game is over.
    void reveal(Model::Position);

    // Returns whether any cells on the board are uncovered. Utilized for
    // testing only.
    bool are_any_cells_revealed();

    // Flags or un-flags a position on the board unless the game is over.
    void flag(Model::Position);

    // Updates time unless the game hasn't started, or the game is over.
    void on_frame(double dt);

    // Updates state when the user has lost. Uncovers every tile on the
    // board, and sets game_over to true.
    void lose();

    // Updates state when the user has won. It sets game_over to true.
    void win();

    // Updates the flag counter. Manually tallies all flagged cells in board.
    void update_flag_counter();

    // Returns whether the game is over.
    bool is_game_over() const;

    // Returns whether the user won.
    bool did_user_win() const;

    // Returns (99 - c), where c is the number of flagged cells on the board.
    int get_flag_counter() const;

    // Gets the number of minutes passed.
    int get_minutes() const;

    // Gets the number of seconds passed since the beginning of the minute in
    // gameplay. If 1:30 has passed during the game, this function returns
    // the integer 30.
    int get_seconds() const;

#ifdef CS211_TESTING
    // When this class is compiled for testing, members of a struct named
    // Test_access will be allowed to access private members of this class.
    friend struct Test_access;
#endif

private:
    // Keeps track of each cell. Contains a std::unordered_map with every
    // cell on the board.
    Board board;

    // Holds the value (99 - c), where c is the number of flagged cells on the
    // board.
    int flag_counter;

    // Holds the time passed during gameplay.
    double time;

    // Whether the game is over.
    bool game_over;

    // Whether the game has started.
    bool game_started;

    // Whether the user won.
    bool did_you_win;

    // Clear mines on the board
    void clear_mines_on_board();

    // Set mine
    void set_mine(Model::Position pos, bool m);

    // Guarantee adjacent mines
    void guarantee_adjacent_mines();
};

#pragma once

#include <ge211.hxx>
#include <iostream>
#include <unordered_map>
#include "cell.hxx"

class Board
{
public:
    // Type aliases
    // Board dimensions will use `int` coordinates.
    using Dimensions = ge211::Dims<int>;

    // Board positions will use `int` coordinates.
    using Position = ge211::Posn<int>;

    // Default constructor
    Board();

    // Constructs a board with the given dimensions. Puts 49 mines in random
    // places on the board.
    Board(Dimensions dims);

    // Returns an unordered_map containing key-value pair representing a
    // Position and a Cell on the board.
    std::unordered_map<Position, Cell>
    get_board() const;

    // Returns whether the given position is in bounds.
    bool good_position(Position) const;

    // Reveals a position on the board. Returns true if what's revealed is a
    // mine.
    bool reveal(Board::Position);

    // Returns whether any cells on the board are uncovered. For testing only.
    bool are_any_cells_revealed();

    // Flags a position on the board.
    void flag(Board::Position);

    // Uncovers all cells on the board that aren't flagged.
    void uncover_all_besides_flagged();

    // Returns whether the user has uncovered all non-mine cells on the
    // board.
    bool win();

    // Returns the number of flagged cells on the board.
    int get_flag_count();

    // Get dimensions passed into the constructor
    Board::Dimensions dimensions();

    // Clear the board
    void clear_mines_on_board();

    // Set a mine
    void set_mine(Board::Position, bool m);

    // Guarantee adjacent mines
    void guarantee_adjacent_mines();
private:
    // The unordered_map containing all the cells
    std::unordered_map<ge211::Posn<int>, Cell> board_;

    // The dimensions of the Board.
    Dimensions dims_;

    // A helper function for reveal. It handles the recursive part of
    // revealing cells on the board.
    void reveal_helper(Board::Position);

    // Returns the number of mines adjacent to one position on the board.
    size_t mines_adjacent_to_one_pos(Position);

    // Generates an array of valid positions surrounding pos.
    std::vector<Board::Position> surrounding_positions(Board::Position pos);


};

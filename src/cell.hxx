#pragma once

#include <iostream>

class Cell {
public:
    Cell();
    Cell(bool m);

    // Sets covered_ to false.
    void uncover();
    // Returns whether the Cell is covered.
    bool is_covered();
    // Sets flag_ to f.
    void set_flag(bool f);
    // Returns whether the Cell is flagged.
    bool is_flagged();
    // Sets mine_ to m.
    void set_mine(bool m);
    // Returns whether the Cell contains a mine.
    bool is_mine();
    // Sets adjacent_mines_ to num.
    void set_adjacent_mines(size_t num);
    // Returns adjacent_mines_.
    size_t get_adjacent_mines();

    // Whether a cell is "covered" or "revealed" already.
    bool covered_;

    // Whether a cell is "flagged" or not.
    bool flag_;

    // Whether a cell has a mine in it or not.
    bool mine_;

    // The number of mines adjacent to this Cell Ensuring that this is
    // correct is the responsibility of the client.
    size_t adjacent_mines_;
};
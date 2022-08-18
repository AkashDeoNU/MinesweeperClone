#include "board.hxx"

using namespace ge211;

// The number of mines randomly placed on the board.
static const int mine_num = 49;

Board::Board()
        : Board(Board::Dimensions{30, 16})
{ }

Board::Board(Dimensions dims)
        : board_(),
          dims_(dims)
{
    // A vector that will hold all Positions on the board.
    std::vector<Board::Position> all_positions;
    for (int w = 0; w < dims_.width; w++)
    {
        for (int h = 0; h < dims_.height; h++)
        {
            // Generate a default cell for every Position on the Board.
            board_.emplace(Position{w,h}, Cell(false));
            // Add a Position to all_positions.
            all_positions.push_back(Position{w, h});
        }
    }
    // Initialize an unbounded random number generator.
    ge211::Random_source<int> r(ge211::unbounded);
    for (int i = 0; i < mine_num; i++)
    {
        // Generate a random number.
        int rand_num = r(0, 480 - 1 - i);
        // Put a mine in a random position in board_.
        board_[all_positions.at(rand_num)].set_mine(true);
        // Remove that random position from the all_positions vector.
        all_positions.erase(all_positions.begin() + rand_num);
    }
    // Ensure the "adjacent_mines" trait of every Cell is correct.
    guarantee_adjacent_mines();
}


std::unordered_map<Board::Position, Cell>
Board::get_board() const
{
    return board_;
}


// Takes a position and returns whether its out of bounds of the Board or not.
bool
Board::good_position(Board::Position pos) const
{
    return pos.x >= 0 && pos.x <= (dims_.width - 1) &&
            pos.y >= 0 && pos.y <= (dims_.height - 1);
}


std::vector<Board::Position>
Board::surrounding_positions(Board::Position pos)
{
    std::vector<Board::Position> surrounding_positions;
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            if ((x || y) && good_position({pos.x + x, pos.y + y})) {
                surrounding_positions.push_back({pos.x + x, pos.y + y});
            }
        }
    }
    return surrounding_positions;
}

size_t
Board::mines_adjacent_to_one_pos(Board::Position pos)
{
    size_t counter = 0;
    // Obtain all good positions surrounding pos.
    std::vector<Board::Position> candidates = surrounding_positions(pos);
    // Check if there is a mine present in all adjacent positions, and check
    // that the position is valid.
    for (Board::Position candidate : candidates)
    {
        if (good_position(candidate) &&
            board_.at({candidate}).is_mine())
        {
            // Add to the counter if a mine is present in one of the
            // surrounding cells.
            counter++;
        }
    }
    return counter;
}


bool
Board::reveal(Board::Position pos)
{
    // Returns true when the user clicks on an un-flagged cell with a mine.
    // That tells the model the user has lost, pretty much.
    if ((!board_.at(pos).is_flagged()) && board_[pos].is_mine())
    {
        return true;
    }
    else
    {
        // Attempts to reveal the cell.
        reveal_helper(pos);
        return false;
    }
}


void
Board::reveal_helper(Board::Position pos)
{
    // When the caller is attempting to reveal an empty cell, or a flagged
    // cell, take no further action.
    if ((! board_.at(pos).is_covered()) || board_.at(pos).is_flagged()) {
        return;
    }
    // When the caller is revealing a cell adjacent to a mine, reveal only
    // that cell, and take no further action.
    else if (board_.at(pos).get_adjacent_mines() != 0)
    {
        board_[pos].uncover();
    }
    // When the caller is revealing a cell adjacent to no mines, reveal only
    // that cell, and reveal all positions surrounding that cell.
    else if (board_.at(pos).get_adjacent_mines() == 0)
    {
        board_[pos].uncover();
        for (auto p: surrounding_positions(pos))
        {
            reveal(p);
        }
    }
}


bool
Board::are_any_cells_revealed()
{
    for (auto& c: board_)
    {
        if (! c.second.is_covered())
        {
            return true;
        }
    }
    return false;
}


void
Board::flag(Board::Position pos)
{
    // Only (un-)flag a Cell if it is covered.
    if (board_[pos].is_covered())
    {
        // If the Cell at Position pos doesn't have a flag, place a flag on
        // it. If it does have a flag, remove the flag.
        board_[pos].set_flag(! board_[pos].is_flagged());
    }
}


void
Board::uncover_all_besides_flagged()
{
    for (auto& c: board_)
    {
        if (! c.second.is_flagged())
        {
            c.second.uncover();
        }

    }
}


bool
Board::win()
{
    // Checks if any covered cells have mines.
    bool result = true;
    for (auto c : board_)
    {
        if ((! c.second.is_mine()) && c.second.is_covered())
        {
            result = false;
        }
    }
    return result;
}


int
Board::get_flag_count()
{
    int count = 0;
    for (auto p: board_)
    {
        if (p.second.is_flagged())
        {
            count++;
        }
    }

    return count;
}

Board::Dimensions
Board::dimensions()
{
    return dims_;
}

void
Board::clear_mines_on_board()
{
    for (auto& p : board_)
    {
        p.second.set_mine(false);
        p.second.set_adjacent_mines(0);
    }
}

void
Board::set_mine(Board::Position pos, bool m)
{
    board_[pos].set_mine(m);
}

void
Board::guarantee_adjacent_mines()
{
    // Ensure the "adjacent_mines" trait of every Cell is correct.
    for (auto& i : board_)
    {
        // Check that there isn't a mine in the Cell at the position.
        if (! i.second.is_mine())
        {
            // Obtain the number of mines adjacent to the given position.
            size_t num = mines_adjacent_to_one_pos(i.first);
            // Set the number of adjacent mines in this Cell.
            i.second.set_adjacent_mines(num);
        }
    }
}

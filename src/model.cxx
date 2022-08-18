#include "model.hxx"


static const int mine_num = 49;


Model::Model()
        : Model(30, 16)
{ }


Model::Model(int width, int height)
        : board(Board({width, height})),
          flag_counter(mine_num),
          time(0.),
          game_over(false),
          game_started(false),
          did_you_win(false)
{ }


std::unordered_map<Model::Position, Cell>
Model::get_board() const
{
    return board.get_board();
}


Model::Dimensions
Model::get_board_dimensions() const
{
    return {30, 16};
}


void
Model::reveal(Model::Position pos)
{
    // Set game_started to true.
    game_started = true;
    // Double check that pos is on the board.
    if (! board.good_position(pos))
    {
        return;
    }
    // If the game isn't over, attempt to reveal cells.
    if (! game_over)
    {
        // If you reveal a mine, you lose.
        if (board.reveal(pos))
        {
            lose();
        }
        // If you reveal all the non-mine cells, you win.
        else if (board.win())
        {
            win();
        }
    }
}


bool
Model::are_any_cells_revealed()
{
    return board.are_any_cells_revealed();
}


void
Model::flag(Model::Position pos)
{
    // Double check pos is on the board
    if (! board.good_position(pos))
    {
        return;
    }
    // As long as the game isn't over, try to flag a cell at Position pos.
    if (! game_over)
    {
        // Set the flag.
        board.flag(pos);
        // Update the flag counter.
        update_flag_counter();
    }
}

void
Model::on_frame(double dt)
{
    // Only add to time if the game has started, and the game isn't over.
    if (game_started && (! game_over))
    {
        time += dt;
    }
}

void
Model::lose()
{
    // Uncover every tile on the board.
    board.uncover_all_besides_flagged();
    // Set game_over to true.
    game_over = true;
}

void
Model::win()
{
    // Set game_over to true.
    game_over = true;
    // Set did_you_win to true.
    did_you_win = true;
}

void
Model::update_flag_counter()
{
    flag_counter = mine_num - board.get_flag_count();
}

bool
Model::is_game_over() const
{
    return game_over;
}

bool
Model::did_user_win() const
{
    return did_you_win;
}

int
Model::get_flag_counter() const
{
    return flag_counter;
}

int
Model::get_minutes() const
{
    return ((int) time) / 60;
}

int
Model::get_seconds() const
{
    return ((int) time) % 60;
}

void
Model::clear_mines_on_board()
{
    board.clear_mines_on_board();
}

void
Model::set_mine(Model::Position pos, bool m)
{
    board.set_mine(pos, m);
}

void
Model::guarantee_adjacent_mines()
{
    board.guarantee_adjacent_mines();
}



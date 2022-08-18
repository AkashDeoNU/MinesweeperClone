#include "view.hxx"

// Constants
static int const cell_size = 32;
static ge211::Color const background_color {128, 128, 128};


View::View(Model const& model)
        : model_(model),
          background_{initial_window_dimensions(), background_color}
{ }


void
View::draw(ge211::Sprite_set& set)
{
    // Set the background.
    set.add_sprite(background_, {0, 0}, 0);
    // Get the board from the Model.
    std::unordered_map<Model::Position, Cell> board = model_.get_board();
    // Draw every cell.
    for (auto p : board)
    {
        if (p.second.is_covered())
        {
            if (p.second.is_flagged())
            {
                set.add_sprite(flagged_cell_,
                               board_to_screen(p.first),
                               1);
            }
            else
            {
                set.add_sprite(covered_cell_,
                               board_to_screen(p.first),
                               1);
            }
        }
        else if (p.second.is_mine())
        {
            set.add_sprite(mine_cell_,
                           board_to_screen(p.first),
                           1);
        }
        else if (p.second.get_adjacent_mines() == 0)
        {
            set.add_sprite(empty_cell_,
                           board_to_screen(p.first),
                           1);
        }
        else if (p.second.get_adjacent_mines() == 1)
        {
            set.add_sprite(one_cell_,
                           board_to_screen(p.first),
                           1);
        }
        else if (p.second.get_adjacent_mines() == 2)
        {
            set.add_sprite(two_cell_,
                           board_to_screen(p.first),
                           1);
        }
        else if (p.second.get_adjacent_mines() == 3)
        {
            set.add_sprite(three_cell_,
                           board_to_screen(p.first),
                           1);
        }
        else if (p.second.get_adjacent_mines() == 4)
        {
            set.add_sprite(four_cell_,
                           board_to_screen(p.first),
                           1);
        }
        else if (p.second.get_adjacent_mines() == 5)
        {
            set.add_sprite(five_cell_,
                           board_to_screen(p.first),
                           1);
        }
        else if (p.second.get_adjacent_mines() == 6)
        {
            set.add_sprite(six_cell_,
                           board_to_screen(p.first),
                           1);
        }
        else if (p.second.get_adjacent_mines() == 7)
        {
            set.add_sprite(seven_cell_,
                           board_to_screen(p.first),
                           1);
        }
        else if (p.second.get_adjacent_mines() == 8)
        {
            set.add_sprite(eight_cell_,
                           board_to_screen(p.first),
                           1);
        }

        // Draw the reset button depending on game state.
        if (! model_.is_game_over())
        {
            set.add_sprite(default_smiley_,
                           get_reset_button_position(),
                           1);
        }
        else if (model_.did_user_win())
        {
            set.add_sprite(win_smiley_,
                           get_reset_button_position(),
                           1);
        }
        else if (! model_.did_user_win())
        {
            set.add_sprite(lose_smiley_,
                           get_reset_button_position(),
                           1);
        }
        // Generate and draw the Flag Counter text sprite.
        ge211::Text_sprite::Builder flag_builder(dseg40);
        flag_builder << model_.get_flag_counter();
        flag_counter_.reconfigure(flag_builder);
        set.add_sprite(flag_counter_, get_flag_counter_position(), 3);
        // Generate and draw the Time Counter text sprite.
        ge211::Text_sprite::Builder time_builder(dseg40);
        time_builder << model_.get_minutes() << ":" << model_.get_seconds();
        time_counter.reconfigure(time_builder);
        set.add_sprite(time_counter, get_time_counter_position(), 3);
    }
}


View::Dimensions
View::initial_window_dimensions() const
{
    return (cell_size * model_.get_board_dimensions()) + Dimensions{0, 80};
}


View::Position
View::board_to_screen(Model::Position logical)
{
    return View::Position{2+(logical.x*cell_size),
                          2+(logical.y*cell_size)};
}


Model::Position
View::screen_to_board(View::Position physical)
{
    return Model::Position{(physical.x-2)/cell_size,
                           (physical.y-2)/cell_size};
}


View::Position
View::get_reset_button_position()
{
    Model::Dimensions board_dims = model_.get_board_dimensions();
    int x = initial_window_dimensions().width/2 - 30;
    int y = (2 + (board_dims.height)*cell_size) + 10;
    return Position{x, y};
}


View::Position
View::get_time_counter_position()
{
    Dimensions dims = time_counter.dimensions();
    return View::Position{3*initial_window_dimensions().width/4 - dims.width/2,
                       get_reset_button_position().y};
}


View::Position
View::get_flag_counter_position()
{
    Dimensions dims = flag_counter_.dimensions();
    return View::Position{(1*initial_window_dimensions().width/4) -
                    dims.width/2,get_reset_button_position().y};
}
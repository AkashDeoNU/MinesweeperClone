#include "controller.hxx"

Controller::Controller()
        : view_(model_),
          mouse_screen_pos(View::Position{0,0})
{ }


void
Controller::draw(ge211::Sprite_set& set)
{
    view_.draw(set);
}


View::Dimensions
Controller::initial_window_dimensions() const
{
    return view_.initial_window_dimensions();
}


void
Controller::on_mouse_move(ge211::Posn<int> pos)
{
    mouse_screen_pos = pos;
}


void
Controller::on_mouse_up(ge211::Mouse_button m, ge211::Posn<int> pos)
{
    mouse_screen_pos = pos;
    // Convert the mouse position on the screen to coordinates on the board.
    Model::Position mouse_board_pos = view_.screen_to_board(mouse_screen_pos);
    // If the Mouse_button passed into the function is the left button,
    // reveal a cell on the board, or reset the game.
    if (m == ge211::Mouse_button::left)
    {
        // Even if the user clicks off the board, it does not throw an
        // exception or generate an error. The reveal function in the model
        // checks that the input is a good position.
        model_.reveal(mouse_board_pos);

        // If the user clicks the reset button, set the model to its defaults.
        View::Position top_left = view_.get_reset_button_position();
        View::Position bottom_right = top_left + View::Dimensions{60, 60};
        if (mouse_screen_pos.x >= top_left.x && mouse_screen_pos.x <=
        bottom_right.x && mouse_screen_pos.y >= top_left.y &&
        mouse_screen_pos.y <= bottom_right.y)
        {
            model_ = Model();
        }
    }
    // If the Mouse_button passed into the function is the right button, flag
    // a cell on the board.
    else if (m == ge211::Mouse_button::right)
    {
        // Even if the given position is not on the board, the flag function
        // checks that mouse_board_pos is a good position.
        model_.flag(mouse_board_pos);
    }
}


void
Controller::on_frame(double dt)
{
    model_.on_frame(dt);
}

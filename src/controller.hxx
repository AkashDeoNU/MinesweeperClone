#pragma once

#include "model.hxx"
#include "view.hxx"

#include <ge211.hxx>

class Controller : public ge211::Abstract_game
{
public:
    Controller();

protected:
    // Functions that inherit from Abstract_game. They set up the View.
    void draw(ge211::Sprite_set& set) override;
    View::Dimensions initial_window_dimensions() const override;

    // These functions are called when particular events happen.
    // When the mouse moves, the variable mouse_screen_pos is updated.
    void on_mouse_move(ge211::Posn<int> pos) override;
    // When the user left-clicks a cell on the board, it reveals the cell.
    // When the user left-clicks the reset button, model_ is set to its
    // defaults.
    // When the user right-clicks a cell on the board, it flags the cell.
    void on_mouse_up(ge211::Mouse_button, ge211::Posn<int> pos) override;

    // The game engine calls this function ever 1/60th of a second. It
    // updates the counter that keeps track of time in the model.
    void on_frame(double dt) override;

private:
    Model model_;
    View view_;

    // A variable used in Controller and View that keeps track of the mouse's
    // location on the screen.
    View::Position mouse_screen_pos;
};

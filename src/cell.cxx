#include "cell.hxx"

Cell::Cell()
        : Cell(false)
{}


Cell::Cell(bool m)
        : covered_(true),
          flag_(false),
          mine_(m),
          adjacent_mines_(false)
{}


void
Cell::uncover()
{
    covered_ = false;
    flag_ = false;
}


bool
Cell::is_covered()
{
    return covered_;
}


void
Cell::set_flag(bool f)
{
    flag_ = f;
}


bool
Cell::is_flagged()
{
    return flag_;
}


void
Cell::set_mine(bool m)
{
    mine_ = m;
}


bool
Cell::is_mine()
{
    return mine_;
}


void
Cell::set_adjacent_mines(size_t num)
{
    adjacent_mines_ = num;
}


size_t
Cell::get_adjacent_mines()
{
    return adjacent_mines_;
}

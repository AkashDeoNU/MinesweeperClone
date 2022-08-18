#include "model.hxx"
#include <catch.hxx>
#include <iostream>


///
/// Test_access struct
///

struct Test_access
{
    Model& model;

    // Constructs a `Test_access` with a reference to the Model under test.
    explicit Test_access(Model&);

    // Gets the unordered map in Board.
    std::unordered_map<Model::Position, Cell> get_board();

    // Gets the dimensions of the board.
    Model::Dimensions get_dimensions();

    // Clear mines on the board
    void clear_mines_on_board();

    // Set mine
    void set_mine(Model::Position pos, bool m);

    // Guarantee Adjacent Mines
    void guarantee_adjacent_mines();
};


Test_access::Test_access(Model& model)
        : model(model)
{ }


std::unordered_map<Model::Position, Cell>
Test_access::get_board()
{
    return model.get_board();
}


Model::Dimensions
Test_access::get_dimensions()
{
    return model.board.dimensions();
}

void
Test_access::clear_mines_on_board()
{
    model.clear_mines_on_board();
}

void
Test_access::set_mine(Model::Position pos, bool m)
{
    model.set_mine(pos, m);
}

void
Test_access::guarantee_adjacent_mines()
{
    model.guarantee_adjacent_mines();
}

///
/// Test cases
///

//
// These tests demonstrate my functional requirements.
//

// Favorite Tests

// 1. Test there are 49 mines randomly scattered across the board.
TEST_CASE("Test there are 49 mines randomly scattered across the board.")
{
    Model m;
    Test_access access(m);
    std::unordered_map<Model::Position, Cell> board = access.get_board();

    // Test that there are 49 mines.
    int counter = 0;
    for (auto& pos: board)
    {
        if (pos.second.is_mine())
        {
            counter++;
        }
    }
    CHECK(counter == 49);

    // Test that the mines are generated randomly.
    // Generate a second model, and spot check that the first and second
    // boards are at all different. (There is the off chance this test
    // generates an error instead of going through okay, which just proves
    // the Model is random as intended.
    Model m2;
    Test_access access2(m2);

    bool track = false;

    // Boards
    std::unordered_map<Model::Position, Cell> board1 = access.get_board();
    std::unordered_map<Model::Position, Cell> board2 = access2.get_board();

    for (int w = 0; w < access.get_dimensions().width; w++)
    {
        for (int h = 0; h < access.get_dimensions().height; h++)
        {
            if (board1[Model::Position{w, h}].is_mine() !=
                board2[Model::Position{w, h}].is_mine())
            {
                track = true;
                break;
            }
        }
    }

    CHECK(track == true);
}

// 2. Reveal mechanics.
TEST_CASE("Reveal mechanics")
{
    Model m;
    Test_access access(m);

    // Generate a hollow square of mined cells.
    access.clear_mines_on_board();

    std::vector<Model::Position> positions;

    // 5x5 hollow square, with the top left cell at {3,3}
    positions.push_back(Model::Position{3+0,3+0});
    positions.push_back(Model::Position{3+1,3+0});
    positions.push_back(Model::Position{3+2,3+0});
    positions.push_back(Model::Position{3+3,3+0});
    positions.push_back(Model::Position{3+4,3+0});

    positions.push_back(Model::Position{3+0,3+1});
    positions.push_back(Model::Position{3+0,3+2});
    positions.push_back(Model::Position{3+0,3+3});
    positions.push_back(Model::Position{3+0,3+4});

    positions.push_back(Model::Position{3+4,3+1});
    positions.push_back(Model::Position{3+4,3+2});
    positions.push_back(Model::Position{3+4,3+3});
    positions.push_back(Model::Position{3+4,3+4});

    positions.push_back(Model::Position{3+1,3+4});
    positions.push_back(Model::Position{3+2,3+4});
    positions.push_back(Model::Position{3+3,3+4});

    for (Model::Position pos : positions)
    {
        access.set_mine(pos, true);
    }

    // Ensuring that adjacent_mines holds true
    access.guarantee_adjacent_mines();

    // Try to reveal something off the screen.
    m.reveal(Model::Position{5000, 5000});
    // Check that it does not exist on the board.
    bool present = false;
    for (auto p: access.get_board())
    {
        if (p.first == Model::Position{5000, 5000})
        {
            present = true;
        }
    }
    CHECK(! present);

    // Reveal a cell next to one of the mines from within the square.
    m.reveal(Model::Position{6, 5});
    // Check that is the only mine revealed on the board.
    std::vector<Model::Position> revealed_cells;
    for (auto p: access.get_board())
    {
        if (! p.second.is_covered())
        {
            revealed_cells.push_back(p.first);
        }
    }
    CHECK(revealed_cells.size() == 1);
    CHECK(std::count(revealed_cells.begin(), revealed_cells.end(),
                     Model::Position{6, 5}));

    // Reveal the cell right in the middle of the hollow square.
    m.reveal(Model::Position{5,5});
    // Check the mines revealed on the board are the ones in the middle of
    // the hollow square.
    revealed_cells.clear();
    for (auto p: access.get_board())
    {
        if (! p.second.is_covered())
        {
            revealed_cells.push_back(p.first);
        }
    }
    CHECK(revealed_cells.size() == 9);
    // Build all the relevant cells
    std::vector<Model::Position> relevant_positions;
    for (int x = 4; x <= 6; x++)
    {
        for (int y = 4; y <= 6; y++)
        {
            relevant_positions.push_back(Model::Position{x, y});
        }
    }
    // Check all the relevant cells
    for (Model::Position pos : relevant_positions)
    {
        CHECK(std::count(revealed_cells.begin(), revealed_cells.end(), pos));
    }

    // Reveal a cell outside the hollow square.
    m.reveal(Model::Position{28, 14});

    // Check that all are revealed except for the mine squares
    revealed_cells.clear();
    for (auto p: access.get_board())
    {
        if (! p.second.is_covered())
        {
            revealed_cells.push_back(p.first);
        }
    }
    CHECK(revealed_cells.size() == (16*30) - 16);
    // Check none of the positions are present in revealed_cells
    for (Model::Position p : positions)
    {
        CHECK(! std::count(revealed_cells.begin(), revealed_cells.end(), p));
    }

    // Check the user has won, and the game is over.
    CHECK(m.is_game_over());
    CHECK(m.did_user_win());
}

// 3. Flag mechanics
TEST_CASE("Flag Mechanics")
{
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    access.set_mine(Model::Position{2,1}, true);
    access.guarantee_adjacent_mines();

    // Place a flag.
    m.flag(Model::Position{8, 8});
    CHECK(m.get_flag_counter() == 48);
    CHECK(access.get_board()[Model::Position{8,8}].is_flagged());

    // Place a second flag.
    m.flag(Model::Position{8,9});
    CHECK(m.get_flag_counter() == 47);
    CHECK(access.get_board()[Model::Position{8,9}].is_flagged());

    // Reveal a non-mine cell, adjacent to a mine.
    m.reveal(Model::Position{2,0});
    // Attempt to flag the above cell.
    m.flag(Model::Position{2,0});
    // Check the counter stays at 47, AND the flag has not been successfully
    // placed
    CHECK(m.get_flag_counter() == 47);
    CHECK(! access.get_board()[Model::Position{2,0}].is_flagged());

    // Place another flag and ensure the counter is at 46.
    m.flag(Model::Position{8, 10});
    CHECK(m.get_flag_counter() == 46);
    CHECK(access.get_board()[Model::Position{8,10}].is_flagged());

    // Try to place a flag at an invalid position.
    m.flag(Model::Position{50, 50});
    CHECK(m.get_flag_counter() == 46);
    // Check the invalid position is not on the board.
    std::unordered_map<Model::Position, Cell> board = access.get_board();
    bool counter = false;
    for (auto p: board)
    {
        if (p.first == Model::Position{50, 50})
        {
            counter = true;
        }
    }
    CHECK(! counter);
}

// 4. Lose scenario.
TEST_CASE("Lose scenario")
{
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();

    // Set up five mines. {1, 8}, {3,0}, {8,8}, {10, 3}, {29, 15}.
    access.set_mine(Model::Position{1,8}, true);
    access.set_mine(Model::Position{3,0}, true);
    access.set_mine(Model::Position{8,8}, true);
    access.set_mine(Model::Position{10,3}, true);
    access.set_mine(Model::Position{29,15}, true);
    access.guarantee_adjacent_mines();

    // Flag and un-flag positions. {0, 0}, {29, 15}, {8,8}.
    m.flag(Model::Position{0,0});
    CHECK(m.get_flag_counter() == 48);
    CHECK(access.get_board()[Model::Position{0,0}].is_flagged());
    m.flag(Model::Position{0,0});
    CHECK(m.get_flag_counter() == 49);
    CHECK(! access.get_board()[Model::Position{29, 15}].is_flagged());

    // Flag position {29,15}
    m.flag(Model::Position{29,15});
    CHECK(m.get_flag_counter() == 48);
    CHECK(access.get_board()[Model::Position{29,15}].is_flagged());

    // Flag and un-flag {8,8}
    m.flag(Model::Position{8, 8});
    CHECK(m.get_flag_counter() == 47);
    CHECK(access.get_board()[Model::Position{8,8}].is_flagged());
    m.flag(Model::Position{8,8});
    CHECK(m.get_flag_counter() == 48);
    CHECK(! access.get_board()[Model::Position{8,8}].is_flagged());

    // Un-flag {29, 15}
    m.flag(Model::Position{29,15});
    CHECK(m.get_flag_counter() == 49);
    CHECK(! access.get_board()[Model::Position{0,0}].is_flagged());

    // Reveal {6,8}.
    m.reveal(Model::Position{6,8});

    // Test that everything is revealed except for the mine positions, and
    // {0, 8}
    std::vector<Model::Position> covered_positions;
    for (auto p: access.get_board())
    {
        if (p.second.is_covered())
        {
            covered_positions.push_back(p.first);
        }
    }
    CHECK(covered_positions.size() == 6);
    // Test the contents of covered_positions
    CHECK(std::count(covered_positions.begin(), covered_positions.end(),
                     Model::Position{1,8}));
    CHECK(std::count(covered_positions.begin(), covered_positions.end(),
                     Model::Position{3,0}));
    CHECK(std::count(covered_positions.begin(), covered_positions.end(),
                     Model::Position{8,8}));
    CHECK(std::count(covered_positions.begin(), covered_positions.end(),
                     Model::Position{10,3}));
    CHECK(std::count(covered_positions.begin(), covered_positions.end(),
                     Model::Position{29,15}));
    CHECK(std::count(covered_positions.begin(), covered_positions.end(),
                     Model::Position{0,8}));

    // Reveal {3,0}.
    m.reveal(Model::Position{3,0});

    // Test game_over, everything is revealed, and user lost
    CHECK(m.is_game_over());
    CHECK(! m.did_user_win());
    bool counter = true;
    for (auto p: access.get_board())
    {
        if (p.second.is_covered())
        {
            counter = false;
        }
    }
    CHECK(counter);
}

// 5. Win scenario
TEST_CASE("Win scenario")
{
    // Set up five mines. {1,8}, {3,0}, {8,8}, {10,3}, {28, 14}.
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();

    // Set up five mines. {1, 8}, {3,0}, {8,8}, {10, 3}, {29, 15}.
    access.set_mine(Model::Position{1,8}, true);
    access.set_mine(Model::Position{3,0}, true);
    access.set_mine(Model::Position{8,8}, true);
    access.set_mine(Model::Position{10,3}, true);
    access.set_mine(Model::Position{28,14}, true);
    access.guarantee_adjacent_mines();

    // Reveal {6,8}.
    m.reveal(Model::Position{6,8});

    // Test that everything is revealed except for the mine positions, and
    // {0,8}, {29,14}, {28, 15}, {29, 15}.
    std::vector<Model::Position> covered_positions;
    for (auto p: access.get_board())
    {
        if (p.second.is_covered())
        {
            covered_positions.push_back(p.first);
        }
    }
    CHECK(covered_positions.size() == 9);
    std::vector<Model::Position> positions{Model::Position{1,8},
                                           Model::Position{3,0},
                                           Model::Position{8,8},
                                           Model::Position{10,3},
                                           Model::Position{28,14},
                                           Model::Position{0,8},
                                           Model::Position{29, 14},
                                           Model::Position{28, 15},
                                           Model::Position{29, 15}};

    for (Model::Position pos : positions)
    {
        CHECK(std::count(covered_positions.begin(), covered_positions.end(),
                         pos));
    }

    // Reveal each of the non-mine positions.
    m.reveal(Model::Position{0,8});
    CHECK(! access.get_board()[Model::Position{0, 8}].is_covered());
    m.reveal(Model::Position{29,14});
    CHECK(! access.get_board()[Model::Position{29, 14}].is_covered());
    m.reveal(Model::Position{28, 15});
    CHECK(! access.get_board()[Model::Position{28, 15}].is_covered());
    m.reveal(Model::Position{29, 15});
    CHECK(! access.get_board()[Model::Position{29, 15}].is_covered());

    // Check that the game is over and the user has won
    CHECK(m.is_game_over());
    CHECK(m.did_user_win());
}

// Elementary tests
TEST_CASE("Test Cell")
{
    Cell c(true);
    CHECK(c.is_mine());
}

TEST_CASE("Test Model")
{
    Model model_;
    CHECK(! model_.are_any_cells_revealed());
}

// Test Specification #1.
// Test that there are 49 mines randomly scattered across the board.
// Check Favorite Test #1.

// Test Specification #2.
// Check that revealing a cell with a mine, "ends the game." Game_over should
// be true. Did_you_win should be false. And, everything but the flagged
// cells should be uncovered.
TEST_CASE("Reveal a cell with the only mine on the board, and lose the game.")
{
    Model m;
    Test_access access(m);

    // Setting up the board.
    access.clear_mines_on_board();
    // Inserting one mine.
    access.set_mine(Model::Position{2, 2}, true);
    // Flag a few cells.
    m.flag(Model::Position{1, 1});
    m.flag(Model::Position{3,3,});

    // Reveal the only mine on the board
    m.reveal(Model::Position{2,2});

    // Check conditions.
    CHECK(m.is_game_over() == true);
    CHECK(m.did_user_win() == false);
    // Check the un-flagged cells.
    // Get the board.
    std::unordered_map<Model::Position, Cell> board = m.get_board();
    // Check that the only covered cells are the ones flagged.
    std::vector<Model::Position> vec;
    for (auto p : board)
    {
        if (p.second.is_covered())
        {
            vec.push_back(p.first);
        }
    }
    CHECK(vec.size() == 2);
    // Checks that Position{1,1} is in vec.
    // https://www.cplusplus.com/reference/algorithm/count/
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{1,1}));
    // Checks that Position{3,3} is in vec.
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{3,3}));
    CHECK(vec.size() == 2);
}

// Check that there are no errors with clicking a mine on the edge.
TEST_CASE("Reveal a mine on the edge")
{
    // Setup.
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    access.set_mine(Model::Position{0,1}, true);
    access.set_mine(Model::Position{0,0}, true);

    // Reveal the edge cell.
    m.reveal(Model::Position{0,1});
    CHECK(m.is_game_over());
    CHECK(! m.did_user_win());
    // Get the board.
    std::unordered_map<Model::Position, Cell> board = m.get_board();
    std::vector<Model::Position> vec;
    for (auto p : board)
    {
        if (p.second.is_covered())
        {
            vec.push_back(p.first);
        }
    }
    CHECK(vec.empty());
    CHECK(board[Model::Position{0,0}].is_mine());
    CHECK(board[Model::Position{0,1}].is_mine());

}
// Check that there are no errors with clicking a mine in a corner.
TEST_CASE("Reveal a mine on the corner")
{
    // Setup.
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    access.set_mine(Model::Position{0,1}, true);
    access.set_mine(Model::Position{0,0}, true);

    // Reveal the edge cell.
    m.reveal(Model::Position{0,0});
    CHECK(m.is_game_over());
    CHECK(! m.did_user_win());
    // Get the board.
    std::unordered_map<Model::Position, Cell> board = m.get_board();
    std::vector<Model::Position> vec;
    for (auto p : board)
    {
        if (p.second.is_covered())
        {
            vec.push_back(p.first);
        }
    }
    CHECK(vec.empty());
    CHECK(board[Model::Position{0,1}].is_mine());
    CHECK(board[Model::Position{0,0}].is_mine());

}
// Check that revealing an invalid position doesn't obliterate the game.
TEST_CASE("Reveal an invalid position")
{
    // Setup
    Model m;

    m.reveal(Model::Position{10000, 10000});

    std::unordered_map<Model::Position, Cell> board = m.get_board();
    bool present = false;
    for (auto p : board)
    {
        if (p.first == Model::Position{10000, 10000})
        {
            present = true;
        }
    }
    CHECK(! present);
}

// Test Specification #3.
// Check that revealing a cell adjacent to a mine only reveals that cell, and
// no others. Check that the tally of the mines adjacent to it is correct.
TEST_CASE("Test revealing a cell adjacent to a mine.")
{
    // Setup.
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();

    access.set_mine(Model::Position{1,1}, true);
    access.set_mine(Model::Position{2,1}, true);

    // Ensure adjacent mines
    access.guarantee_adjacent_mines();

    // Reveal the cell.
    m.reveal(Model::Position{1,0});
    // Check that only this cell is revealed.
    // Create a vector storing the Positions of the Cells that are uncovered.
    std::vector<Model::Position> vec;
    // Get the board.
    std::unordered_map<Model::Position, Cell> board = m.get_board();
    // Cycle through the board
    for (auto p : board)
    {
        if (! p.second.is_covered())
        {
            vec.push_back(p.first);
        }
    }
    // Check the size of vec is 1.
    CHECK(vec.size() == 1);
    // Check vec contains Position{1,0}
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{1,0}));
    // Check the tally of the mines adjacent to the Position is correct.
    CHECK(board[Model::Position{1,0}].get_adjacent_mines() == 2);
}

// Check one mine adjacent.
TEST_CASE("One mine adjacent")
{
    // Setup
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    access.set_mine(Model::Position{1,1}, true);
    access.guarantee_adjacent_mines();

    // Reveal the cell
    m.reveal(Model::Position{2,1});

    // Check that only this cell is revealed.
    // Create a vector storing the Positions of the Cells that are uncovered.
    std::vector<Model::Position> vec;
    // Get the board.
    std::unordered_map<Model::Position, Cell> board = m.get_board();
    // Cycle through the board
    for (auto p : board)
    {
        if (! p.second.is_covered())
        {
            vec.push_back(p.first);
        }
    }
    // Check the size of vec is 1.
    CHECK(vec.size() == 1);
    // Check vec contains Position{2,1}
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{2,1}));
    // Check the tally of the mines adjacent to the Position is correct.
    CHECK(board[Model::Position{2,1}].get_adjacent_mines() == 1);
}
// Check 8 mines adjacent.
TEST_CASE("Check 8 mines adjacent")
{
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    // Set a mine in 8 places around 1,1
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            if (x || y)
            {
                access.set_mine(Model::Position{1 + x, 1 + y}, true);
            }
        }
    }
    // Ensure adjacent mines is met
    access.guarantee_adjacent_mines();

    // Reveal the relevant cell, in this case {1,1}
    m.reveal(Model::Position{1,1});

    // Check that only this cell is revealed.
    // Create a vector storing the Positions of the Cells that are uncovered.
    std::vector<Model::Position> vec;
    // Get the board.
    std::unordered_map<Model::Position, Cell> board = m.get_board();
    // Cycle through the board
    for (auto p : board)
    {
        if (! p.second.is_covered())
        {
            vec.push_back(p.first);
        }
    }
    // Check the size of vec is 1.
    CHECK(vec.size() == 1);
    // Check vec contains Position{1,1}
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{1,1}));
    // Check the tally of the mines adjacent to the Position is correct.
    CHECK(board[Model::Position{1,1}].get_adjacent_mines() == 8);
}
// Check revealing a cell on the edge, or in a corner produces no errors.
TEST_CASE("Revealing a cell on the edge, or in a corner produces no errors.")
{
    // Set up
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    access.set_mine(Model::Position{1,0}, true);
    access.set_mine(Model::Position{1,1}, true);
    access.guarantee_adjacent_mines();

    // Reveal edge and corner cell.
    m.reveal(Model::Position{0,0});
    m.reveal(Model::Position{0,1});

    // Check that only two cells is revealed.
    // Create a vector storing the Positions of the Cells that are uncovered.
    std::vector<Model::Position> vec;
    // Get the board.
    std::unordered_map<Model::Position, Cell> board = m.get_board();
    // Cycle through the board
    for (auto p : board)
    {
        if (! p.second.is_covered())
        {
            vec.push_back(p.first);
        }
    }
    // Check the size of vec is 1.
    CHECK(vec.size() == 2);
    // Check vec contains Position{0,0}
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{0,0}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{0,1}));
    // Check the tally of the mines adjacent to the Position is correct.
    CHECK(board[Model::Position{0,0}].get_adjacent_mines() == 2);
    CHECK(board[Model::Position{0,1}].get_adjacent_mines() == 2);
}

// Test Specification #4.
// Reveal a cell with only the surrounding eight cells unoccupied by mines.
TEST_CASE("First recursive reveal test")
{
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();

    std::vector<Model::Position> positions;

    positions.push_back(Model::Position{0,0});
    positions.push_back(Model::Position{1,0});
    positions.push_back(Model::Position{2,0});
    positions.push_back(Model::Position{3,0});
    positions.push_back(Model::Position{4,0});

    positions.push_back(Model::Position{0,1});
    positions.push_back(Model::Position{0,2});
    positions.push_back(Model::Position{0,3});
    positions.push_back(Model::Position{0,4});

    positions.push_back(Model::Position{4,1});
    positions.push_back(Model::Position{4,2});
    positions.push_back(Model::Position{4,3});
    positions.push_back(Model::Position{4,4});

    positions.push_back(Model::Position{1,4});
    positions.push_back(Model::Position{2,4});
    positions.push_back(Model::Position{3,4});

    for (Model::Position pos : positions)
    {
        access.set_mine(pos, true);
    }

    access.guarantee_adjacent_mines();

    // Reveal the individual square in the middle.
    m.reveal(Model::Position{2,2});

    // Check the following.
    // Create a vector storing the Positions of the Cells that are uncovered.
    std::vector<Model::Position> vec;
    // Get the board.
    std::unordered_map<Model::Position, Cell> board = m.get_board();
    // Cycle through the board
    for (auto p : board)
    {
        if (! p.second.is_covered())
        {
            vec.push_back(p.first);
        }
    }
    // Check the size of vec is 1.
    CHECK(vec.size() == 9);
    // Check vec contains Position{0,0}
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{1,1}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{2,1}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{3,1}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{1,2}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{2,2}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{3,2}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{1,3}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{2,3}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{3,3}));
}
// Reveal a cell with a mine two cells away, and the rest have no real
// restrictions.
TEST_CASE("Recursive reveal with two mines on the board")
{
    // This doesn't result in an automatic win, because of the placement of
    // one of the mines.

    Model m;
    Test_access access(m);
    access.clear_mines_on_board();

    access.set_mine(Model::Position{2, 1}, true);
    access.set_mine(Model::Position{29, 0}, true);

    access.guarantee_adjacent_mines();

    // Reveal any miscellaneous Position.
    m.reveal(Model::Position{8, 8});

    // Check the following.
    // Create a vector storing the Positions of the Cells that are covered.
    std::vector<Model::Position> vec;
    // Get the board.
    std::unordered_map<Model::Position, Cell> board = m.get_board();
    // Cycle through the board
    for (auto p : board)
    {
        if (p.second.is_covered())
        {
            vec.push_back(p.first);
        }
    }
    // Check the size of vec is 3.
    CHECK(vec.size() == 3);
    // Check vec contains Position{0,0}
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{2,0}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{2,1}));
    CHECK(std::count(vec.begin(), vec.end(), Model::Position{29,0}));
}

// Test Specification #5.
// Try to reveal a flagged cell.
TEST_CASE("Reveal a flagged cell")
{
    // Setup
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    access.set_mine(Model::Position{0,0}, true);
    access.guarantee_adjacent_mines();

    // Flag {0,0}
    m.flag(Model::Position{0,0});

    // Reveal the flagged cell.
    m.reveal(Model::Position{0,0});

    // CHECK the flagged cell is not revealed.
    std::unordered_map<Model::Position, Cell> board = access.get_board();
    CHECK(board[Model::Position{0,0}].is_covered());
}
// Try to reveal an uncovered cell.
TEST_CASE("Reveal an uncovered cell")
{
    // Setup
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    access.set_mine(Model::Position{2,1}, true);
    access.guarantee_adjacent_mines();
    // Reveal all the cells
    m.reveal(Model::Position{7, 10});
    // Double check that the game isn't over.
    CHECK(! m.is_game_over());
    // Try to reveal an uncovered cell
    m.reveal(Model::Position{8,8});

    // CHECK
    std::unordered_map<Model::Position, Cell> board = access.get_board();
    CHECK(! board[Model::Position{8,8}].is_mine());
    CHECK(board[Model::Position{8,8}].get_adjacent_mines() == 0);
    CHECK(! board[Model::Position{8,8}].is_covered());
    CHECK(! board[Model::Position{8,8}].is_flagged());
}

// Test Specification #6.
// Test the flag() function. Try and flag a cell.
// Then un-flag the cell.
// Try and (un)flag() a revealed cell.
TEST_CASE("Flag, and un-flag a cell. Flag a revealed cell.")
{
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    access.set_mine(Model::Position{2,1}, true);
    access.guarantee_adjacent_mines();

    m.flag(Model::Position{0,0});
    // Check that it's actually flagged
    std::unordered_map<Model::Position, Cell> board = access.get_board();
    CHECK(board[Model::Position{0,0}].is_flagged());

    // Check that it's actually un-flagged
    m.flag(Model::Position{0,0});
    // Check
    board = access.get_board();
    CHECK(! board[Model::Position{0,0}].is_flagged());

    // Reveal
    m.reveal(Model::Position{8,8});
    // Try and flag a revealed cell.
    m.flag(Model::Position{8,8});
    // CHECK
    board = access.get_board();
    CHECK(! board[Model::Position{8,8}].is_mine());
    CHECK(board[Model::Position{8,8}].get_adjacent_mines() == 0);
    CHECK(! board[Model::Position{8,8}].is_covered());
    CHECK(! board[Model::Position{8,8}].is_flagged());
}

// Test Specification #7.
// Test that the reset button works as designed. In other words, that the
// constructor effectively restores the characteristics/traits of the model
// to their defaults.
TEST_CASE("Reset button")
{
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    access.set_mine(Model::Position{0,0}, true);
    access.guarantee_adjacent_mines();
    m.reveal(Model::Position{0,0});
    m.flag(Model::Position{1, 0});

    // Test that
    m = Model();
    Test_access access2(m);
    access2.clear_mines_on_board();

    std::unordered_map<Model::Position, Cell> board = access2.get_board();

    // Test that {0,0} is covered.
    CHECK(board[Model::Position{0,0}].is_covered());
    // Test that {1,0} is un-flagged.
    CHECK(! board[Model::Position{1,0}].is_flagged());
}

// Test Specification #8
// Place 2 flags on covered cells. Ensure the counter is at 47
// Then reveal a non-mine cell. Then attempt to flag the uncovered cell.
// Ensure that the counter stays put at 47.
// Then place another flag, and ensure the counter is at 46.
// Check Favorite Test #2: "Flag Mechanics"


// Place 50 flags
// Test that the flag counter is at -1.
// The actual Minesweeper game has a negative tally. So, it should be negative
TEST_CASE("Test that when c > 100, the flag counter is negative")
{
    Model m;
    Test_access access(m);
    // The model has randomly generated all the mines by this point.
    // Now, place 50 flags on unique positions on the board without any rhyme
    // or reason.
    int counter = 0;
    for (auto& p: access.get_board())
    {
        if (counter < 50)
        {
            m.flag(p.first);
            counter++;
            CHECK(m.get_flag_counter() == (49 - counter));
        }
        else
        {
            break;
        }
    }
    CHECK(m.get_flag_counter() == -1);
}

// Test Specification #9
// Test by observation.

// Test Specification #10
// Test winning the game. When the user uncovers all non-mine cells, game_over
// should be true and did_you_win should be true.

TEST_CASE("Win the game")
{
    Model m;
    Test_access access(m);
    access.clear_mines_on_board();
    access.set_mine(Model::Position{20, 1}, true);
    access.guarantee_adjacent_mines();

    // Two cells need to be uncovered to finish the game. Any cell besides
    // {20, 0} and {20, 0}.
    m.reveal(Model::Position{0,0});
    m.reveal(Model::Position{20,0});

    // Test the game is over, and the user has won.
    CHECK(m.is_game_over());
    CHECK(m.did_user_win());
}




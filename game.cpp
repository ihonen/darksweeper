#include "game.hh"

#include "images.hh"
#include "tile.hh"
#include <ctime>
#include <iostream>
#include <limits>
#include <random>
#include <QIcon>
#include <QLayout>
#include <QMouseEvent>

static const int WINDOW_MARGIN = 3;
static const int GAMEVIEW_TOP_MARGIN = 3;
const QString Game::TITLE = "Darksweeper";

static const int TIME_COUNTER_INTERVAL_ms = 1000;

static const uint32_t WINDOW_BACKGROUND_COLOR = 0x222222;

using std::map;
using std::pair;
using std::vector;

const map<Game::GridSize, Game::Rules> Game::RULES =
{
   {Game::GridSize::SMALL,      { 8,  8,  10,   54}},
   {Game::GridSize::MEDIUM,     {16, 16,  40,  216}},
   {Game::GridSize::LARGE,      {32, 16,  99,  413}},
   {Game::GridSize::VERY_LARGE, {48, 32, 320, 1216}}
};

struct Coordinate
{
    int x;
    int y;
};

static const vector<Coordinate> ADJACENT_COORD_OFFSETS =
{
    {-1,  1},
    {-1,  0},
    {-1, -1},
    { 0,  1},
    { 0, -1},
    { 1,  1},
    { 1,  0},
    { 1, -1}
};

Game::Game(QWidget* parent) :
    QMainWindow(parent)
{
    this->init_member_vars();
    this->init_signals();
    this->generate_tiles(m_active_rules->n_tiles_x, m_active_rules->n_tiles_y);
    this->init_layout();
    this->set_window_properties();
    this->reset_game();
}

void Game::init_member_vars()
{
    m_first_click_received = false;
    m_game_lost = false;
    m_game_won = false;
    m_tiles = {};
    m_tiles_flat = {};
    m_adjacent_tiles = {};
    m_active_rules = &(RULES.at(GridSize::LARGE));
    m_cleared_tiles = 0;
    m_mines_left = m_active_rules->n_mines;
    m_highlight_focus = nullptr;
    m_highlighted_tiles = {};

    m_game_scene = new GameScene(this);
    m_game_view = new GameView(this);
    m_mine_counter = new LcdCounter(m_mines_left, this);
    m_time_counter = new LcdCounter(0, this);
    m_smiley_button = new SmileyButton(this);
    m_timer = new QTimer(this);
}

void Game::init_signals()
{
    QObject::connect(m_game_scene,
                     &GameScene::tile_left_released,
                     this,
                     &Game::on_tile_left_released);
    QObject::connect(m_game_scene,
                     &GameScene::tile_right_pressed,
                     this,
                     &Game::on_tile_right_pressed);
    QObject::connect(m_game_scene,
                     &GameScene::tile_left_pressed,
                     this,
                     &Game::on_tile_left_pressed);

    QObject::connect(m_timer,
                     &QTimer::timeout,
                     this,
                     &Game::on_timer_timeout);

    QObject::connect(m_smiley_button,
                     &SmileyButton::clicked,
                     this,
                     &Game::reset_game);
}

void Game::generate_tiles(int width, int height)
{
    for (int x = 0; x < width; ++x)
    {
        m_tiles.push_back({});
        m_adjacent_tiles.push_back({});
        for (int y = 0; y < height; ++y)
        {
            Tile* new_tile = new Tile(x, y);
            m_game_scene->add_tile(new_tile);
            m_tiles.at(x).push_back(new_tile);
            m_adjacent_tiles.at(x).push_back({});
            m_tiles_flat.push_back(new_tile);
        }
    }

    this->compute_adjacent_tiles();

    m_game_view->setScene(m_game_scene);
}

void Game::init_layout()
{
    this->setFixedSize(m_game_scene->width() + 2 * WINDOW_MARGIN,
                       m_smiley_button->height() + m_game_scene->height()
                       + 2 * WINDOW_MARGIN + GAMEVIEW_TOP_MARGIN);

    m_game_view->setFixedSize(m_game_scene->width() + 2 * m_game_view->frameWidth(),
                             m_game_scene->height() + 2 * m_game_view->frameWidth());

    m_mine_counter->move(WINDOW_MARGIN, WINDOW_MARGIN);
    m_time_counter->move(this->width() - m_time_counter->width() - WINDOW_MARGIN, WINDOW_MARGIN);
    m_smiley_button->move((this->width()) / 2 - (m_smiley_button->width() / 2), WINDOW_MARGIN);

    m_game_view->move(WINDOW_MARGIN, m_smiley_button->height() + WINDOW_MARGIN + GAMEVIEW_TOP_MARGIN);
}

void Game::set_window_properties()
{
    this->setAutoFillBackground(true);
    this->setPalette(QPalette(QPalette::Background,
                              QColor(WINDOW_BACKGROUND_COLOR)));

    this->setWindowIcon(QIcon(IMG_GAME_ICON_LARGE));
    this->setWindowTitle(Game::TITLE);
}

Game::~Game()
{
    for (auto& tile : m_tiles_flat) delete tile;
}

void Game::reset_game()
{
    for (auto& tile : m_tiles_flat) tile->reset();
    m_cleared_tiles = 0;
    m_mines_left = m_active_rules->n_mines;
    m_highlight_focus = nullptr;
    m_highlighted_tiles = {};
    m_first_click_received = false;
    m_game_won = false;
    m_game_lost = false;
    m_time_counter->set_value(0);
    m_mine_counter->set_value(m_mines_left);
}

void Game::compute_adjacent_tiles()
{
    for (auto& tile : m_tiles_flat)
        this->compute_adjacent_tiles_of_tile(tile);
}

void Game::compute_adjacent_tiles_of_tile(Tile* tile)
{
    for (const auto& coord_offset : ADJACENT_COORD_OFFSETS)
    {
        Tile* adjacent_tile = this->tile_at(tile->x() + coord_offset.x,
                                            tile->y() + coord_offset.y);
        if (adjacent_tile) m_adjacent_tiles.at(tile->x()).at(tile->y())
                                           .push_back(adjacent_tile);
    }
}

void Game::generate_mines(int mine_count, Tile* first_tile_clicked)
{
    while (mine_count > 0)
    {
        Tile* target_tile = this->tile_at(this->random_x_coord(),
                                          this->random_y_coord());
        if (target_tile == first_tile_clicked) continue;
        else if (target_tile->contains_mine()) continue;
        target_tile->add_mine();
        --mine_count;
    }
}

void Game::compute_adjacent_mines()
{
    for (auto& tile : m_tiles_flat)
    {
        int adjacent_mines = 0;
        for (auto& neighbor_tile : this->adjacent_tiles(tile))
            if (neighbor_tile->contains_mine()) ++adjacent_mines;

        tile->set_adjacent_mines(adjacent_mines);
    }
}

int Game::adjacent_flags(Tile* tile)
{
    int adjacent_flags = 0;
    for (auto& neighbor_tile : this->adjacent_tiles(tile))
        if (neighbor_tile->status() == Tile::Status::FLAGGED)
            ++adjacent_flags;

    return adjacent_flags;
}

vector<Tile*>& Game::adjacent_tiles(Tile *tile)
{
    return m_adjacent_tiles.at(tile->x()).at(tile->y());
}

void Game::reveal_empty_area(Tile* center_tile)
{
    if (center_tile->adjacent_mines() == 0)
    {
        for (auto& adjacent_tile : this->adjacent_tiles(center_tile))
        {
            if (adjacent_tile->status() == Tile::Status::UNREVEALED)
                this->on_tile_left_released(adjacent_tile);
        }
    }
}

bool Game::all_adjacent_mines_flagged(Tile* tile)
{
    return (this->adjacent_flags(tile) == tile->adjacent_mines())
           && tile->adjacent_mines() > 0;
}

void Game::reveal_adjacent_nonflagged(Tile* tile)
{
    if (this->all_adjacent_mines_flagged(tile))
        for (auto& adjacent_tile : this->adjacent_tiles(tile))
            if (adjacent_tile->status() == Tile::Status::UNREVEALED)
                this->on_tile_left_released(adjacent_tile);
}

void Game::non_mine_tile_left_clicked(Tile* tile)
{
    Tile::Status pre_click_status = tile->status();
    tile->left_released();

    if (pre_click_status == Tile::Status::UNREVEALED)
    {
        ++m_cleared_tiles;
        if (tile->adjacent_mines() == 0)
            reveal_empty_area(tile);
    }
    else if (pre_click_status == Tile::Status::REVEALED)
        if (this->all_adjacent_mines_flagged(tile))
            this->reveal_adjacent_nonflagged(tile);
}

void Game::mine_tile_clicked(Tile* tile)
{
    if (tile->status() == Tile::Status::UNREVEALED)
        m_game_lost = true;
    tile->left_released();
}

void Game::highlight_adjacent_unmarked(Tile* tile)
{
    for (auto& adjacent_tile : this->adjacent_tiles(tile))
        if (adjacent_tile->status() == Tile::Status::UNREVEALED)
            adjacent_tile->highlight();
}

void Game::unhighlight_adjacent(Tile* tile)
{
    for (auto& adjacent_tile : this->adjacent_tiles(tile))
        adjacent_tile->unhighlight();
}

void Game::on_first_tile_clicked(Tile* tile)
{
    this->generate_mines(m_active_rules->n_mines, tile);
    this->compute_adjacent_mines();
    m_timer->start(TIME_COUNTER_INTERVAL_ms);
    m_first_click_received = true;
}

void Game::on_tile_left_released(Tile* tile)
{
    if (m_game_lost || m_game_won) return;

    if (!m_first_click_received) this->on_first_tile_clicked(tile);

    this->clear_highlight_focus();

    if (!tile->contains_mine())
    {
        this->non_mine_tile_left_clicked(tile);
        m_smiley_button->set_status(SmileyButton::Status::HAPPY);
    }
    else
    {
        this->mine_tile_clicked(tile);
        if (m_game_lost)
            on_game_lost();
    }

    if (this->win_condition())
        on_game_won();

    this->update_smileybutton();
}

void Game::on_tile_right_pressed(Tile* tile)
{
    if (m_game_lost || m_game_won) return;

    tile->right_click();
    if (tile->status() == Tile::Status::FLAGGED)
    {
        --m_mines_left;
        m_mine_counter->decrement();
    }
    else if (tile->status() == Tile::Status::QUESTIONED)
    {
        ++m_mines_left;
        m_mine_counter->increment();
    }
}

void Game::clear_highlight_focus()
{
    if (m_highlight_focus == nullptr) return;

    for (auto& tile : this->adjacent_tiles(m_highlight_focus))
        tile->unhighlight();

    m_highlight_focus->unhighlight();
    m_highlight_focus = nullptr;
}

void Game::update_smileybutton()
{
    if (m_game_lost)
        m_smiley_button->set_status(SmileyButton::Status::DEAD);
    else if (m_game_won)
        m_smiley_button->set_status(SmileyButton::Status::COOL);
    else
        m_smiley_button->set_status(SmileyButton::Status::HAPPY);
}

void Game::set_highlight_focus(Tile* tile)
{
    this->clear_highlight_focus();

    if (tile->status() == Tile::Status::REVEALED)
        this->highlight_adjacent_unmarked(tile);
    else if (tile->status() == Tile::Status::UNREVEALED)
        tile->highlight();

    m_highlight_focus = tile;
}

void Game::on_tile_left_pressed(Tile* tile)
{
    if (m_game_lost || m_game_won) return;

    m_smiley_button->set_status(SmileyButton::Status::WORRIED);
    if (tile != m_highlight_focus)
        this->set_highlight_focus(tile);

    tile->left_press();
}

Tile* Game::tile_at(int x, int y)
{
    if (!this->is_legal_coord(x, y)) return nullptr;
    return m_tiles.at(x).at(y);
}

bool Game::is_legal_coord(int x, int y)
{
    return (x >= 0 && x < m_active_rules->n_tiles_x)
            && (y >= 0 && y < m_active_rules->n_tiles_y);
}

bool Game::win_condition()
{
    m_game_won = (m_cleared_tiles == m_active_rules->n_clear_tiles);
    return m_game_won;
}

void Game::flag_unflagged_mines()
{
    for (auto& tile : m_tiles_flat)
        if (tile->contains_mine()) tile->set_graphic(IMG_TILE_FLAGGED);
}

void Game::reveal_unflagged_mines()
{
    for (auto& tile : m_tiles_flat)
    {
        if (!tile->contains_mine()) continue;

        if (tile->status() == Tile::Status::UNREVEALED
            || tile->status() == Tile::Status::QUESTIONED)
        {
            tile->set_graphic(IMG_TILE_REVEALED_MINE);
        }
    }
}

void Game::reveal_false_flags()
{
    for (auto& tile : m_tiles_flat)
    {
        if (tile->contains_mine()) continue;

        if (tile->status() == Tile::Status::FLAGGED)
            tile->set_graphic(IMG_TILE_REVEALED_FALSE_FLAG);
    }
}

void Game::on_game_won()
{
    assert(m_game_won);
    this->flag_unflagged_mines();
    m_mine_counter->set_value(0);
    m_timer->stop();
}

void Game::on_game_lost()
{
    assert(m_game_lost);
    this->reveal_unflagged_mines();
    this->reveal_false_flags();
    m_timer->stop();
}

void Game::on_timer_timeout()
{
    m_time_counter->increment();
}

int Game::random_non_negative_int()
{
    static std::default_random_engine rng =
            std::default_random_engine(time(nullptr));
    static std::uniform_int_distribution<int> distr =
            std::uniform_int_distribution<int>(0, std::numeric_limits<int>::max());
    return distr(rng);
}

int Game::random_x_coord()
{
    return this->random_non_negative_int() % m_active_rules->n_tiles_x;
}

int Game::random_y_coord()
{
    return this->random_non_negative_int() % m_active_rules->n_tiles_y;
}

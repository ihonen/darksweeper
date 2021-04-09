#ifndef WIDGET_HH
#define WIDGET_HH

#include "lcdcounter.hh"
#include "gamescene.hh"
#include "gameview.hh"
#include "smileybutton.hh"
#include <map>
#include <random>
#include <vector>
#include <QLayout>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QWidget>
#include <QMenuBar>
#include <QMainWindow>

class Game : public QMainWindow
{
    Q_OBJECT

public:
    Game(QWidget* parent = nullptr);
    virtual ~Game();
private slots:
    void on_tile_left_pressed(Tile* tile_at);
    void on_tile_left_released(Tile* tile_at);
    void on_tile_right_pressed(Tile* tile_at);
    void on_timer_timeout();
private:
    enum class GridSize
    {
        SMALL,
        MEDIUM,
        LARGE,
        VERY_LARGE
    };

    struct Rules
    {
        int n_tiles_x = 0;
        int n_tiles_y = 0;
        int n_mines = 0;
        int n_clear_tiles = 0;
    };

    void init_member_vars();
    void init_menubar();
    void init_signals();
    void init_layout();
    void generate_tiles(int x, int y);
    void set_window_properties();
    void reset_game();
    void compute_adjacent_tiles();
    void compute_adjacent_tiles_of_tile(Tile* tile);
    void generate_mines(int mine_count, Tile* first_tile_clicked);
    void compute_adjacent_mines();
    int adjacent_flags(Tile* tile);
    std::vector<Tile*>& adjacent_tiles(Tile* tile_at);
    void reveal_empty_area(Tile* center_tile);
    void reveal_neighbor_tiles(Tile* tile_at);
    void reveal_adjacent_nonflagged(Tile* tile);
    Tile* tile_at(int x, int y);
    bool is_legal_coord(int x, int y);
    bool all_adjacent_mines_flagged(Tile* tile);
    bool win_condition();
    bool lose_condition();
    void non_mine_tile_left_clicked(Tile* tile);
    void mine_tile_clicked(Tile* tile);
    void highlight_adjacent_unmarked(Tile* tile);
    void unhighlight_adjacent(Tile* tile);
    void on_game_won();
    void on_game_lost();
    std::vector<Tile*> tiles_flattened();
    void reveal_unflagged_mines();
    void reveal_false_flags();
    void flag_unflagged_mines();
    void set_highlight_focus(Tile* tile);
    void clear_highlight_focus();
    void update_smileybutton();
    void on_first_tile_clicked(Tile* tile);
    int random_non_negative_int();
    int random_x_coord();
    int random_y_coord();

    static const std::map<enum GridSize, struct Rules> RULES;
    static const QString TITLE;

    GameView* m_game_view;
    GameScene* m_game_scene;
    std::vector<std::vector<Tile*>> m_tiles;
    std::vector<Tile*> m_tiles_flat;
    std::vector<std::vector<std::vector<Tile*>>> m_adjacent_tiles;
    bool m_first_click_received;
    bool m_game_lost;
    bool m_game_won;
    Tile* m_highlight_focus;
    int m_mines_left;
    int m_cleared_tiles;
    const Rules* m_active_rules;
    std::vector<Tile*> m_highlighted_tiles;
    QTimer* m_timer;
    LcdCounter* m_mine_counter;
    LcdCounter* m_time_counter;
    SmileyButton* m_smiley_button;
    QMenuBar* m_menubar;
    QGridLayout* m_layout;
};

#endif // WIDGET_HH

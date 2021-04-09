#ifndef TILE_HH
#define TILE_HH

#include <vector>
#include <QGraphicsPixmapItem>

class Tile : public QGraphicsPixmapItem
{
public:
    enum class Status
    {
        UNREVEALED,
        FLAGGED,
        QUESTIONED,
        REVEALED
    };

    enum class Graphic
    {
        TILE_CLEAR,
        TILE_FLAGGED,
        TILE_MINE,
        TILE_MINE_EXPLODED,
        TILE_FALSE_FLAGGED,
        TILE_QUESTION_MARK,
        TILE_DEFAULT
    };

    Tile(int x, int y, QGraphicsItem* parent = nullptr);
    int x();
    int y();
    void set_status(Tile::Status new_state);
    Status status();
    void reveal();
    void add_mine();
    void set_adjacent_mines(int count);
    int adjacent_mines();
    bool contains_mine();
    void highlight();
    void unhighlight();
    bool highlighted();
    void reset();
    void left_press();
    void left_press_stop();
    void left_released();
    void right_click();
    void set_graphic(const QString& new_graphic_path_);
    const QString& get_graphic();

    static const int GRAPHIC_WIDTH = 20; // pixels
    static const int GRAPHIC_HEIGHT = 20; // pixels
private:
    void assert_invariants();
    const QString& adjacent_graphic_name();
    int m_x;
    int m_y;
    Status m_status;
    int m_n_adjacent_mines;
    bool m_has_mine;
    std::vector<Tile*> m_adjacent_tiles;
    bool m_is_highlighted;
    QString m_graphic_url;
};

#endif // TILE_HH

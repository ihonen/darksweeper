#include "tile.hh"

#include "images.hh"
#include <QGraphicsSceneMouseEvent>

using std::vector;

Tile::Tile(int x, int y, QGraphicsItem* parent) : QGraphicsPixmapItem(parent),
    m_x(x),
    m_y(y),
    m_status(Status::UNREVEALED),
    m_n_adjacent_mines(0),
    m_has_mine(false),
    m_is_highlighted(false),
    m_graphic_url("")
{
    this->reset();
}

void Tile::reset()
{
    m_status = Status::UNREVEALED;
    m_n_adjacent_mines = 0;
    m_has_mine = false;
    m_is_highlighted = false;
    m_graphic_url = IMG_TILE_DEFAULT;
    this->set_graphic(m_graphic_url);
    this->assert_invariants();
}

int Tile::x()
{
    return m_x;
}

int Tile::y()
{
    return m_y;
}

void Tile::set_status(Status new_status)
{
    this->m_status = new_status;
    this->assert_invariants();
}

Tile::Status Tile::status()
{
    return m_status;
}

void Tile::add_mine()
{
    m_has_mine = true;
}

void Tile::set_adjacent_mines(int count)
{
    m_n_adjacent_mines = count;
    this->assert_invariants();
}

int Tile::adjacent_mines()
{
    return m_n_adjacent_mines;
}

bool Tile::contains_mine()
{
    return m_has_mine;
}

void Tile::highlight()
{
    this->assert_invariants();
    m_is_highlighted = true;
    this->setPixmap(QPixmap(IMG_TILE_CLEAR_0));
    this->assert_invariants();
}

void Tile::unhighlight()
{
    this->assert_invariants();
    m_is_highlighted = false;
    this->set_graphic(m_graphic_url);
    this->assert_invariants();
}

bool Tile::highlighted()
{
    return m_is_highlighted;
}

void Tile::left_press()
{
    if (!m_is_highlighted && m_status == Status::UNREVEALED)
    {
        this->highlight();
    }
    this->assert_invariants();
}

void Tile::left_press_stop()
{
    if (m_is_highlighted)
    {
        this->unhighlight();
    }
    this->assert_invariants();
}

void Tile::left_released()
{
    if (m_status == Status::UNREVEALED)
    {
        this->set_status(Status::REVEALED);
        if (!m_has_mine)
        {
            this->set_graphic(this->adjacent_graphic_name());
        }
        else
        {
            this->set_graphic(IMG_TILE_REVEALED_EXPLODED);
        }
    }

    this->assert_invariants();
}

void Tile::right_click()
{
    if (m_status == Status::UNREVEALED)
    {
        this->set_status(Status::FLAGGED);
        this->set_graphic(IMG_TILE_FLAGGED);
    }
    else if (m_status== Status::FLAGGED)
    {
        this->set_status(Status::QUESTIONED);
        this->set_graphic(IMG_TILE_QUESTION_MARK);
    }
    else if (m_status == Status::QUESTIONED)
    {
        this->set_status(Status::UNREVEALED);
        this->set_graphic(IMG_TILE_DEFAULT);
    }
    this->assert_invariants();
}

void Tile::set_graphic(const QString& img_path)
{
    m_graphic_url = img_path;
    this->setPixmap(QPixmap(img_path));
    this->assert_invariants();
}

const QString& Tile::get_graphic()
{
    return m_graphic_url;
}

void Tile::assert_invariants()
{
    assert(m_n_adjacent_mines >= 0);
    assert(adjacent_mines() <= 8);
    assert(m_x >= 0);
    assert(m_y >= 0);
    assert(m_status == Status::UNREVEALED
           || m_status == Status::FLAGGED
           || m_status == Status::QUESTIONED
           || m_status == Status::REVEALED);
}

const QString& Tile::adjacent_graphic_name()
{
    switch (m_n_adjacent_mines)
    {
        case 0: return IMG_TILE_CLEAR_0;
        case 1: return IMG_TILE_CLEAR_1;
        case 2: return IMG_TILE_CLEAR_2;
        case 3: return IMG_TILE_CLEAR_3;
        case 4: return IMG_TILE_CLEAR_4;
        case 5: return IMG_TILE_CLEAR_5;
        case 6: return IMG_TILE_CLEAR_6;
        case 7: return IMG_TILE_CLEAR_7;
        case 8: return IMG_TILE_CLEAR_8;
        default: return IMG_TILE_CLEAR_8;
    }
    this->assert_invariants();
}

#include "gamescene.hh"

#include <iostream>

GameScene::GameScene(QObject* parent) :
    QGraphicsScene(parent),
    m_is_left_mouse_pressed(false),
    m_is_right_mouse_pressed(false),
    m_last_tile_touched(nullptr)
{
}

void GameScene::add_tile(Tile* tile)
{
    this->addItem(tile);
    tile->setPos(tile->mapToScene(tile->x() * Tile::GRAPHIC_WIDTH,
                                  tile->y() * Tile::GRAPHIC_HEIGHT));
}

void GameScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    Tile* tile = tile_at(event->lastScenePos());
    if (!tile) return;
    m_last_tile_touched = tile;

    if (event->button() == Qt::LeftButton)
    {
        m_is_left_mouse_pressed = true;
        emit(tile_left_pressed(tile));
    }
    else if (event->button() == Qt::RightButton)
    {
        m_is_right_mouse_pressed = true;
        emit(tile_right_pressed(tile));
    }

    m_last_tile_touched = tile;

    this->QGraphicsScene::mousePressEvent(event);
}

void GameScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    Tile* tile = this->tile_at(event->lastScenePos());
    if (!tile)
    {
        if (m_last_tile_touched) m_last_tile_touched->unhighlight();
        return;
    }
    m_last_tile_touched = tile;

    if (m_is_left_mouse_pressed && tile != nullptr)
    {
        emit(tile_left_pressed(tile));
    }

    this->QGraphicsScene::mouseMoveEvent(event);
}

void GameScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Tile* tile = tile_at(event->lastScenePos());
    if (!tile) return;
    m_last_tile_touched = tile;

    if (event->button() == Qt::LeftButton)
    {
        m_is_left_mouse_pressed = false;
        emit(tile_left_released(tile));
    }
    else if (event->button() == Qt::RightButton)
    {
        m_is_right_mouse_pressed = false;
    }

    this->QGraphicsScene::mouseReleaseEvent(event);
}

void GameScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
        this->mouseReleaseEvent(event);
    else if (event->button() == Qt::RightButton)
        this->mousePressEvent(event);

    this->QGraphicsScene::mouseDoubleClickEvent(event);
}

Tile* GameScene::tile_at(const QPointF& pos)
{
    return dynamic_cast<Tile*>(this->itemAt(pos, QTransform()));
}

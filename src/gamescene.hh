#ifndef GAMESCENE_HH
#define GAMESCENE_HH

#include "tile.hh"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class GameScene : public QGraphicsScene
{
    Q_OBJECT
signals:
    void tile_left_pressed(Tile* tile);
    void tile_left_released(Tile* tile);
    void tile_right_pressed(Tile* tile);
public:
    explicit GameScene(QObject* parent = nullptr);
    virtual ~GameScene() override = default;
    void add_tile(Tile* tile);
private:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
    Tile* tile_at(const QPointF& pos);

    bool m_is_left_mouse_pressed;
    bool m_is_right_mouse_pressed;
    Tile* m_last_tile_touched;
};

#endif // GAMESCENE_HH

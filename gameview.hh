#ifndef GAMEVIEW_HH
#define GAMEVIEW_HH

#include <QGraphicsView>
#include <QMouseEvent>

class GameView : public QGraphicsView
{
public:
    GameView(QWidget* parent = nullptr);
};

#endif // GAMEVIEW_HH

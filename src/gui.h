#ifndef BARNESHUT_GUI_H
#define BARNESHUT_GUI_H

#include <QApplication>
#include <QPushButton>
#include <QPainter>
#include <QMainWindow>
#include <QMouseEvent>
#include <QTimer>
#include <QElapsedTimer>
#include <QLabel>

#include "quadtree.h"


class DotWindow : public QMainWindow {
Q_OBJECT

public:
    static constexpr qreal window_width = 800;
    static constexpr qreal window_height = 800;
    static constexpr qreal dt_refresh = 16;
    using UpdateFunction = std::function<void(Quadtree &)>;

    DotWindow(Quadtree &qt, UpdateFunction updateFunc, QWidget *parent = nullptr);

private slots:

    void updateParticle();

private:
    Quadtree qt;
    std::function<void(Quadtree &)> &&updateFunction;
    qreal zoom = 1.0;
    qreal half_width = window_width / 2;
    qreal half_height = window_height / 2;
    qreal camX = window_width / 2;
    qreal camY = window_height / 2;
    QPointF lastMousePos;
    QElapsedTimer simulationTimer;
    std::vector<QBrush> brushColors;

    struct Screenbox {
        Vector<qreal> min;
        Vector<qreal> max;
    };

    [[nodiscard]] QPointF particleToPixel(const Particle &p) const;

    void drawParticles(QPainter &painter, const std::vector<Particle> &particles) const;

    bool screenBoxIsVisible(Screenbox s);

    void drawQuadtree(QPainter &painter, const Screenbox &sbox, NodeId id);

    void paintEvent(QPaintEvent *) override;

    void mousePressEvent(QMouseEvent *event) override;

    [[nodiscard]] bool reachedLeftBorder() const;

    [[nodiscard]] bool reachedRightBorder() const;

    [[nodiscard]] bool reachedBottomBorder() const;

    [[nodiscard]] bool reachedTopBorder() const;

    void mouseMoveEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;
};

#endif //BARNESHUT_GUI_H
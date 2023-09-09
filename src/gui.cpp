#ifdef DEBUG

#include <iostream>

#endif

#include "gui.h"

DotWindow::DotWindow(Quadtree &qt, UpdateFunction updateFunc, QWidget *parent) :
        QMainWindow(parent),
        qt(qt),
        updateFunction(std::move(updateFunc)) {
    setWindowTitle("Particle Simulator");
    setStyleSheet("QMainWindow {background: 'black';}");
    setFixedSize(QSize(window_width, window_height));
    setMouseTracking(true);

    // animation timer
    simulationTimer.start();
    auto *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateParticle()));
    timer->start(dt_refresh); // ms (16 = 60 FPS)
}

void DotWindow::updateParticle() {
    updateFunction(qt);
    update();
}

QPointF DotWindow::particleToPixel(const Particle &p) const {
    // (x - offset) * width / zoom
    qreal x = (p.r.x * window_width - camX + zoom * half_width) / zoom;
    qreal y = ((1 - p.r.y) * window_height - camY + zoom * half_height) / zoom;
    return {x, y};
}

void DotWindow::drawParticles(QPainter &painter, const std::vector<Particle> &particles) const {
    painter.setPen(Qt::white);
    for (const auto &particle: particles) {
        QPointF point = particleToPixel(particle);
        if (point.x() > 0 && point.x() < window_width && point.y() > 0 && point.x() < window_height) {
            painter.drawEllipse(point.x(), point.y(), 1, 1);
        }
    }
}

bool DotWindow::screenBoxIsVisible(Screenbox s) {
    if (s.min.x > camX + half_width * zoom) return false;
    if (s.max.x < camX - half_width * zoom) return false;
    if (s.min.y > camY + half_height * zoom) return false;
    if (s.max.y < camY - half_height * zoom) return false;
    return true;
}

void DotWindow::drawQuadtree(QPainter &painter, const Screenbox &sbox, NodeId id) {
    const Node &currentNode = qt.nodes[id];
    if (currentNode.isLeaf() && screenBoxIsVisible(sbox)) {
        qreal x1 = (sbox.min.x - camX + half_width * zoom) / zoom;
        qreal x2 = (sbox.max.x - camX + half_width * zoom) / zoom;
        if (x2 - x1 < 1) return;
        qreal y1 = (sbox.min.y - camY + half_height * zoom) / zoom;
        qreal y2 = (sbox.max.y - camY + half_height * zoom) / zoom;
        QPen pen(Qt::darkGreen, 0.5, Qt::DotLine);
        painter.setPen(pen);
        painter.drawRect(x1, y1, x2 - x1, y2 - y1);
    }

    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            NodeId childId = currentNode.children[i][j];
            if (childId == null_node) continue;
            qreal xmin = sbox.min.x, ymin = sbox.min.y;
            qreal xmax = sbox.max.x, ymax = sbox.max.y;
            qreal midx = (xmin + xmax) / 2, midy = (ymin + ymax) / 2;
            if (i == 0 && j == 0)
                drawQuadtree(painter, {{xmin, midy},
                                       {midx, ymax}}, childId);
            else if (i == 0 && j == 1)
                drawQuadtree(painter, {{midx, midy},
                                       {xmax, ymax}}, childId);
            else if (i == 1 && j == 0)
                drawQuadtree(painter, {{xmin, ymin},
                                       {midx, midy}}, childId);
            else
                drawQuadtree(painter, {{midx, ymin},
                                       {xmax, midy}}, childId);
        }
    }
}

void DotWindow::paintEvent(QPaintEvent *) {
#ifdef DEBUG
    auto startTime = std::chrono::high_resolution_clock::now();
#endif
    QPainter painter(this);
    drawParticles(painter, qt.particles);
    //drawQuadtree(painter, {{0,            0},{window_width, window_height}}, qt.root);
#ifdef DEBUG
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    std::cout << "Elapsed time drawing: " << duration.count() << " microseconds" << std::endl;
#endif
}

void DotWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos();
    }
}

bool DotWindow::reachedLeftBorder() const {
    return camX - half_width * zoom < 0;
}

bool DotWindow::reachedRightBorder() const {
    return camX + half_width * zoom > window_width;
}

bool DotWindow::reachedBottomBorder() const {
    return camY - half_height * zoom < 0;
}

bool DotWindow::reachedTopBorder() const {
    return camY + half_height * zoom > window_height;
}

void DotWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        // Pan the camera based on the mouse movement
        QPointF delta = (event->pos() - lastMousePos) * zoom;
        camX -= delta.x();
        camY -= delta.y();
        if (reachedLeftBorder()) camX += delta.x();
        if (reachedRightBorder()) camX += delta.x();
        if (reachedTopBorder()) camY += delta.y();
        if (reachedBottomBorder()) camY += delta.y();
        lastMousePos = event->pos();
    }
}

void DotWindow::wheelEvent(QWheelEvent *event) {
    int degrees = event->angleDelta().y() / 32;
    qreal factor = std::pow(1.02, -degrees);
    qreal _zoom = std::min(zoom * factor, 1.);
    if (reachedLeftBorder()) camX += +half_width * _zoom - camX;;
    if (reachedRightBorder()) camX += -half_width * _zoom - camX + window_width;
    if (reachedBottomBorder()) camY += +half_height * _zoom - camY;
    if (reachedTopBorder()) camY += -half_height * _zoom - camY + window_height;
    zoom = std::min(zoom * factor, 1.);
}

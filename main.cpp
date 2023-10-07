#include <random>
#include <vector>
#include <iostream>
#include <chrono>

#include "src/quadtree.h"
#include "src/physics.h"
#include "src/gui.h"


int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    std::mt19937 rng(static_cast<unsigned>(42));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> distRadius(0., 0.2);
    std::uniform_real_distribution<float> distAngle(.0, 2.0 * 3.1415);
    std::uniform_real_distribution<float> distVelocity(1., 1.);

    int numparticle = 3333;
    std::vector<Particle> particles;
    for (int i = 0; i < numparticle; ++i) {
        float radius = distRadius(gen);
        float angle = distAngle(gen);
        float velocity = distVelocity(gen);
        Particle p;
        p.mass = 1;
        p.r.x = radius * cos(angle) + 0.25;
        p.r.y = radius * sin(angle) + 0.25;
        p.v.x = -velocity * sin(angle) * radius;
        p.v.y = velocity * cos(angle) * radius;
        particles.push_back(p);
    }
    for (int i = 0; i < numparticle; ++i) {
        float radius = distRadius(gen);
        float angle = distAngle(gen);
        float velocity = distVelocity(gen);
        Particle p;
        p.mass = 1;
        p.r.x = radius * cos(angle) + 0.75;
        p.r.y = radius * sin(angle) + 0.75;
        p.v.x = -velocity * sin(angle) * radius;
        p.v.y = velocity * cos(angle) * radius;
        particles.push_back(p);
    }

    Quadtree qt = build(particles);
    DotWindow window(qt);
    window.show();

    return app.exec();
}

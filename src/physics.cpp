#include "physics.h"

constexpr Length squared_smoothing = smoothing * smoothing;

Force force(Length sqdistance) {
    return G / ((sqdistance + squared_smoothing) * sqrt(sqdistance + squared_smoothing));
}

void _barnesHut(Vector<Acceleration> &acc, Quadtree &qt, NodeId id, Particle &particle, float threshold) {
    if (id == null_node)
        return;
    if (qt.nodes[id].isLeaf()) {
        auto i = qt.node_particles_begin[id];
        if (&qt.particles[i] == &particle)
            return;
        Length dx = particle.r.x - qt.particles[i].r.x;
        Length dy = particle.r.y - qt.particles[i].r.y;
        Length d2 = dx * dx + dy * dy;
        Force f = particle.mass * qt.particles[i].mass * force(d2);
        acc.x += f * dx / particle.mass;
        acc.y += f * dy / particle.mass;
        return;
    } else {
        Length s = (qt.widths[id]);
        const struct Particle &center_of_mass = qt.center_of_mass[id];
        Length dx = particle.r.x - center_of_mass.r.x;
        Length dy = particle.r.y - center_of_mass.r.y;
        Length d2 = dx * dx + dy * dy;
        float sd_ratio = s * s / d2;
        if (sd_ratio < threshold) {
            Force f = particle.mass * center_of_mass.mass * force(d2);
            acc.x += f * dx / particle.mass;
            acc.y += f * dy / particle.mass;
            return;
        } else {
            _barnesHut(acc, qt, qt.nodes[id].children[0][0], particle, threshold);
            _barnesHut(acc, qt, qt.nodes[id].children[0][1], particle, threshold);
            _barnesHut(acc, qt, qt.nodes[id].children[1][0], particle, threshold);
            _barnesHut(acc, qt, qt.nodes[id].children[1][1], particle, threshold);
        }
    }
}

constexpr float squared_threshold = 0.5f * 0.5f;

void barnesHut(Quadtree &qt, float threshold = squared_threshold) {
    for (auto &p: qt.particles) {
        Vector<Acceleration> acc = {0};
        _barnesHut(acc, qt, 0, p, threshold);
        p.a = p.a_;
        p.a_ = acc;
    }
}


void leapfrog(Particle &p, Time dt) {
    Length new_rx = p.r.x + p.v.x * dt + .5f * p.a.x * dt;
    Length new_ry = p.r.y + p.v.y * dt + .5f * p.a.y * dt;
    Velocity new_vx = p.v.x + .5f * (p.a.x + p.a_.x) * dt;
    Velocity new_vy = p.v.y + .5f * (p.a.y + p.a_.y) * dt;

    if (new_rx > 1. || new_rx < 0.) {
        new_rx = round(new_rx);
        new_vx *= -1;
    }
    if (new_ry > 1. || new_ry < 0.) {
        new_ry = round(new_ry);
        new_vy *= -1;
    }

    p.r.x = new_rx;
    p.r.y = new_ry;
    p.v.x = new_vx;
    p.v.y = new_vy;
}

void integrate(Quadtree &qt, Time dt) {
    barnesHut(qt);
    for (auto &p: qt.particles)
        leapfrog(p, dt);
}

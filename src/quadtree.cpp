#include "quadtree.h"


Box &Box::operator|=(Particle const &p) {
    min.x = std::min(min.x, p.r.x);
    min.y = std::min(min.y, p.r.y);
    max.x = std::max(max.x, p.r.x);
    max.y = std::max(max.y, p.r.y);
    return *this;
}

bool Node::isLeaf() const {
    return this->children[0][0] == null_node &&
           this->children[0][1] == null_node &&
           this->children[1][0] == null_node &&
           this->children[1][1] == null_node;
};

template<typename Iterator>
Box bbox(Iterator begin, Iterator end) {
    Box result;
    for (auto it = begin; it != end; ++it)
        result |= *it;
    return result;
}

Vector<Length> middle(Vector<Length> const &r1, Vector<Length> const &r2) {
    return {(r1.x + r2.x) / 2.f, (r1.y + r2.y) / 2.f};
}

template<typename Iterator>
NodeId _build(Quadtree &tree, Box const &bbox, Iterator begin, Iterator end, std::size_t depth = 0) {
    if (begin == end) return null_node;

    NodeId result = tree.nodes.size();
    tree.nodes.emplace_back();
    tree.node_particles_begin.emplace_back(begin - tree.particles.begin());
    tree.widths.emplace_back(bbox.max.x - bbox.min.x);
    Mass mass = 0;
    for (auto it = begin; it != end; ++it) {
        const auto &p = *it;
        mass += p.mass;
    };
    tree.mass.emplace_back(mass);
    Length xcom = 0, ycom = 0;
    for (auto it = begin; it != end; ++it) {
        const auto &p = *it;
        xcom += p.mass * p.r.x;
        ycom += p.mass * p.r.y;
    };
    tree.center_of_mass.emplace_back(Particle{mass, xcom / mass, ycom / mass});

    if (begin + 1 == end) return result;

    if (depth == DEPTH_LIMIT) return result;

    Vector<Length> center = middle(bbox.min, bbox.max);
    auto bottom = [center](Particle const &p) { return p.r.y < center.y; };
    auto left = [center](Particle const &p) { return p.r.x < center.x; };

    Iterator split_y = std::partition(begin, end, bottom);
    Iterator split_x_lower = std::partition(begin, split_y, left);
    Iterator split_x_upper = std::partition(split_y, end, left);

    tree.nodes[result].children[0][0] = _build(
            tree,
            {bbox.min, center},
            begin,
            split_x_lower,
            depth + 1
    );
    tree.nodes[result].children[0][1] = _build(
            tree,
            {{center.x,   bbox.min.y},
             {bbox.max.x, center.y}},
            split_x_lower,
            split_y,
            depth + 1
    );
    tree.nodes[result].children[1][0] = _build(
            tree,
            {{bbox.min.x, center.y},
             {center.x,   bbox.max.y}},
            split_y,
            split_x_upper,
            depth + 1
    );
    tree.nodes[result].children[1][1] = _build(
            tree,
            {center, bbox.max},
            split_x_upper,
            end,
            depth + 1
    );

    return result;
}

Quadtree build(const std::vector<Particle> &particles) {
    Quadtree result;
    result.particles = std::move(particles);
    result.root = _build(
            result,
            {{0, 0},
             {1, 1}},
            result.particles.begin(),
            result.particles.end()
    );
    result.node_particles_begin.push_back(result.particles.size());
    return result;
}
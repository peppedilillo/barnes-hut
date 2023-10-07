#ifndef BARNESHUT_QUADTREE_H
#define BARNESHUT_QUADTREE_H

#include <algorithm>
#include <limits>
#include <cstdint>
#include <vector>

#include "types.h"


using NodeId = std::uint32_t;
static constexpr NodeId null_node = NodeId(-1);
static constexpr std::size_t DEPTH_LIMIT = 64;

struct Node {
    NodeId children[2][2]{
            {null_node, null_node},
            {null_node, null_node}
    };

    bool isLeaf() const;
};

static constexpr float inf = std::numeric_limits<Length>::infinity();

struct Box {
    Vector<Length> min = {inf, inf};
    Vector<Length> max = {-inf, -inf};

    Box &operator|=(Particle const &p);
};

template<typename Iterator>
Box bbox(Iterator begin, Iterator end);

struct Quadtree {
    Box bbox;
    NodeId root{};
    std::vector<Node> nodes;
    std::vector<Particle> particles;
    std::vector<std::uint32_t> node_particles_begin;
    std::vector<Length> widths;
    std::vector<Mass> mass;
    std::vector<Particle> center_of_mass;
};

Quadtree build(const std::vector<Particle> &particles);

#endif

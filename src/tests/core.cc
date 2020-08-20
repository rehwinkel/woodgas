#include <iostream>

#include "../core/core.h"

int main(int argc, char* argv[]) {
    core::Game game;
    core::Entity e0 = game.create_entity();
    core::Entity e1 = game.create_entity();

    size_t id0 = e0.get_id();
    size_t id1 = e1.get_id();

    e0.add_child(std::move(e1));

    game.add_entity(std::move(e0));

    std::cout << "Has game e0? " << game.has_entity(id0) << std::endl;
    std::cout << "Has game e1? " << game.has_entity(id1) << std::endl;

    std::cout << "Has e0 e1? " << game.get_entity(id0).has_child(id1) << std::endl;

    std::cout << "Remove E1..." << std::endl;
    game.get_entity(id0).destroy_child(id1);

    std::cout << "Has e0 e1? " << game.get_entity(id0).has_child(id1) << std::endl;

    std::cout << "Remove E0..." << std::endl;
    game.destroy_entity(id0);

    std::cout << "Has game e0? " << game.has_entity(id0) << std::endl;
    std::cout << "Has game e1? " << game.has_entity(id1) << std::endl;

}
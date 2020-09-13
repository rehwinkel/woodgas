#include "core.h"

using namespace core;

Interface::Interface() : logger(nullptr), renderer(nullptr) {}

Interface::Interface(logging::Logger& logger)
    : logger(&logger), renderer(nullptr) {}

Interface::Interface(render::Renderer& renderer)
    : logger(nullptr), renderer(&renderer) {}

Interface::Interface(logging::Logger& logger, render::Renderer& renderer)
    : logger(&logger), renderer(&renderer) {}

render::Renderer& Interface::get_renderer() { return *this->renderer; }

bool Interface::has_renderer() { return this->renderer; }

logging::Logger& Interface::get_logger() { return *this->logger; }

bool Interface::has_logger() { return this->logger; }

Component::Component() : enabled(true) {}

bool Component::is_enabled() { return this->enabled; }

void Component::set_active(bool state) { this->enabled = state; }

Component::~Component() {}

Entity::Entity(size_t id) : id(id), parent(nullptr) {}

bool Entity::is_enabled() { return this->enabled; }

void Entity::set_active(bool state) { this->enabled = state; }

void Entity::add_child(Entity entity) {
    if (this->has_child(entity.get_id()))
        throw std::runtime_error("Tried to add children with the id " +
                                 std::to_string(entity.get_id()) +
                                 ", but it already exists!");
    entity.parent = this;
    this->children.insert(std::make_pair(entity.get_id(), std::move(entity)));
}

Entity& Entity::get_child(size_t entity_id) {
    if (!this->has_child(entity_id))
        throw std::runtime_error("Tried to get children with id " +
                                 std::to_string(entity_id) +
                                 ", but it doesn't exist!");
    return this->children.at(entity_id);
}

void Entity::destroy_child(size_t entity_id) {
    if (!this->has_child(entity_id))
        throw std::runtime_error("Tried to destroy children with id " +
                                 std::to_string(entity_id) +
                                 ", but it doesn't exist!");
    this->children.erase(entity_id);
}

bool Entity::has_child(size_t entity_id) noexcept {
    return this->children.find(entity_id) != this->children.end();
}

bool Entity::has_parent() noexcept { return this->parent; }

size_t Entity::get_id() noexcept { return this->id; }

Entity& Entity::get_parent() {
    if (!this->has_parent())
        throw std::runtime_error(
            "Tried to get parent of entity, but entity has no parent!");
    return *this->parent;
}

void Entity::add_component(std::unique_ptr<Component> component) {
    if (this->components.find(typeid(*component).hash_code()) ==
        this->components.end()) {
        std::vector<std::unique_ptr<Component>> comp_vec;
        comp_vec.push_back(std::move(component));
        this->components.insert(std::make_pair(
            typeid(*comp_vec.at(0)).hash_code(), std::move(comp_vec)));
    } else if (!(*component).is_unique()) {
        this->components.at(typeid(*component).hash_code())
            .push_back(std::move(component));
    } else {
        throw std::runtime_error("Tried to add unique Component " +
                                 std::string(typeid(*component).name()) +
                                 " twice!");
    }
}

void Entity::init(Interface& interface) {
    for (auto& components : this->components) {
        for (auto& component : components.second) {
            component->init(interface);
        }
    }
    for (auto& child : this->children) {
        child.second.init(interface);
    }
}

void Entity::update(Interface& interface) {
    for (auto& components : this->components) {
        for (auto& component : components.second) {
            component->update(interface);
        }
    }
    for (auto& child : this->children) {
        child.second.update(interface);
    }
}

Entity::~Entity() {}

Game::Game() : entity_id(0) {}

size_t Game::next_id() { return this->entity_id++; }

Entity Game::create_entity() { return std::move(Entity{this->next_id()}); }

void Game::add_entity(Entity entity) {
    if (this->has_entity(entity.get_id()))
        throw std::runtime_error("Tried to add an entity with the id " +
                                 std::to_string(entity.get_id()) +
                                 ", but it already exists!");
    this->entities.insert(std::make_pair(entity.get_id(), std::move(entity)));
}

Entity& Game::get_entity(size_t entity_id) {
    if (!this->has_entity(entity_id))
        throw std::runtime_error("Tried to get entity with id " +
                                 std::to_string(entity_id) +
                                 ", but it doesn't exist!");
    return this->entities.at(entity_id);
}

void Game::destroy_entity(size_t entity_id) {
    if (!this->has_entity(entity_id))
        throw std::runtime_error("Tried to destroy the entity with id " +
                                 std::to_string(entity_id) +
                                 ", but it doesn't exist!");
    this->entities.erase(entity_id);
}

bool Game::has_entity(size_t entity_id) noexcept {
    return this->entities.find(entity_id) != this->entities.end();
}

void Game::init(Interface& interface) {
    for (auto& child : this->entities) {
        child.second.init(interface);
    }
}

void Game::update(Interface& interface) {
    for (auto& child : this->entities) {
        child.second.update(interface);
    }
}

Game::~Game() {}
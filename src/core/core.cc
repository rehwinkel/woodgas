#include "core.h"

#include <iostream>

core::Component::Component() : enabled(true) {
}

bool core::Component::is_enabled() {
    return this->enabled;
}

void core::Component::set_active(bool state) {
    this->enabled = state;
}

core::Component::~Component() {
}

core::Entity::Entity(size_t id) : parent(nullptr), id(id) {
}

bool core::Entity::is_enabled() {
    return this->enabled;
}

void core::Entity::set_active(bool state) {
    this->enabled = state;
}

void core::Entity::add_children(Entity entity) {
    if(this->has_children(entity.get_id())) throw std::runtime_error("Tried to add children with the id " + std::to_string(entity.get_id()) + ", but it already exists!");
    entity.parent = this;
    this->children.insert(std::make_pair(entity.get_id(), std::move(entity)));
}

core::Entity& core::Entity::get_children(size_t entity_id) {
    if(!this->has_children(entity_id)) throw std::runtime_error("Tried to get children with id " + std::to_string(entity_id) + ", but it doesn't exist!");
    return this->children.at(entity_id);
}

void core::Entity::destroy_children(size_t entity_id) {
    if(!this->has_children(entity_id)) throw std::runtime_error("Tried to destroy children with id " + std::to_string(entity_id) + ", but it doesn't exist!");
    this->children.erase(entity_id);
}

bool core::Entity::has_children(size_t entity_id) noexcept {
    return this->children.find(entity_id) != this->children.end();
}

bool core::Entity::has_parent() noexcept {
    return this->parent;
}

size_t core::Entity::get_id() noexcept {
    return this->id;
}

core::Entity& core::Entity::get_parent() {
    if(!this->has_parent()) throw std::runtime_error("Tried to get parent of entity, but entity has no parent!");
    return *this->parent;
}

void core::Entity::add_component(std::unique_ptr<Component> component) {
    if(this->components.find(typeid(*component).hash_code()) == this->components.end()) {
        std::vector<std::unique_ptr<Component>> comp_vec;
        comp_vec.push_back(std::move(component));
        this->components.insert(std::make_pair(typeid(*comp_vec.at(0)).hash_code(), std::move(comp_vec)));
    } else if(!(*component).is_unique()) {
        this->components.at(typeid(*component).hash_code()).push_back(std::move(component));
    } else {
        throw std::runtime_error("Tried to add unique Component " + std::string(typeid(*component).name()) + " twice!");
    }
}

core::Entity::~Entity() {
}


core::Game::Game() : entity_id(0) {

}

size_t core::Game::next_id() {
    return this->entity_id++;
}

core::Entity core::Game::create_entity() {
    return std::move(Entity{this->next_id()});
}

void core::Game::add_entity(Entity entity) {
    if(this->has_entity(entity.get_id())) throw std::runtime_error("Tried to add an entity with the id " + std::to_string(entity.get_id()) + ", but it already exists!");
    this->entities.insert(std::make_pair(entity.get_id(), std::move(entity)));
}

core::Entity& core::Game::get_entity(size_t entity_id) {
    if(!this->has_entity(entity_id)) throw std::runtime_error("Tried to get entity with id " + std::to_string(entity_id) + ", but it doesn't exist!");
    return this->entities.at(entity_id);
}

void core::Game::destroy_entity(size_t entity_id) {
    if(!this->has_entity(entity_id)) throw std::runtime_error("Tried to destroy the entity with id " + std::to_string(entity_id) + ", but it doesn't exist!");
    this->entities.erase(entity_id);
}

bool core::Game::has_entity(size_t entity_id) noexcept {
    return this->entities.find(entity_id) != this->entities.end();
}

core::Game::~Game() {

}
// header for core ECS functionality and API

#pragma once

#include <map>
#include <stdexcept>
#include <memory>

#include "../render/render.h"
#include "../util/timer.h"

namespace core {
    class Game;

    class Interface {
        logging::Logger* logger;
        render::Renderer* renderer;
        timer::Time* time;
        Game* game;

       public:
        Interface();
        Interface(logging::Logger& logger, render::Renderer& renderer,
                  timer::Time& time, Game& game);
        render::Renderer& get_renderer();
        bool has_renderer();
        logging::Logger& get_logger();
        bool has_logger();
        timer::Time& get_time();
        bool has_time();
        Game& get_game();
        bool has_game();
    };

    class Entity;

    class Component {
       private:
        bool enabled;

       protected:
        Entity* entity;

       public:
        Component();
        virtual void update(Interface& interface) = 0;
        virtual void init(Interface& interface) = 0;
        bool is_enabled();
        void set_active(bool state);
        void set_entity(Entity& entity);
        virtual bool is_unique() = 0;
        virtual ~Component();
    };

    class Entity {
       private:
        size_t id;

        bool enabled;
        std::map<size_t, std::vector<std::unique_ptr<Component>>> components;

        std::map<size_t, Entity> children;
        Entity* parent;

       public:
        explicit Entity(size_t id);
        Entity(Entity&& other);
        Entity(const Entity& other) = delete;
        bool is_enabled();
        void set_active(bool state);
        void add_component(std::unique_ptr<Component> component);
        void add_child(Entity entity);
        Entity& get_child(size_t entity_id);
        void destroy_child(size_t entity_id);
        bool has_child(size_t entity_id) noexcept;
        size_t get_id() noexcept;
        bool has_parent() noexcept;
        template <class t>
        inline void remove_component();
        template <class t>
        inline bool has_component() noexcept;
        template <class T>
        inline std::vector<std::unique_ptr<core::Component>>& get_component();
        template <class T>
        inline T& get_single_component();
        Entity& get_parent();
        void init(Interface& interface);
        void update(Interface& interface);
        ~Entity();
    };

    class Game {
       private:
        size_t entity_id;

        std::map<size_t, Entity> entities;

        size_t next_id();

       public:
        Game();
        Entity create_entity();
        void add_entity(Entity entity);
        Entity& get_entity(size_t entity_id);
        void destroy_entity(size_t entity_id);
        bool has_entity(size_t entity_id) noexcept;
        void init(Interface& interface);
        void update(Interface& interface);
        ~Game();
    };
}

template <class t>
bool core::Entity::has_component() noexcept {
    return this->components.find(typeid(t).hash_code()) !=
           this->components.end();
}

template <class t>
void core::Entity::remove_component() {
    if (this->has_component<t>()) {
        this->components.erase(typeid(t).hash_code());
    } else {
        throw std::runtime_error("Tried to remove " +
                                 std::string(typeid(t).name()) +
                                 ", but it doesn't exist!");
    }
}

template <class T>
std::vector<std::unique_ptr<core::Component>>& core::Entity::get_component() {
    if (this->has_component<T>()) {
        return this->components.at(typeid(T).hash_code());
    } else {
        throw std::runtime_error("Tried to get " +
                                 std::string(typeid(T).name()) +
                                 ", but it doesn't exist!");
    }
}

template <class T>
inline T& core::Entity::get_single_component() {
    std::vector<std::unique_ptr<core::Component>>& components =
        this->get_component<T>();
    return (T&)*components.at(0);
}
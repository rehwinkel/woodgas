# woodgas
An ECS-Based game engine written in C++.

## Structure
At it's core, woodgas is an ECS (entity-component-system) engine. Entities are organized in scenes, hierarchically. Each entity has a number of components that define it's behavior. Every component is updated every tick and can use various APIs offered to render, play sound or handle inputs. These APIs are also exposed to lua such that a developer can write their game entirely in lua. No need for compilation.

## Todo
* [x] Write Top-Level ECS with support for Entites (transformation aware) and Components. (Scenes are just Entities at the Root of the project)
* [ ] Write Renderer API (2D rendering of textures / colored quads with location, rotation and scale). (Sprites define their size in pixels per unit)
* [ ] Write Sound API (2D spatial sound, support for looping sounds and oneshot sounds)
* [ ] Write Timing and Input API (accessing time information {via glfwGetTime}, Support for mouse, keyboard & maybe controllers). (Control in form of declaratively defined axis and triggers)
* [x] Write Asset management API (loading assets of any kind, using assets and uploading them to renderer, {custom asset types?})
* [x] Write Logging API (simple logging on multiple levels)
* [ ] Write Python integration (access to the APIs, declarative API for creating entities with components)
* [ ] Write bundler that assembles single executable from assets and code. (using compression, insert asset package into binary)
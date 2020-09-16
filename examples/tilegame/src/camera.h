#pragma once

#include <core/core.h>

class CameraComponent : public core::Component {
   private:
    float x, y;
    float aspect_ratio;

   public:
    CameraComponent(float aspect_ratio);
    virtual void init(core::Interface &interface);
    virtual void update(core::Interface &interface);
    virtual bool is_unique();
    float get_x();
    float get_y();
    float get_aspect_ratio();
    void move(float x, float y);
};
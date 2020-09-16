#include "camera.h"

bool CameraComponent::is_unique() { return true; }

CameraComponent::CameraComponent(float aspect_ratio)
    : x(0), y(0), aspect_ratio(aspect_ratio) {}

void CameraComponent::init(core::Interface &interface) {
    interface.get_renderer().upload_ortho(
        -1 * this->aspect_ratio, 1 * this->aspect_ratio, -1, 1, 0.1f, 100);
}

void CameraComponent::update(core::Interface &interface) {
    interface.get_renderer().upload_view(this->x, this->y, 0);
}

float CameraComponent::get_x() { return this->x; }

float CameraComponent::get_y() { return this->y; }

float CameraComponent::get_aspect_ratio() { return this->aspect_ratio; }

void CameraComponent::move(float x, float y) {
    this->x += x;
    this->y += y;
}
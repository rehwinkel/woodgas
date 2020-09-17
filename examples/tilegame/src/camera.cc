#include "components.h"

bool CameraComponent::is_unique() { return true; }

CameraComponent::CameraComponent(float aspect_ratio, float scale)
    : aspect_ratio(aspect_ratio), scale(scale) {}

void CameraComponent::init(core::Interface &interface) {
    interface.get_renderer().upload_ortho(
        -1 * this->aspect_ratio, 1 * this->aspect_ratio, -1, 1, 0.1f, 100);
}

void CameraComponent::update(core::Interface &interface) {
    interface.get_renderer().upload_view(
        0, 0, 0, 1.0f / this->scale);  // TODO: get x y from transfrom
}

float CameraComponent::get_aspect_ratio() { return this->aspect_ratio; }

float CameraComponent::get_scale() { return this->scale; }
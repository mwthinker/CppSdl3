#ifndef CPPSDL3_SDL_UTIL_GRAPHIC_H
#define CPPSDL3_SDL_UTIL_GRAPHIC_H

#include <glm/gtc/constants.hpp>
#include <glm/vec2.hpp>

#include <array>
#include <type_traits>

namespace sdl {

	constexpr auto Pi = glm::pi<float>();

	[[nodiscard]] glm::vec2 getHexagonCorner(int nbr, float startAngle = 0.f);

	[[nodiscard]] glm::vec2 getHexagonCorner(const glm::vec2& center, float size, int nbr, float startAngle = 0.f);

	[[nodiscard]] std::array<glm::vec2, 6> getHexagonCorners(const glm::vec2& center, float radius, float startAngle = 0.f);

}

#endif

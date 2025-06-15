#include "glm.h"

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace sdl {

	glm::vec2 getHexagonCorner(int nbr, float startAngle) {
		return glm::rotate(glm::vec2{1, 0.f}, Pi / 3 * nbr + startAngle);
	}

	glm::vec2 getHexagonCorner(const glm::vec2& center, float size, int nbr, float startAngle) {
		return center + size * getHexagonCorner(nbr, startAngle);
	}

	std::array<glm::vec2, 6> getHexagonCorners(const glm::vec2& center, float radius, float startAngle) {
		std::array<glm::vec2, 6> corners;
		for (int i = 0; i < 6; ++i) {
			corners[i] = getHexagonCorner(center, radius, i, startAngle);
		}
		return corners;
	}

}

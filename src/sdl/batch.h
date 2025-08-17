#ifndef CPPSDL3_SDL_BATCH_H
#define CPPSDL3_SDL_BATCH_H

#include "gpu.h"

#include <vector>
#include <initializer_list>
#include <cstdint>
#include <concepts>
#include <span>
#include <algorithm>

namespace sdl {

	template <VertexType Vertex>
	class Batch {
	public:
		Batch() = default;
		
		void startBatch() {
			index_ = static_cast<uint32_t>(vertices_.size());
		}

		void pushBack(const Vertex& vertex) {
			vertices_.push_back(vertex);
		}

		void insert(std::span<const Vertex> span) {
			vertices_.insert(vertices_.end(), span.begin(), span.end());
		}

		void insert(std::initializer_list<Vertex> list) {
			insert(std::span<const Vertex>(list.begin(), list.size()));
		}

		void pushBackIndex(uint32_t index) {
			indices_.push_back(index + index_);
		}

		void insertIndices(std::span<const uint32_t> indices) {
			std::transform(indices.begin(), indices.end(), std::back_inserter(indices_),
				[this](uint32_t index) { return index + index_; });
		}

		void insertIndices(std::initializer_list<uint32_t> localIndices) {
			insertIndices(std::span<const uint32_t>(localIndices.begin(), localIndices.size()));
		}

		void reserve(size_t vertexCount, size_t indexCount) {
			vertices_.reserve(vertexCount);
			indices_.reserve(indexCount);
		}

		void shrinkToFit() {
			vertices_.shrink_to_fit();
			indices_.shrink_to_fit();
		}

		void clear() {
			vertices_.clear();
			indices_.clear();
			index_ = 0;
		}

		std::span<const Vertex> vertices() const {
			return vertices_;
		}

		std::span<const uint32_t> indices() const {
			return indices_;
		}

	private:
		std::vector<Vertex> vertices_;
		std::vector<uint32_t> indices_;
		uint32_t index_ = 0;
	};

}

#endif

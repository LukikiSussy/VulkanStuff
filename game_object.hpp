#pragma once

#include <memory>

class GameObject {
public:
	using id_t = unsigned int;

	static GameObject createGameObject() {
		static id_t currentId = 0;

		return GameObject{ currentId++ };
	}

	GameObject(const GameObject&) = delete;
	GameObject& operator=(const GameObject&) = delete;
	GameObject(GameObject&&) = default;
	GameObject& operator=(GameObject&&) = default;

	id_t getId() {
		return id;
	}

private:
	GameObject(id_t objId) : id(objId) {

	}

	id_t id;
};
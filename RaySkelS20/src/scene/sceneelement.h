#pragma once

class Scene;

class SceneElement
{
public:
	virtual ~SceneElement() {}

	Scene* getScene() const { return scene; }

protected:
	SceneElement(Scene* s)
		: scene(s) {}

	Scene* scene;
};

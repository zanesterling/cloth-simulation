#pragma once

class UI {
public:
	int width, height;

	UI(int w, int h) : width(w), height(h) {}

	void render();
};

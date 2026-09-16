#pragma once

enum QuadMode {
	kNone = 0,
	kForward,
	kBackward,
	kRandom,
};


class QuadDispatch {
public:
	QuadDispatch() {

	}
	~QuadDispatch() {

	}

	void moveForward() {
		currentIndex++;
		if (currentIndex >= 4) {
			currentIndex = 0;
		}
	}

	int getIndex() {
		return currentIndex;
	}
	
private:
	int currentIndex = 0;


};
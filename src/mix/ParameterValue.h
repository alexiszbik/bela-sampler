#pragma once

struct ParameterValue {
	float value = 0.f;
	bool valueHasChanged = true;

	void setValue(float newValue) {
		if(value != newValue) {
			value = newValue;
			valueHasChanged = true;
		}
	}

	float getValue() {
		valueHasChanged = false;
		return value;
	}
};

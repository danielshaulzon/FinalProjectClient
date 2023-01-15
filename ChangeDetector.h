template <typename T>
/**
 * @brief This template class is used to detect when a value has changed since the last time it was checked.
 * \n It is used to declare a private member variable of that type.
 * @tparam T The type of the value to be monitored.
 */
class ChangeDetector {
public:
	/**
	 * @brief Construct a new Change Detector object.
	 * @param initialValue The initial value to be monitored.
	 */
	ChangeDetector(T initialValue) : _value(initialValue) {}

	// Return true if the value has changed since the last time this function was called.
	bool changed(T newValue) {
		// checks if the existing value is not equal to the new value.
		if (_value != newValue) {
			// if it is not equal it sets the existing value to the new value.
			_value = newValue;
			return true;
		}
		// if the values are equal it returns false.
		return false;
	}

	// Return the current value
	T getValue() const {
		return _value;
	}

private:
	// The current value
	T _value;
};
# SharedPubSubPy Documentation

## Overview
The SharedPubSubPy project provides a C++ library for inter-process communication using POSIX shared memory. It includes templated `Publisher` and `Subscriber` classes that allow for efficient data sharing between processes. This project also includes Pybind11 bindings to expose the integer versions of these classes to Python, enabling easy integration with Python applications.

## Features
- Lock-free inter-process communication.
- Event-driven notifications for subscribers.
- Direct access to the latest value in shared memory.
- Supports multiple subscribers to a single publisher.
- Persistent shared memory data, allowing processes to exit and re-enter without data loss.

## Requirements
- A POSIX environment (Most Linux distributions).
- C++20 or later.
- Python 3.x.
- Pybind11.

## Installation
1. Clone the repository:
   ```
   git clone https://github.com/YourUsername/SharedPubSubPy.git
   cd SharedPubSubPy
   ```

2. Create a build directory and compile the project:
   ```
   mkdir build && cd build
   cmake ..
   make
   ```

3. Install the Python bindings:
   ```
   pip install .
   ```

## Usage
### C++ Example
```cpp
#include <iostream>
#include <thread>
#include "SharedPubSub.hpp"

int main() {
    shps::Publisher<int> publisher("PubSub");
    int value = 0;

    while (true) {
        publisher.publish(++value);
        std::cout << "PUBLISHER: " << value << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
```

### Python Example
```python
import SharedPubSubPy

# Create a Publisher
publisher = SharedPubSubPy.Publisher("PubSub")

# Create a Subscriber
subscriber = SharedPubSubPy.Subscriber("PubSub", "Subscriber1", True)

# Publish and subscribe
publisher.publish(42)
value = subscriber.readWait()
print(f"SUBSCRIBER: {value}")
```

## Building the Pybind11 Bindings
To build the Pybind11 bindings, ensure that you have Pybind11 installed and included in your CMake configuration. The bindings are defined in `src/pybind_module.cpp`, which exposes the `Publisher<int>` and `Subscriber<int>` classes to Python.

## Contributing
Contributions are welcome! Please submit a pull request or open an issue for any enhancements or bug fixes.

## License
This project is licensed under the MIT License. See the LICENSE file for details.
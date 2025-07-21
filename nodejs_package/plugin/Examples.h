#include <atomic>
#include <iostream>
using namespace std;

class ExampleClass{
    public:
        int value1 = 0;
        float value2 = 0.0;
        long int value3 = 42;
        
        void printValues() const {
            cout << "EXAMPLE CLASS : " << dec << 
            value1 << ", " <<
            value2 << ", " <<
            value3 << "" <<
            endl;
        };

        // Comparison operators
        auto operator<=>(const ExampleClass&) const = default;
        bool operator==(const ExampleClass&) const = default;
};

// To make atomic variable works in structs or objects,
// You need to manually declare all the constructors and operators
class ExampleClassAtomic{
    public:
        atomic<int> value1 = 0;
        atomic<float> value2 = 0.0;
        atomic<long> value3 = 42;

        // Defaults constructor/destructor
        ExampleClassAtomic() = default;
        ~ExampleClassAtomic() = default;

        void printValues() const{
            cout << "EXAMPLE CLASS ATOMIC : " << dec << 
            value1.load() << ", " <<
            value2.load() << ", " <<
            value3.load() << "" <<
            endl;
        };
        
        //Need getter and setter for python compatibility
        int getValue1() const{
            return value1.load();
        }
        float getValue2() const{
            return value2.load();
        }
        long getValue3() const{
            return value3.load();
        }

        void setValue1(const int& input){
            value1.store(input);
        }
        void setValue2(const float& input){
            value2.store(input);
        }
        void setValue3(const long& input){
            value3.store(input);
        }

        // Copy constructor
        ExampleClassAtomic(const ExampleClassAtomic& other):
            value1(other.value1.load()), // Atomic uses load() to get the value
            value2(other.value2.load()), 
            value3(other.value3.load()){} 

        // Copy assignment
        ExampleClassAtomic& operator=(const ExampleClassAtomic& other) {
            if (this != &other) {
                value1 = other.value1.load();
                value2 = other.value2.load();
                value3 = other.value3.load();
            }
            return *this;
        }

        // All operators
        bool operator==(const ExampleClassAtomic& other) const {
            return (value1.load() == other.value1.load() && 
                    value2.load() == other.value2.load() && 
                    value3.load() == other.value3.load());
        }

        bool operator!=(const ExampleClassAtomic& other) const {
            return !(*this == other);
        }
        bool operator<=(const ExampleClassAtomic& other) const {
            return (value1.load() <= other.value1.load() && 
                    value2.load() <= other.value2.load() && 
                    value3.load() <= other.value3.load());
        }
        bool operator>=(const ExampleClassAtomic& other) const {
            return (value1.load() >= other.value1.load() && 
                    value2.load() >= other.value2.load() && 
                    value3.load() >= other.value3.load());
        }
};
#include <string>
#include <cstring>
#include <ostream>

using namespace std;

// This class is not thread safe, reading in a queue is recommended
template<size_t N>
class FixedString {
    public:

        FixedString(){}

        FixedString(string str){
            size_ = snprintf(data,sizeof(data), "%s",str.c_str());
        }

        string get() const {
            return string(data);
        }

        void set(string str){
            size_ = snprintf(data,sizeof(data), "%s",str.c_str());
        }

        void set(const char* str){
            size_ = snprintf(data,sizeof(data), "%s",str);
        }

        size_t size(){
            return size_;
        }

        bool operator==(const FixedString<N>& other) const {
            return strncmp(data, other.data, N) == 0;
        }
        bool operator==(const string& str) const {
            return strncmp(data, str, N) == 0;
        }
        bool operator==(const char* c) const {
            return strncmp(data, c, N) == 0;
        }
        
        bool operator!=(const FixedString<N>& other) const {
            return strncmp(data, other.data, N) != 0;
        }
        bool operator!=(const string& str) const {
            return strncmp(data, str, N) != 0;
        }
        bool operator!=(const char* c) const {
            return strncmp(data, c, N) != 0;
        }
        
        FixedString<N>& operator=(const FixedString<N>& other) {
            if (this != &other) {
                set(other.get());
            }
            return *this;
        }

        FixedString<N>& operator=(const string& str) {
            set(str);
            return *this;
        }
        FixedString<N>& operator=(const char* c) {
            set(c);
            return *this;
        }
        ~FixedString() = default;

    private:
        char data[N] = {'\0'};
        size_t size_ = 0;
};

template<size_t N>
std::ostream& operator<<(std::ostream& os, const FixedString<N>& fs) {
    os << fs.get();
    return os;
}
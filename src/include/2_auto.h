
struct Outer
{
    struct Inner {
        operator int() const {
            return 0;
        }
    };

    Inner getInner() const { return {}; }
};

struct CheckInner
{
    bool value;

    CheckInner(Outer::Inner) : value {true} {}

    CheckInner(int a) : value{false} {}

};

template<typename T, template <class> class Container>
T autoSum(Container<T>& container)
{
    T result = 0;
    // here using auto allows us to know nothing about iterator type.
    // we imply that Container is iterable
    for (auto& i : container)
    {
        result += i;
    }
    return result;
}
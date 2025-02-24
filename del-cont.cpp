


class dc {
public:
    dc(){}

    template <typename ReturnType,typename... Args>
    void call_function(ReturnType (*func)(Args...),Args... args) {
        func(args...);
    }

    template <typename ReturnType,typename... Args, int t>
    ReturnType dcAlgorithm(Quiver<t> Q, ReturnType (*func)(Args...), Args... args){
        
    }
};
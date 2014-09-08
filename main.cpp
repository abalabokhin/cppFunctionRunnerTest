#include <iostream>
#include <functional>

struct RunningSettings {
    int maxI;
    int b;
};

template <class Fn> class FunctionRunner
{
public:
    FunctionRunner()
    {}

    FunctionRunner(Fn * fn)
            : function(fn)
    {}

    template<typename... Values> void Run(RunningSettings rs, Values... values) {
        for (int i = 0; i < rs.maxI; ++i) {
            function(values..., RunningSettings{i, 0});
        }
    }

private:
    Fn * function;
};

template <class Fn> class RSFunctionRunner
{
public:
    RSFunctionRunner(Fn * fn)
            : function(fn)
    {}

    template<typename... Values> void Run(RunningSettings rs, Values... values) {
        std::function<void(RunningSettings)> newFn = AddArgs(std::function<Fn>(function), values...);
        for (int i = 0; i < rs.maxI; ++i) {
            newFn(RunningSettings{i, 0});
        }
    }

private:
    std::function<void(RunningSettings)> AddArgs(std::function<void(RunningSettings)> fn) {
        return fn;
    }

    template<typename T, typename... Tail> std::function<void(Tail... tail, RunningSettings)> AddArgs(std::function<void(T, Tail... tail, RunningSettings)> fn, T head, Tail... tail) {
        return AddArgs(SetArg(fn, head), tail...);
    }

    template <typename T, typename... Tail> std::function<void(Tail... tail, RunningSettings)> SetArg(std::function<void(T, Tail... tail, RunningSettings)> fn, T value) {
        return std::bind(fn, std::placeholders::_1, value);
    }

    Fn * function;
};

template <class Fn> class IntFunctionRunner
{
public:
    IntFunctionRunner(Fn * fn)
            : function(fn)
    {}

    template<typename... Values> void Run(int rs, Values... values) {
        std::function<void(int)> newFn = AddArgs(std::function<Fn>(function), values...);
        for (int i = 0; i < rs; ++i) {
            newFn(i);
        }
    }

private:
    std::function<void(int)> AddArgs(std::function<void(int)> fn) {
        return fn;
    }

    template<typename T, typename... Tail> std::function<void(Tail... tail, int)> AddArgs(std::function<void(T, Tail... tail, int)> fn, T head, Tail... tail) {
        return AddArgs(SetArg(fn, head), tail...);
    }

    template <typename T, typename... Tail> std::function<void(Tail... tail, int)> SetArg(std::function<void(T, Tail... tail, int)> fn, T value) {
        return std::bind(fn, std::placeholders::_1, value);
    }

    Fn * function;
};

void funcA(int a, int b, std::string const & c, RunningSettings i) {
    std::cout << "funcA " << a + b << std::endl;
}

void funcB(int a, RunningSettings i) {
    std::cout << "funcB " << a << std::endl;
}

void funcC(int a, int i) {
    std::cout << "funcC " << a << std::endl;
}

int main() {
    FunctionRunner<decltype(funcA)> runnerA(funcA);
    runnerA.Run(RunningSettings{3, 1}, 3, 4, "fg");

    RSFunctionRunner<decltype(funcB)> runnerB_(funcB);
    runnerB_.Run(RunningSettings{2, 1}, 3);

    IntFunctionRunner<decltype(funcC)> runnerC_(funcC);
    runnerC_.Run(2, 3);
    return 0;
}

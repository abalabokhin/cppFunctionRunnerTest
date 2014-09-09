#include <iostream>
#include <functional>

struct RunningSettings {
    int maxI;
    int b;
};

using KernelRunningSettings = RunningSettings;

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

    template<typename ArgsT, typename FuncArgsT, typename... ArgsTail, typename... FuncArgsTail>
    std::function<void(FuncArgsTail... funcTail, RunningSettings)> AddArgs(std::function<void(FuncArgsT, FuncArgsTail... funcTail, RunningSettings)> fn, ArgsT head, ArgsTail... tail) {
        return AddArgs(SetArg(fn, head), tail...);
    }

    template <typename ArgsT, typename FuncArgsT, typename... Tail>
    std::function<void(Tail... tail, RunningSettings)> SetArg(std::function<void(FuncArgsT, Tail... tail, RunningSettings)> fn, ArgsT value) {
        return std::bind(fn, value, std::placeholders::_1);
    }

    Fn * function;
};

void funcA(int a, int b/*, std::string const & c*/, RunningSettings i) {
    std::cout << "funcA " << a + b << std::endl;
}

void funcB(int a, RunningSettings i) {
    std::cout << "funcB " << a << std::endl;
}

int main() {
    FunctionRunner<decltype(funcA)> runnerA(funcA);
    runnerA.Run(RunningSettings{3, 1}, 3, 4);

    RSFunctionRunner<decltype(funcA)> runnerB_(funcA);
    int a = 6;
    runnerB_.Run(RunningSettings{2, 1}, a, 34);

    return 0;
}
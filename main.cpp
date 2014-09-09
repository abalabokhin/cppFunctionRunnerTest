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
            function(RunningSettings{i, 0}, values...);
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
    std::function<void(RunningSettings)> AddArgs(std::function<void(RunningSettings, FuncArgsT, FuncArgsTail... funcTail)> fn, ArgsT head, ArgsTail... tail) {
        return AddArgs(SetArg(fn, head), tail...);
    }

    template <typename ArgsT, typename FuncArgsT, typename... Tail>
    std::function<void(RunningSettings, Tail... tail)> SetArg(std::function<void(RunningSettings, FuncArgsT, Tail... tail)> fn, ArgsT value) {
        if (sizeof...(Tail) == 0)
            return std::bind(fn, std::placeholders::_1, value);
        //if (sizeof...(Tail) == 1)
//            return std::bind(fn, std::placeholders::_1, value, std::placeholders::_3);
//        if (sizeof...(Tail) == 2)
//            return std::bind(fn, std::placeholders::_1, value);
    }

    Fn * function;
};

void funcA(RunningSettings i, int a, int b/*, std::string const & c*/) {
    std::cout << "funcA " << a + b << std::endl;
}

void funcB(RunningSettings i, int a) {
    std::cout << "funcB " << a << std::endl;
}

void funcC(int i, int a) {

}

/*std::function<void(RunningSettings i, int a)> Bind() {
    return std::bind(funcA, std::placeholders::_1, std::placeholders::_2, 12);

}*/

int main() {
    FunctionRunner<decltype(funcA)> runnerA(funcA);
    runnerA.Run(RunningSettings{3, 1}, 3, 4);

//    RSFunctionRunner<decltype(funcA)> runnerA_(funcA);
    int a = 6;
//    runnerA_.Run(RunningSettings{2, 1}, a, 4);

    RSFunctionRunner<decltype(funcB)> runnerB_(funcB);
    runnerB_.Run(RunningSettings{2, 1}, a);



    return 0;
}
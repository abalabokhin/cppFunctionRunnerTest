#include <iostream>
#include <functional>

struct RunningSettings {
    int maxI;
    int b;
};

using KernelRunningSettings = RunningSettings;

template <class Fn> class DirectFunctionRunner
{
public:
    DirectFunctionRunner()
    {}

    DirectFunctionRunner(Fn * fn)
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

template <class Fn> class DoubleIntFunctionRunner
{
public:
    DoubleIntFunctionRunner(Fn * fn)
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

    /// General SetArg functions for different number of parameters for any types.
    template <typename ArgsT, typename FAT1>
    std::function<void(RunningSettings)> SetArg(std::function<void(RunningSettings, FAT1)> fn, ArgsT value) {
        return std::bind(fn, std::placeholders::_1, value);
    }

    template <typename ArgsT, typename FAT1, typename FAT2>
    std::function<void(RunningSettings, FAT2)> SetArg(std::function<void(RunningSettings, FAT1, FAT2)> fn, ArgsT value) {
        return std::bind(fn, std::placeholders::_1, value, std::placeholders::_2);
    }

    template <typename ArgsT, typename FAT1, typename FAT2, typename FAT3>
    std::function<void(RunningSettings, FAT2, FAT3)> SetArg(std::function<void(RunningSettings, FAT1, FAT2, FAT3)> fn, ArgsT value) {
        return std::bind(fn, std::placeholders::_1, value, std::placeholders::_2, std::placeholders::_3);
    }

    template <typename ArgsT, typename FAT1, typename FAT2, typename FAT3, typename FAT4>
    std::function<void(RunningSettings, FAT2, FAT3, FAT4)> SetArg(std::function<void(RunningSettings, FAT1, FAT2, FAT3, FAT4)> fn, ArgsT value) {
        return std::bind(fn, std::placeholders::_1, value, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    }

    /// SetArg functions for int as a first parameter and different number of other parameters of other types.
    std::function<void(RunningSettings)> SetArg(std::function<void(RunningSettings, int)> fn, int value) {
        return std::bind(fn, std::placeholders::_1, value * 2);
    }

    template <typename FAT2>
    std::function<void(RunningSettings, FAT2)> SetArg(std::function<void(RunningSettings, int, FAT2)> fn, int value) {
        return std::bind(fn, std::placeholders::_1, value * 2, std::placeholders::_2);
    }

    template <typename FAT2, typename FAT3>
    std::function<void(RunningSettings, FAT2, FAT3)> SetArg(std::function<void(RunningSettings, int, FAT2, FAT3)> fn, int value) {
        return std::bind(fn, std::placeholders::_1, value * 2, std::placeholders::_2, std::placeholders::_3);
    }

    template <typename FAT2, typename FAT3, typename FAT4>
    std::function<void(RunningSettings, int, FAT3, FAT4)> SetArg(std::function<void(RunningSettings, int, FAT2, FAT3, FAT4)> fn, int value) {
        return std::bind(fn, std::placeholders::_1, value * 2, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
    }
    /// You can add more, depends on how many arguments your functions can have

    Fn * function;
};

void funcA(RunningSettings i, int a, int b, std::string const & c) {
    std::cout << "funcA " << a + b << std::endl;
}

void funcB(RunningSettings i, int a) {
    std::cout << "funcB " << a << std::endl;
}

int main() {
    DirectFunctionRunner<decltype(funcA)> directRunner(funcA);
    directRunner.Run(RunningSettings{3, 1}, 7, 5, "fg");

    DoubleIntFunctionRunner<decltype(funcB)> runnerB(funcB);
    runnerB.Run(RunningSettings{2, 1}, 6);

    DoubleIntFunctionRunner<decltype(funcA)> runnerA(funcA);
    runnerA.Run(RunningSettings{2, 1}, 7, 5, "df");

    return 0;
}

/// How it should look like with variadic templates
/*class clGpuKernel : public cl::Kernel
{
public:
    clGpuKernel(const cl::Program &program, const cl::CommandQueue &cq, const char *name)
            :   cl::Kernel(program, name), defaultCq(&cq)
    {}

    clGpuKernel()
    {}

    template<typename... Values> void RunKernel(KernelRunningSettings const & runningSettings, Values... values) {
        SetAllArgs(values...);
        defaultCq->enqueueNDRangeKernel(*this, runningSettings.GetOffsetClNDRange(), runningSettings.GetGlobalClNDRange(), runningSettings.GetLocalClNDRange());
        defaultCq->finish();
    }

    template<typename... Values> void SetAllArgs(Values... values) {
        iArg = 0;
        AddArgs(values...);
    }

private:
    void AddArgs() {
        SetArg(0);
    }

    template<typename T, typename... Tail> void AddArgs(T head, Tail... tail) {
        SetArg(head);
        AddArgs(tail...);
    }

    template <typename T> void SetArg(T value) {
        setArg(iArg++, value);
    }

    template <typename T> void SetArg(CpuGpuBuffer<T> * value) {
        setArg(iArg++, value->getGpuBuffer());
    }

    int iArg = 0;
    const cl::CommandQueue * defaultCq;
};
*/
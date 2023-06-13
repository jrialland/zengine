#include "BackTrace.hpp"

#define BACKWARD_HAS_BFD 1
#include <backward.hpp>

static backward::SignalHandling sh;

void print_backtrace(int skip)
{
    using namespace backward;

    StackTrace st;
    st.load_here(32);
    st.skip_n_firsts(3+skip);

    TraceResolver tr;
    tr.load_stacktrace(st);

    Printer p;
    p.object = true;
    p.color_mode = ColorMode::always;
    p.address = true;
    p.print(st);

}
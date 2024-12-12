#ifndef OVERRIDE
#  error OVERRIDE is not defined
#endif
#if OVERRIDE != 1
#  error OVERRIDE has the wrong value
#endif

#ifndef ONLY_IN_C
#  error ONLY_IN_C is not defined in C sources
#endif
#ifdef ONLY_IN_CXX
#  error ONLY_IN_CXX is defined in C sources
#endif

#if !NOVALUE
#  error NOVALUE evaluated as a Boolean is not true
#endif

#ifndef EMPTYVALUE
#  error EMPTYVALUE is not defined
#endif

#if 3 - EMPTYVALUE - 3 != 6
#  error EMPTYVALUE is not empty
#endif

void foo()
{
#ifndef __CC_ARM
#  error __CC_ARM not defined
#endif
}

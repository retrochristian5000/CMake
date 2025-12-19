#define _STR(x) #x
#define STR(x) _STR(x)

enum vocalization
{
  meow,  // tame cat
  roar,  // wild cat
  whine, // tame dog
  growl, // wild dog
};

static_assert(NOISE == EXPECTED,
              "expected " STR(EXPECTED) ", got " STR(NOISE));

int main()
{
  return 0;
}


constexpr int func(int A, int B)
{
  // Verify that we have at least c++14
  if (A < B) {
    return A + B;
  } else {
    return B * A;
  }
}

proctype euclid(int a, b) {
    int x = a;
    int y = b;
    assert(a > 0 && b > 0);
    do
    :: x > y -> x = x - y
    :: x < y -> y = y - x
    :: else -> break
    od
    printf("gcd(%d, %d) = %d\n", a, b, x);
}

init {
    run euclid(25, 5)
    run euclid(2312,134)
    run euclid(13, 32423)
}

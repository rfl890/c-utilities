#include <errno.h>
#include <stdint.h>
#include <immintrin.h>
#include <limits.h>
#include <stdio.h>

#ifdef __GNUC__
#define LOG2(X)                                                                \
((uint32_t)(8 * sizeof(unsigned long long) - __builtin_clzll((X)) - 1))
#else
#ifdef _MSC_VER
#include <intrin.h>
inline uint32_t LOG2(uint64_t X) {
    unsigned long out;
    _BitScanReverse64(&out, X);
    return out;
}
#endif
#endif

static const char *usage = "usage: %s [SIDES]"
                           "\n";

uint64_t get_closest_bitstring(uint64_t x) {
    uint32_t bits = LOG2(x);
    return ((uint64_t)1 << (bits + 1)) - 1;
}

uint64_t rdrand_random() {
    uint64_t random;
    while (!_rdrand64_step((unsigned long long *)&random)) {}
    return random;
}

uint64_t diceroll(uint64_t sides) {
    sides = sides - 1;
    uint64_t bitstring = get_closest_bitstring(sides);
    uint64_t random = rdrand_random() & bitstring;
    while (random > sides) {
        random = rdrand_random() & bitstring;
    }
    return random + 1;
}

int main(int argc, char **argv) {
    int64_t sides = 0;
    if (argc < 2) {
        sides = 6;
    }
    else {
        char *endptr;
        sides = strtoll(argv[1], &endptr, 10);
        if (endptr == argv[1] || *endptr != '\0' || ((sides == LLONG_MIN || sides == LLONG_MAX) && errno == ERANGE)) {
            fprintf(stderr, usage, argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    fprintf(stderr, "Rolling a %llu-sided die...", (unsigned long long)sides);
    printf("You rolled %llu!\n", (unsigned long long)diceroll(sides));
}

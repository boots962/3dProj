#pragma once

#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>
#include <vector>

class Perlin {
private:
    // Removed 'static const' so each Perlin object can have its own unique array
    int p[512]; 

    static float fade(float t) {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    static float lerp(float t, float a, float b) {
        return a + t * (b - a);
    }

    static float grad(int hash, float x, float y) {
        int h = hash & 15;
        float u = h < 8 ? x : y;
        float v = h < 4 ? y : h == 12 || h == 14 ? x : 0.0f;
        return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
    }

public:
    // CONSTRUCTOR: This runs once when you create the Perlin object
    Perlin(unsigned int seed) {
        // 1. Fill a temporary vector with numbers 0 to 255
        std::vector<int> permutation(256);
        std::iota(permutation.begin(), permutation.end(), 0);

        // 2. Shuffle those numbers using the provided seed
        std::mt19937 engine(seed);
        std::shuffle(permutation.begin(), permutation.end(), engine);

        // 3. Copy the shuffled numbers into our p[] array TWICE 
        // (Duplicating it prevents out-of-bounds errors during noise generation)
        for (int i = 0; i < 256; i++) {
            p[i] = permutation[i];
            p[i + 256] = permutation[i];
        }
    }

    // Removed 'static' because it now needs to read this specific object's p[] array
    float noise(float x, float y) {
        int X = (int)floor(x) & 255;
        int Y = (int)floor(y) & 255;

        x -= floor(x);
        y -= floor(y);

        float u = fade(x);
        float v = fade(y);

        int A = p[X] + Y, AA = p[A], AB = p[A + 1];
        int B = p[X + 1] + Y, BA = p[B], BB = p[B + 1];

        return lerp(v, lerp(u, grad(p[AA], x, y), grad(p[BA], x - 1, y)), 
                       lerp(u, grad(p[AB], x, y - 1), grad(p[BB], x - 1, y - 1)));
    }
};
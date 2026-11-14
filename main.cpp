#include "raylib.h"
#include <array>
#include <vector>
#include <deque>
#include <cmath>
#include <omp.h>

using State = std::array<double, 4>;

const double g = 9.81, m1 = 1.0, m2 = 1.0, L1 = 1.0, L2 = 1.0;
const double SCALE = 180.0;

struct Pendul {
    State x;
    Vector2 p1, p2;
    std::deque<Vector2> urma;
    Color culoare;
};

State f_pendul(const State& x) {
    double t1 = x[0], t2 = x[1], w1 = x[2], w2 = x[3];
    double delta = t1 - t2;
    double den = 2.0 * m1 + m2 - m2 * std::cos(2.0 * t1 - 2.0 * t2);

    double dw1 = (-g * (2.0 * m1 + m2) * std::sin(t1) 
                  - m2 * g * std::sin(t1 - 2.0 * t2) 
                  - 2.0 * std::sin(delta) * m2 * (w2 * w2 * L2 + w1 * w1 * L1 * std::cos(delta))) 
                 / (L1 * den);

    double dw2 = (2.0 * std::sin(delta) * (w1 * w1 * L1 * (m1 + m2) 
                  + g * (m1 + m2) * std::cos(t1) 
                  + w2 * w2 * L2 * m2 * std::cos(delta))) 
                 / (L2 * den);

    return {w1, w2, dw1, dw2};
}

State rk4_step(const State& x, double h) {
    auto add = [](const State& a, const State& b, double s) -> State {
        return {a[0] + s * b[0], a[1] + s * b[1], a[2] + s * b[2], a[3] + s * b[3]};
    };

    State k1 = f_pendul(x);
    State k2 = f_pendul(add(x, k1, h * 0.5));
    State k3 = f_pendul(add(x, k2, h * 0.5));
    State k4 = f_pendul(add(x, k3, h));

    return {
        x[0] + (h / 6.0) * (k1[0] + 2.0 * k2[0] + 2.0 * k3[0] + k4[0]),
        x[1] + (h / 6.0) * (k1[1] + 2.0 * k2[1] + 2.0 * k3[1] + k4[1]),
        x[2] + (h / 6.0) * (k1[2] + 2.0 * k2[2] + 2.0 * k3[2] + k4[2]),
        x[3] + (h / 6.0) * (k1[3] + 2.0 * k2[3] + 2.0 * k3[3] + k4[3])
    };
}

int main() {
    const int W = 900, H = 900;
    InitWindow(W, H, "Efectul Fluturelui - C++ & OpenMP");
    SetTargetFPS(60);

    const int M = 500;
    const double h = 0.005;
    const int sub_steps = 3;
    const size_t lungime_urma = 60;
    Vector2 origine = { W / 2.0f, H / 2.0f };

    std::vector<Pendul> pendule(M);
    for (int i = 0; i < M; ++i) {
        pendule[i].x = { 2.0 * PI / 3.0, 2.0 * PI / 3.0 + i * 1e-5, 0.0, 0.0 };
        pendule[i].culoare = ColorFromHSV((float)i / M * 300.0f, 0.85f, 1.0f);
    }

    while (!WindowShouldClose()) {
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < M; ++i) {
            for (int s = 0; s < sub_steps; ++s) {
                pendule[i].x = rk4_step(pendule[i].x, h);
            }

            float x1 = origine.x + (float)(L1 * std::sin(pendule[i].x[0]) * SCALE);
            float y1 = origine.y + (float)(L1 * std::cos(pendule[i].x[0]) * SCALE);
            float x2 = x1 + (float)(L2 * std::sin(pendule[i].x[1]) * SCALE);
            float y2 = y1 + (float)(L2 * std::cos(pendule[i].x[1]) * SCALE);

            pendule[i].p1 = {x1, y1};
            pendule[i].p2 = {x2, y2};

            pendule[i].urma.push_back(pendule[i].p2);
            if (pendule[i].urma.size() > lungime_urma) {
                pendule[i].urma.pop_front();
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < M; ++i) {
            Color c_urma = Fade(pendule[i].culoare, 0.35f);
            for (size_t j = 1; j < pendule[i].urma.size(); ++j) {
                DrawLineV(pendule[i].urma[j - 1], pendule[i].urma[j], c_urma);
            }
        }

        for (int i = 0; i < M; ++i) {
            Color c_brat = Fade(pendule[i].culoare, 0.7f);
            DrawLineV(origine, pendule[i].p1, c_brat);
            DrawLineV(pendule[i].p1, pendule[i].p2, c_brat);
            DrawCircleV(pendule[i].p2, 2.5f, pendule[i].culoare);
        }

        DrawFPS(15, 15);
        DrawText(TextFormat("Pendule: %d | Thread-uri OpenMP: %d", M, omp_get_max_threads()), 15, 40, 20, WHITE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

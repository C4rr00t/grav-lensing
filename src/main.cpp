#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

using namespace sf;

const double G = 6.67430e-11;
const double c = 299792458.0;
const double METERS_PER_PIXEL = 1e5;
const double DPHI_STEP = 0.001;
const int STEPS_PER_FRAME = 3;

struct BlackHole {
    Vector2f position; // pixels
    double mass;
    double r_s; // meters
    double photon_sphere; // meters

    BlackHole(Vector2f pos, double m) {
        position = pos;
        mass = m;
        r_s = (2.0 * G * mass) / (c * c);
        photon_sphere = 1.5 * r_s;
    }

    void draw(RenderWindow& window) {
        float display_radius = std::max(5.0f, static_cast<float>(r_s / METERS_PER_PIXEL));

        CircleShape circle(display_radius);
        circle.setFillColor(Color::Black);
        circle.setOutlineColor(Color::Red);
        circle.setOutlineThickness(2.0f);
        circle.setOrigin(Vector2f(display_radius, display_radius));
        circle.setPosition(position);
        window.draw(circle);

        float photon_display_radius = photon_sphere / METERS_PER_PIXEL;
        if (photon_display_radius > display_radius) {
            CircleShape photonCircle(photon_display_radius);
            photonCircle.setFillColor(Color::Transparent);
            photonCircle.setOutlineColor(Color(255, 255, 0, 100));
            photonCircle.setOutlineThickness(1.0f);
            photonCircle.setOrigin(Vector2f(photon_display_radius, photon_display_radius));
            photonCircle.setPosition(position);
            window.draw(photonCircle);
        }
    }
};

struct Ray {
    Vector2f position;
    std::vector<Vector2f> trail;

    double r, phi;
    double u;
    double du_dphi;
    double phi_direction;

    Vector2f blackHolePos;
    bool active = true;

    Ray(Vector2f pos, Vector2f bhPos) {
        position = pos;
        blackHolePos = bhPos;

        Vector2f relPosPixels = position - blackHolePos;

        double x = relPosPixels.x * METERS_PER_PIXEL;
        double y = relPosPixels.y * METERS_PER_PIXEL;

        r = std::hypot(x, y);
        phi = std::atan2(y, x);

        double impact = y;

        if (std::abs(impact) < 1.0) {
            active = false;
            return;
        }

        u = 1.0 / r;
        du_dphi = std::cos(phi) / impact;

        phi_direction = (impact < 0) ? 1.0 : -1.0;

        trail.push_back(position);
    }

    void draw(RenderWindow& window) {
        for (size_t i = 1; i < trail.size(); i++) {
            float fadeRatio = static_cast<float>(i) / trail.size();
            int alpha = static_cast<int>(255 * fadeRatio);
            const Vertex line[] = {
                Vertex(trail[i - 1], Color(255, 255, 255, alpha)),
                Vertex(trail[i], Color(255, 255, 255, alpha))
            };
            window.draw(line, 2, PrimitiveType::Lines);
        }

        if (active) {
            CircleShape point(2.0f);
            point.setFillColor(Color::White);
            point.setOrigin(Vector2f(1.0f, 1.0f));
            point.setPosition(position);
            window.draw(point);
        }
    }

    void step(double r_s) {
        if (!active) return;

        for (int i = 0; i < STEPS_PER_FRAME; i++) {
            double d2u_dphi2 = 1.5 * r_s * u * u - u;
            double dphi = phi_direction * DPHI_STEP;

            du_dphi += d2u_dphi2 * dphi;
            u += du_dphi * dphi;
            phi += dphi;

            r = 1.0 / u;

            if (r <= r_s) {
                active = false;
                return;
            }

            position.x = blackHolePos.x + static_cast<float>((r * std::cos(phi)) / METERS_PER_PIXEL);
            position.y = blackHolePos.y + static_cast<float>((r * std::sin(phi)) / METERS_PER_PIXEL);

            trail.push_back(position);

            if (trail.size() > 1000) {
                trail.erase(trail.begin());
            }

            if (position.x < -100 || position.x > 900 || position.y < -100 || position.y > 700) {
                active = false;
                return;
            }
        }
    }
};



int main() {
    RenderWindow window(VideoMode({800, 600}), "Gravitational Lensing");
    BlackHole black_hole(Vector2f(400, 300), 1.989e30 * 1000);
    std::vector<Ray> rays;

    for (float y = 150; y < 450; y += 15) {
        rays.push_back(Ray(Vector2f(50, y), black_hole.position));
    }

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        window.clear(Color(20, 20, 40));

        for (Ray& ray : rays) {
            ray.step(black_hole.r_s);
            ray.draw(window);
        }

        black_hole.draw(window);
        window.display();
    }
    return 0;
}

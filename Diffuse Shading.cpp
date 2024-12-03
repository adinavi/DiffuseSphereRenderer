#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>

struct Vec3 {
    double x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& v) const { return Vec3(x + v.x, y + v.y, z + v.z); }
    Vec3 operator-(const Vec3& v) const { return Vec3(x - v.x, y - v.z, z - v.z); }
    Vec3 operator*(double t) const { return Vec3(x * t, y * t, z * t); }
    Vec3 operator/(double t) const { return Vec3(x / t, y / t, z / t); }

    // Element-wise multiplication
    Vec3 operator*(const Vec3& v) const { return Vec3(x * v.x, y * v.y, z * v.z); }

    double dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 normalize() const {
        double len = std::sqrt(x * x + y * y + z * z);
        return *this / len;
    }
};

struct Ray {
    Vec3 origin, direction;

    Ray(const Vec3& origin, const Vec3& direction)
        : origin(origin), direction(direction.normalize()) {}
};

bool hit_sphere(const Vec3& center, double radius, const Ray& ray, double& t) {
    Vec3 oc = ray.origin - center;
    double a = ray.direction.dot(ray.direction);
    double b = 2.0 * oc.dot(ray.direction);
    double c = oc.dot(oc) - radius * radius;
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;
    t = (-b - std::sqrt(discriminant)) / (2.0 * a);
    return true;
}

Vec3 ray_color(const Ray& ray) {
    Vec3 sphere_center(0, 0, -1);
    double sphere_radius = 0.5;
    double t;

    Vec3 light_position(0, 1, -0.5);
    Vec3 light_color(1, 1, 1); // White light
    Vec3 sphere_color(1, 1, 0); // Yellow

    if (hit_sphere(sphere_center, sphere_radius, ray, t)) {
        Vec3 hit_point = ray.origin + ray.direction * t;
        Vec3 normal = (hit_point - sphere_center).normalize();
        Vec3 light_dir = (light_position - hit_point).normalize();
        double diffuse = std::max(0.0, normal.dot(light_dir));

        // Apply shading with element-wise multiplication
        return sphere_color * light_color * diffuse;
    }

    return Vec3(0, 0, 0); // Black background
}

void render_scene(const std::string& filename, int image_width, int image_height) {
    std::ofstream file(filename);
    file << "P3\n" << image_width << " " << image_height << "\n255\n";

    Vec3 origin(0, 0, 0);
    double viewport_height = 2.0;
    double viewport_width = 2.0 * image_width / image_height;
    double focal_length = 1.0;

    Vec3 horizontal(viewport_width, 0, 0);
    Vec3 vertical(0, viewport_height, 0);
    Vec3 lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focal_length);

    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            double u = double(i) / (image_width - 1);
            double v = double(j) / (image_height - 1);
            Ray ray(origin, lower_left_corner + horizontal * u + vertical * v - origin);
            Vec3 color = ray_color(ray);
            file << static_cast<int>(255.999 * color.x) << " "
                << static_cast<int>(255.999 * color.y) << " "
                << static_cast<int>(255.999 * color.z) << "\n";
        }
    }

    file.close();
}

int main() {
    const int image_width = 400;
    const int image_height = 400;
    render_scene("output.ppm", image_width, image_height);
    return 0;
}

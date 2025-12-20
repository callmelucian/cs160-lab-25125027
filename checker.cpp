#include <bits/stdc++.h>
using namespace std;

// --- Structures & Geometry Helpers ---

struct Point {
    double x, y;
};

double dist_sq(Point p1, Point p2) {
    return (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y);
}

double dist(Point p1, Point p2) {
    return std::sqrt(dist_sq(p1, p2));
}

// Shortest distance from point P to segment AB
double point_to_segment_dist(Point p, Point a, Point b) {
    double l2 = dist_sq(a, b);
    if (l2 == 0.0) return dist(p, a);

    // Project p onto the line defined by ab, clamped between 0 and 1
    double t = ((p.x - a.x) * (b.x - a.x) + (p.y - a.y) * (b.y - a.y)) / l2;
    t = std::max(0.0, std::min(1.0, t));

    Point projection = { a.x + t * (b.x - a.x), a.y + t * (b.y - a.y) };
    return dist(p, projection);
}

// Check if a point is within 'eps' distance of ANY segment in the polyline
bool is_near_polyline(Point p, const std::vector<Point>& polyline, double eps) {
    for (size_t i = 0; i + 1 < polyline.size(); ++i) {
        if (point_to_segment_dist(p, polyline[i], polyline[i+1]) <= eps) {
            return true;
        }
    }
    return false;
}

// --- File I/O ---

std::vector<Point> read_trajectory(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Point> path;
    if (!file.is_open()) return path;

    int K;
    if (file >> K) {
        double x, y;
        for (int i = 0; i < K; ++i) {
            file >> x >> y;
            path.push_back({x, y});
        }
    }
    return path;
}

double get_path_length(const std::vector<Point>& path) {
    double len = 0.0;
    for (size_t i = 0; i + 1 < path.size(); ++i) {
        len += dist(path[i], path[i+1]);
    }
    return len;
}

// --- IoU Logic ---

// Calculates how much of 'path_a' is within 'eps' of 'path_b'
double calculate_covered_length(const std::vector<Point>& path_a, const std::vector<Point>& path_b, double eps) {
    double covered_len = 0.0;
    for (size_t i = 0; i + 1 < path_a.size(); ++i) {
        Point p1 = path_a[i];
        Point p2 = path_a[i+1];
        
        // Check midpoint to approximate if the segment is "close"
        Point mid = { (p1.x + p2.x) / 2.0, (p1.y + p2.y) / 2.0 };
        
        if (is_near_polyline(mid, path_b, eps)) {
            covered_len += dist(p1, p2);
        }
    }
    return covered_len;
}

int main() {
    std::string output_file = "./build/output.txt";
    std::string answer_file = "./build/answer.txt";
    double EPS = 500.0; // Adjustable tolerance

    std::vector<Point> traj_out = read_trajectory(output_file);
    std::vector<Point> traj_ans = read_trajectory(answer_file);

    if (traj_out.empty() || traj_ans.empty()) {
        std::cerr << "Error reading files." << std::endl;
        return 1;
    }

    double len_out = get_path_length(traj_out);
    double len_ans = get_path_length(traj_ans);

    // Symmetric Intersection: Average of (Out near Ans) and (Ans near Out)
    // This handles the "noisy length" issue better than a one-way check.
    double len_out_near_ans = calculate_covered_length(traj_out, traj_ans, EPS);
    double len_ans_near_out = calculate_covered_length(traj_ans, traj_out, EPS);
    
    // Using the max or average helps stabilize IoU against the zigzag noise length
    double intersection = (len_out_near_ans + len_ans_near_out) / 2.0;

    double union_len = len_out + len_ans - intersection;
    double iou = (union_len > 1e-9) ? (intersection / union_len) : 0.0;

    // std::cout << "Length Output: " << len_out << "\n";
    // std::cout << "Length Answer: " << len_ans << "\n";
    // std::cout << "Intersection (approx): " << intersection << "\n";
    // std::cout << "IoU: " << iou << std::endl;

    ofstream ouf("./build/verdict.txt");
    ouf << fixed << setprecision(6) << iou << "\n";
    ouf.close();

    return 0;
}
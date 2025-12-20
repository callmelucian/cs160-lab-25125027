#include <bits/stdc++.h>
using namespace std;

/// CONSTANTS
const double eps = 1e-6; // an arbritary small number
const double oo = 1e9; // an arbritary large number

/// HELPER FUNCTIONS
double squared (double a) { return a * a; }

/// POINT STRUCT TO REPRESENT 2D POINTS AND VECTORS
struct Point {
    double x, y;
    Point() : x(0), y(0) {}
    Point (double x, double y) : x(x), y(y) {}

    Point operator + (const Point &o) const { return Point(x + o.x, y + o.y); }
    Point operator - (const Point &o) const { return Point(x - o.x, y - o.y); }
    Point operator * (const double &scalar) const { return Point(x * scalar, y * scalar); }

    bool operator == (const Point &o) const { return abs(x - o.x) < eps && abs(y - o.y) < eps; }
    bool operator != (const Point &o) const { return abs(x - o.x) >= eps || abs(y - o.y) >= eps; }
    bool operator < (const Point &o) const { return (y == o.y ? x < o.x : y < o.y); }
    
    friend long double dot (const Point &a, const Point &b) { return a.x * b.x + a.y * b.y; }
    friend long double cross (const Point &a, const Point &b) { return a.x * b.y - a.y * b.x; }

    friend double euclideDist (const Point &a, const Point &b) { return sqrt(squared(a.x - b.x) + squared(a.y - b.y)); }
    friend double squaredDist (const Point &a, const Point &b) { return squared(a.x - b.x) + squared(a.y - b.y); }

    friend double pointOnLine (const Point &a, const Point &b, const Point &point) {
        Point x = a - point, y = b - point;
        return abs(cross(x, y)) < eps && dot(x, y) < eps;
    }

    friend istream& operator>> (istream &inf, Point &p) { return inf >> p.x >> p.y; }
    friend ostream& operator<< (ostream &ouf, const Point &p) { return ouf << "(" << p.x << ", " << p.y << ")"; }
};

/// SEGMENT STRUCT TO CALCULATE POINT-TO-SEGMENT DISTANCE AND PROJECTION
struct Segment {
    double A, B, C; // Ax + by + C = 0
    Point a, b;

    Segment() : A(0), B(0), C(0) {}
    Segment (Point a, Point b) :
        A(a.y - b.y), B(b.x - a.x), C(cross(a, b)), a(a), b(b) {}

    friend double euclideDist (const Segment &d, const Point &p) {
        if (dot(d.b - d.a, p - d.b) <= 0 && dot(d.a - d.b, p - d.a) <= 0) { // projection is on the line
            double numer = abs(d.A * p.x + d.B * p.y + d.C);
            double denom = sqrt(d.A * d.A + d.B * d.B);
            return numer / denom;
        }
        else return min(euclideDist(p, d.a), euclideDist(p, d.b));
    }

    friend Point projection (const Segment &d, const Point &p) {
        if (dot(d.b - d.a, p - d.b) <= 0 && dot(d.a - d.b, p - d.a) <= 0) { // projection is on the line
            double numer = d.A * p.x + d.B * p.y + d.C;
            double denom = d.A * d.A + d.B * d.B;
            double scalar = numer / denom;
            return p - Point(d.A, d.B) * scalar;
        }
        return (euclideDist(p, d.a) < euclideDist(p, d.b) ? d.a : d.b);
    }

    friend double intersectLength(const Segment &a, const Segment &b) {
        double det = a.A * b.B - b.A * a.B;
        if (abs(det) >= eps) return 0.0; // Lines are not parallel

        if (abs(a.A * b.C - a.C * b.A) >= eps) return 0.0; // Parallel, but not collinear

        // Segments are collinear. Find overlap using a 1D projection (t-parameter).
        double aLengthSq = pow(a.b.x - a.a.x, 2) + pow(a.b.y - a.a.y, 2);

        if (aLengthSq < eps) return 0.0; // Segment 'a' is a point

        auto get_t = [&](const Point& p) {
            return ((p.x - a.a.x) * (a.b.x - a.a.x) + (p.y - a.a.y) * (a.b.y - a.a.y)) / aLengthSq;
        };

        double t_b_a = get_t(b.a);
        double t_b_b = get_t(b.b);

        double t_min_b = min(t_b_a, t_b_b);
        double t_max_b = max(t_b_a, t_b_b);

        // Segment 'a' is [0.0, 1.0] in t-space
        double overlap_start = max(0.0, t_min_b);
        double overlap_end = min(1.0, t_max_b);
        
        // Check if they overlap
        if (overlap_start >= overlap_end - eps) {
            return 0.0;
        }

        // Calculate length
        double length_a = sqrt(aLengthSq);
        return (overlap_end - overlap_start) * length_a;
    }
};

/// MORE HELPER FUNCTIONS
double polyLength (const vector<Point> &polyline) {
    double ans = 0.0;
    for (int i = 1; i < polyline.size(); i++)
        ans += euclideDist(polyline[i - 1], polyline[i]);
    return ans;
}
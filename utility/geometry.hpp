/**
 * DESCRIPTION: THIS HEADER FILE CONTAINS BASIC GEOMETRIC DATA STRUCTURES
 * NOTE: THIS FILE IS IMPLEMENTED BY MYSELF
 * TODO: RE-IMPLEMENT STRUCT SEGMENT SINCE I FEEL SUSPICIOUS ABOUT IT
 */

#include <bits/stdc++.h>
using namespace std;

// CONSTANTS
const double eps = 1e-6;
const double oo = 1e9;

// HELPER FUNCTIONS
double squared (double a) { return a * a; }

// POINT STRUCT TO REPRESENT 2D POINTS AND VECTORS
struct Point {
    double x, y;

    // Constructors
    Point() : x(0), y(0) {}
    Point (double x, double y) : x(x), y(y) {}

    // Simple arithmetic operators
    Point operator + (const Point &o) const { return Point(x + o.x, y + o.y); }
    Point operator - (const Point &o) const { return Point(x - o.x, y - o.y); }
    Point operator * (const double &scalar) const { return Point(x * scalar, y * scalar); }

    // Comparison operators
    bool operator == (const Point &o) const { return abs(x - o.x) < eps && abs(y - o.y) < eps; }
    bool operator != (const Point &o) const { return abs(x - o.x) >= eps || abs(y - o.y) >= eps; }
    bool operator < (const Point &o) const { return (y == o.y ? x < o.x : y < o.y); }
    
    // Numeric products
    friend long double dot (const Point &a, const Point &b) { return a.x * b.x + a.y * b.y; }
    friend long double cross (const Point &a, const Point &b) { return a.x * b.y - a.y * b.x; }

    // Numeric distances
    friend double euclideDist (const Point &a, const Point &b) { return sqrt(squared(a.x - b.x) + squared(a.y - b.y)); }
    friend double squaredDist (const Point &a, const Point &b) { return squared(a.x - b.x) + squared(a.y - b.y); }

    // Other utility functions
    friend double pointOnLine (const Point &a, const Point &b, const Point &point) {
        Point x = a - point, y = b - point;
        return abs(cross(x, y)) < eps && dot(x, y) < eps;
    }

    // Input/output operatos
    friend istream& operator>> (istream &inf, Point &p) { return inf >> p.x >> p.y; }
    friend ostream& operator<< (ostream &ouf, const Point &p) { return ouf << "(" << p.x << ", " << p.y << ")"; }
};

// SEGMENT STRUCT TO CALCULATE POINT-TO-SEGMENT DISTANCE AND PROJECTION
struct Segment {
    double A, B, C; // Ax + by + C = 0
    Point a, b;

    // Constructors
    Segment() : A(0), B(0), C(0) {}
    Segment (Point a, Point b) :
        A(a.y - b.y), B(b.x - a.x), C(cross(a, b)), a(a), b(b) {}

    // Find the euclide distance between a Segment and a Point
    friend double euclideDist (const Segment &d, const Point &p) {
        if (dot(d.b - d.a, p - d.b) <= 0 && dot(d.a - d.b, p - d.a) <= 0) { // projection is on the line
            double numer = abs(d.A * p.x + d.B * p.y + d.C);
            double denom = sqrt(d.A * d.A + d.B * d.B);
            return numer / denom;
        }
        else return min(euclideDist(p, d.a), euclideDist(p, d.b));
    }

    // Find the project between a Semgent and a Point
    friend Point projection (const Segment &d, const Point &p) {
        if (dot(d.b - d.a, p - d.b) <= 0 && dot(d.a - d.b, p - d.a) <= 0) { // projection is on the line
            double numer = d.A * p.x + d.B * p.y + d.C;
            double denom = d.A * d.A + d.B * d.B;
            double scalar = numer / denom;
            return p - Point(d.A, d.B) * scalar;
        }
        return (euclideDist(p, d.a) < euclideDist(p, d.b) ? d.a : d.b);
    }
};

// POLYLINE STRUCT
struct Polyline {
private:
    vector<Point> container;
    double totalLength;

public:
    // Constructors
    Polyline() : totalLength(0.0) {}
    Polyline (const vector<Point> &init) : container(init), totalLength(0.0) {
        for (int i = 1; i < container.size(); i++)
            totalLength += euclideDist(container[i - 1], container[i]);
    }

    // Quick access but protect data
    const Point& operator[] (int i) const { return container[i]; }
    int size() const { return container.size(); }
    double length() const { return totalLength; }
    
    // The correct modification function
    void modify (int i, const Point &newPoint) {
        if (i < 0 || i >= size()) return;
        if (i > 0) {
            totalLength -= euclideDist(container[i - 1], container[i]);
            totalLength += euclideDist(container[i - 1], newPoint);
        }
        if (i + 1 < size()) {
            totalLength -= euclideDist(container[i], container[i + 1]);
            totalLength += euclideDist(newPoint, container[i + 1]);
        }
        container[i] = newPoint;
    }

    // Append new point to the end of the path
    void append (const Point &newPoint) {
        if (size())
            totalLength += euclideDist(container.back(), newPoint);
        container.push_back(newPoint);
    }

    // Remove the last point from the end of the path
    Point pop() {
        if (!size()) return Point(0.0, 0.0); // no point to pop
        Point toRemove = container.back();
        container.pop_back();
        if (size())
            totalLength -= euclideDist(container.back(), toRemove);
        return toRemove;
    }
    void clear() { container.clear(), totalLength = 0.0; }

    // Extract a portion of the polyline using the projection of a Point to itself
    Polyline extract (const Point &p, bool toEnd) {
        if (!size()) return Polyline();

        // find closest segemet
        double close = oo; Segment closeSegment; int closePoint = 0;
        for (int i = 1; i < size(); i++) {
            Segment s(container[i - 1], container[i]);
            double current = euclideDist(s, p);
            if (current < close)
                close = current, closeSegment = s, closePoint = i;
        }
        Polyline ans;

        // extract from projection to endpoint
        if (toEnd) {
            ans.append(projection(closeSegment, p));
            for (int i = closePoint; i < size(); i++) ans.append(container[i]);
        }
        // extract from startpoint to projection
        else {
            for (int i = 0; i < closePoint; i++) ans.append(container[i]);
            ans.append(projection(closeSegment, p));
        }
        return ans;
    }

    // Output operator
    friend ostream& operator<< (ostream &ouf, const Polyline &pll) {
        ouf << "<Polyline of length " << pll.length() << ": ";
        for (int i = 0; i < pll.size(); i++) cout << pll[i] << " ";
        ouf << "/>";
    }
};
#pragma once
#include<vector>
#include<cmath>
#include<memory>
#include<algorithm>
#include<utility>
//#define float double
class vec {
public:
	float x, y;
	void normalization();
	float vec::size() { return sqrt(x*x + y*y); }
	vec::vec() {}
	vec::vec(float x, float y) { this->x = x; this->y = y; }
	vec vec::operator+() { return *this; }
	vec vec::operator-() { return vec(-x, -y); }
	vec& vec::operator=(const vec& v) { this->x = v.x;	this->y = v.y; return *this; }
	vec& vec::operator+=(const vec& v) { this->x += v.x;	this->y += v.y;	return *this; }
	vec& vec::operator-=(const vec& v) { this->x -= v.x;	this->y -= v.y;	return *this; }
};
inline float operator*(const vec& v, const vec& u) { return (u.x * v.x + u.y * v.y); }
inline vec operator*(const vec& v, float k) { return vec(k*v.x, k*v.y); }
inline vec operator*(float k, const vec& v) { return vec(k*v.x, k*v.y); }
inline vec operator/(const vec& v, float k) { return vec(v.x / k, v.y / k); }
inline vec operator+(const vec& u, const vec& v) {
	vec w;
	w.x = u.x + v.x;
	w.y = u.y + v.y;
	return w;
}
inline vec operator-(const vec& u, const vec& v) {	//vector-vector
	vec w;
	w.x = u.x - v.x;
	w.y = u.y - v.y;
	return w;
}
struct human {
	vec r;
	vec v;
	float size;
	float m;
	human(float x, float y, float vx, float vy, float size, float m) {
		this->r.x = x;
		this->r.y = y;
		this->v.x = vx;
		this->v.y = vy;
		this->size = size;
		this->m = m;
	}
	float human::dist(human a) { return (this->r - a.r).size(); }
};


struct human_num_list {
	int human_num = 0;
	human_num_list* next = nullptr;
};

class humans
{
public:
	humans();
	void nextstep();
	std::vector<human> vh;
	void set_reflec_const(float x) {
		reflec_const = x;
	}
	int last_unused_hnlist;
	int num_of_hnlist = 0;
private:
	std::vector<std::vector<human_num_list*>> lattice;
	std::vector<std::vector<std::vector<int>>> lattice2;
	std::vector<human_num_list*> unused_hnlist;
	void setLattice();
	void set_lattice2();
	void reflect();
	void opt_reflect();
	void check_reflect(int, human_num_list* &);
	void check_reflect2(int, int, int);
	void to_unused(human_num_list* &);
	void reflect_calc(int, int);
	void opt2_reflect();
	void reflect_wall();
	const float timestep = (float)0.001;
	const int Xsize = 800;
	const int Ysize = 500;
	const int lattice_size = 10;
	float reflec_const = (float)1.0;
};


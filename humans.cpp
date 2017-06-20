#include"pch.h"
#include "humans.h"
using namespace std;

void vec::normalization() {
	float s = this->size();
	x /= s;
	y /= s;
}

humans::humans()
{
	for (int i = 0; i < 50; i++) {
		for (int j = 0; j < 50; j++) {
			vh.push_back(human(i*9.0, j*9.0, 100.0, 1.0, 4.0, 1.0));
		}
	}
	vh.push_back(human(120, 120, 10, 0, 80, 10));
	//vh.push_back(human(500, 300, 10, 0, 70, 10));

	setLattice();
	set_lattice2();
}
void humans::nextstep() {
	reflect_wall();
	//reflect();
	//opt_reflect();
	opt2_reflect();
}
void humans::setLattice() {
	num_of_hnlist = 0;
	for (auto h : vh) {
		int  s = (int)(2 * h.size / lattice_size);
		num_of_hnlist += (s + 2)*(s + 2);
	}
	unused_hnlist.resize(num_of_hnlist);
	void* p = malloc(sizeof(human_num_list)* num_of_hnlist);
	human_num_list* q = (human_num_list*)p;
	for (int i = 0; i < num_of_hnlist; i++) {
		unused_hnlist[i] = q + i;
	}
	last_unused_hnlist = num_of_hnlist - 1;

	lattice.resize(Xsize / lattice_size + 2, vector<human_num_list*>(Ysize / lattice_size + 2));
}
void humans::set_lattice2() {
	lattice2.resize(Xsize / lattice_size + 2, vector<vector<int>>(Ysize / lattice_size + 2));
	for (int i = 0; i < lattice2.size(); i++) {
		for (int j = 0; j < lattice2[i].size(); j++) {
			lattice2[i][j].reserve(10);
		}
	}

}
void humans::reflect_wall() {
	for (auto itr = vh.begin(); itr != vh.end(); ++itr) {
		itr->r += itr->v*timestep;
		if (itr->v.x < 0 && itr->r.x < 0 + itr->size) {
			itr->r.x = 2 * itr->size - itr->r.x;
			itr->v.x *= -1;
		}
		else if (itr->v.x > 0 && itr->r.x > Xsize - itr->size) {
			itr->r.x = 2 * (Xsize - itr->size) - itr->r.x;
			itr->v.x *= -1;
		}
		if (itr->v.y < 0 && itr->r.y < 0 + itr->size) {
			itr->r.y = 2 * itr->size - itr->r.y;
			itr->v.y *= -1;
		}
		else if (itr->v.y> 0 && itr->r.y > Ysize - itr->size) {
			itr->r.y = 2 * (Ysize - itr->size) - itr->r.y;
			itr->v.y *= -1;
		}
	}
}
void humans::to_unused(human_num_list* &ptr) {
	if (ptr != nullptr) {
		if (ptr->next) {
			to_unused(ptr->next);
		}
		unused_hnlist[last_unused_hnlist + 1] = ptr;
		ptr = nullptr;
		last_unused_hnlist++;
	}
	return;
}
void humans::opt_reflect() {
	for (int i = 0; i < lattice.size(); i++) {
		for (int j = 0; j < lattice[i].size(); j++) {
			to_unused(lattice[i][j]);
		}
	}
	for (int human_num = 0; human_num < vh.size(); human_num++) {
		int xs = max((float)0.001, (float)(vh[human_num].r.x - vh[human_num].size)) / lattice_size;
		int xe = min((float)(Xsize - 0.001), (float)(vh[human_num].r.x + vh[human_num].size)) / lattice_size;
		int ys = max((float)0.001, (float)(vh[human_num].r.y - vh[human_num].size)) / lattice_size;
		int ye = min((float)(Ysize - 0.001), (float)(vh[human_num].r.y + vh[human_num].size)) / lattice_size;
		for (int i = xs; i <= xe; i++) {
			for (int j = ys; j <= ye; j++) {
				check_reflect(human_num, lattice[i][j]);
			}
		}
	}
}
void humans::opt2_reflect() {
	for (int i = 0; i < lattice2.size(); i++) {
		for (int j = 0; j < lattice2[i].size(); j++) {
			lattice2[i][j].resize(0);
		}
	}
	for (int human_num = 0; human_num < vh.size(); human_num++) {
		int xs = max((float)0.001, (float)(vh[human_num].r.x - vh[human_num].size)) / lattice_size;
		int xe = min((float)(Xsize - 0.001), (float)(vh[human_num].r.x + vh[human_num].size)) / lattice_size;
		int ys = max((float)0.001, (float)(vh[human_num].r.y - vh[human_num].size)) / lattice_size;
		int ye = min((float)(Ysize - 0.001), (float)(vh[human_num].r.y + vh[human_num].size)) / lattice_size;
		for (int i = xs; i <= xe; i++) {
			for (int j = ys; j <= ye; j++) {
				check_reflect2(human_num, i, j);
			}
		}
	}
}

void humans::check_reflect(int human_num, human_num_list* &ptr) {
	if (ptr == nullptr) {
		ptr = unused_hnlist[last_unused_hnlist]; unused_hnlist[last_unused_hnlist] = nullptr;
		last_unused_hnlist--;
		ptr->human_num = human_num;
		ptr->next = nullptr;
	}
	else {
		auto A = human_num + vh.begin();
		auto B = ptr->human_num + vh.begin();
		if ((A->r - B->r).size() < A->size + B->size) {
			if ((A->r - B->r)*(A->v - B->v) < 0) {
				vec e1 = A->r - B->r;
				e1.normalization();
				vec e2(e1.y, -e1.x);
				vec nAv = (A->v * e2)*e2;
				vec nBv = (B->v * e2)*e2;
				vec centerV = (A->m * (e1 * (A->v*e1)) + B->m * (e1 *(e1* B->v))) / (A->m + B->m);
				nAv += centerV;
				nBv += centerV;
				nAv += (-reflec_const)*(e1*(A->v - B->v)*e1) * (B->m / (A->m + B->m));
				nBv += (-reflec_const)*(e1*(B->v - A->v)*e1) * (A->m / (A->m + B->m));
				A->v = nAv;
				B->v = nBv;
			}
		}
		check_reflect(human_num, ptr->next);
	}
}
void humans::reflect_calc(int a, int b) {
	auto A = a + vh.begin();
	auto B = b + vh.begin();
	if ((A->r - B->r).size() < A->size + B->size) {
		if ((A->r - B->r)*(A->v - B->v) < 0) {
			vec e1 = A->r - B->r;
			e1.normalization();
			//vec e2(e1.y, -e1.x);
			float Ae2 = A->v.x * e1.y - A->v.y * e1.x;
			float Be2 = B->v.x * e1.y - B->v.y * e1.x;
			vec nAv(e1.y * Ae2, -e1.x * Ae2);
			vec nBv(e1.y * Be2, -e1.x * Be2);
			//vec nAv = (A->v * e2)*e2;
			//vec nBv = (B->v * e2)*e2;
			float Ae1 = A->v.x * e1.x + A->v.y * e1.y;
			float Be1 = B->v.x * e1.x + B->v.y * e1.y;
			//vec centerV = (A->m * (e1 * A->v * e1) + B->m * (e1 * B->v * e1)) / (A->m + B->m);
			//nAv += centerV;
			//nBv += centerV;
			//nAv += (e1*(A->v - B->v)*e1) * ((-reflec_const)*B->m / (A->m + B->m));
			//nBv += (e1*(B->v - A->v)*e1) * ((-reflec_const)*A->m / (A->m + B->m));
			nAv += e1*(((Ae1 - Be1) * (-reflec_const) * B->m + A->m * Ae1 + B->m * Be1) / (A->m + B->m));
			nBv += e1*(((Be1 - Ae1) * (-reflec_const) * A->m + A->m * Be1 + B->m * Be1) / (A->m + B->m));
			A->v = nAv;
			B->v = nBv;
		}
	}
}
void humans::check_reflect2(int human_num, int i, int j) {
	for (int k = 0; k < lattice2[i][j].size(); k++) {
		//reflect_calc(human_num, lattice2[i][j][k]);
		auto A = human_num + vh.begin();
		auto B = lattice2[i][j][k] + vh.begin();
		if ((A->r - B->r).size() < A->size + B->size) {
			if ((A->r - B->r)*(A->v - B->v) < 0) {
				vec e1 = A->r - B->r;
				e1.normalization();
				vec e2(e1.y, -e1.x);
				vec nAv = (A->v * e2)*e2;
				vec nBv = (B->v * e2)*e2;
				vec centerV = (A->m * (e1 * A->v * e1) + B->m * (e1 * B->v * e1)) / (A->m + B->m);
				nAv += centerV;
				nBv += centerV;
				nAv += (-reflec_const)*(e1*(A->v - B->v)*e1) * (B->m / (A->m + B->m));
				nBv += (-reflec_const)*(e1*(B->v - A->v)*e1) * (A->m / (A->m + B->m));
				A->v = nAv;
				B->v = nBv;
			}
		}
	}
	lattice2[i][j].push_back(human_num);
}
void humans::reflect() {
	for (auto A = vh.begin(); A != vh.end(); ++A) {
		auto B = A;
		++B;
		for (; B != vh.end(); ++B) {
			if ((A->r - B->r).size() < A->size + B->size) {
				if ((A->r - B->r)*(A->v - B->v) < 0) {
					vec e1 = A->r - B->r;
					e1.normalization();
					vec e2(e1.y, -e1.x);
					vec nAv = (A->v * e2)*e2;
					vec nBv = (B->v * e2)*e2;
					vec centerV = (A->m * (e1 * (A->v*e1)) + B->m * (e1 *(e1* B->v))) / (A->m + B->m);
					nAv += centerV;
					nBv += centerV;
					nAv += (-reflec_const)*(e1*(A->v - B->v)*e1) * (B->m / (A->m + B->m));
					nBv += (-reflec_const)*(e1*(B->v - A->v)*e1) * (A->m / (A->m + B->m));
					A->v = nAv;
					B->v = nBv;
				}
			}
		}
	}
}
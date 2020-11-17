#pragma once

class pt2 {
public:
	double x;
	double y;
	pt2 mtp(double);

	pt2();
	pt2(double, double);
};
class pt2i {
public:
	int x;
	int y;
};
class pt3 {
public:
	double x;
	double y;
	double z;
	void asg(double x, double y, double z);
	void asg2(pt3 xyz);
	pt3 pls(pt3 pts);
	pt3 mns(pt3 pts);
	pt3 mtp(double pts);
	pt3 mtp(pt3 pts);
	pt3 norm();

	static double dot(pt3, pt3);	// 内積
	static pt3 cross(pt3, pt3);		// 外積(クロス積)

	pt3();
	pt3(double, double, double);
};
class pt4 {
public:
	double w;
	double x;
	double y;
	double z;
	void asg(double w, double x, double y, double z);
	void asg2(pt4 wxyz);
	pt4 pls(pt4 pts);
	pt4 pls(double, double, double, double);
	pt4 mns(pt4 pts);
	pt4 mtp(double pts);
	pt4 mtp(pt4 pts);
	pt3 xyz();
	pt4 norm();
	void asgPt3(pt3);

	static double dot(pt4, pt4);		// 内積
	static pt4 cross(pt4, pt4, pt4);	// 外積(クロス積)

	pt4();
	pt4(double, double, double, double);
	pt4(double, pt3);
};
class pt6 {
public:
	double xy, yz, xz, wx, wy, wz;
	void asg(double, double, double, double, double, double);
	void asg2(pt6 wxyz);
	pt6 pls(pt6 pts);
	pt6 pls(double, double, double, double, double, double);
};


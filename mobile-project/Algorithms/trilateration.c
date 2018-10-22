#include "trilateration.h"

#define height 175.0
#define upper height+5
#define lower height-5

float pythagoras(float point_height, float measured_distance) {
	if ((point_height <= upper) && (point_height >= lower)) {
		return measured_distance;
	} else if (point_height > upper) {
		return sqrt(measured_distance*measured_distance - (point_height - height)*(point_height - height));
	} else if (point_height < lower) {
		return sqrt(measured_distance*measured_distance - (height - point_height)*(height - point_height));
	}
	printf("ERROR in Pythagoras\r\n");
	return 0;
}

uint8 trilaterate3(uint16 *P1, uint16 *P2, uint16 *P3, float *L, uint16 *posi)
{
    float x1 = (float)P1[0];
    float y1 = (float)P1[1];
    float z1 = (float)P1[2];

    float x2 = (float)P2[0];
    float y2 = (float)P2[1];
    float z2 = (float)P2[2];

    float x3 = (float)P3[0];
    float y3 = (float)P3[1];
    float z3 = (float)P3[2];

    float tL1 = L[0]*100;
    float tL2 = L[1]*100;
    float tL3 = L[2]*100;

    float L1 = pythagoras(z1, tL1);
    float L2 = pythagoras(z2, tL2);
    float L3 = pythagoras(z3, tL3);

    z1 = 175;
    z2 = 175;
    z3 = 175;

    //calculate coords in plane of stations
    float LB1 = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
    float LB2 = sqrt((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2) + (z3 - z2) * (z3 - z2));
    float LB3 = sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3) + (z1 - z3) * (z1 - z3));

    float X = (L1*L1  - L2*L2  + LB1*LB1)/(2*LB1);
    float C1 = sqrt (L1*L1 - X*X);
    if (L1*L1 - X*X < 0) return 0;
    float XB = (LB3*LB3 - LB2*LB2 + LB1*LB1 )/(2*LB1 );
    if (LB3*LB3 - XB* XB < 0 ) return 0;
    float CB=  sqrt(LB3*LB3 - XB* XB );
    if (C1*C1+(XB - X)*(XB - X)< 0) return 0;
    float D1 = sqrt(C1*C1+(XB - X)*(XB - X));
    float Y = (D1*D1 - L3*L3  + CB*CB  )/(2*CB );
    if (C1*C1 - Y*Y < 0) return 0;
    float Z = sqrt(C1 * C1 - Y * Y);

    //Now transform X,Y,Z to x,y,z
    //Find the unit vectors in X,Y,Z
    float Xx = (x2-x1);
    float Xy = (y2-y1);
    float Xz = (z2-z1);
    float Xl = sqrt(Xx*Xx+Xy*Xy+Xz*Xz);
    Xx = Xx / Xl;
    Xy = Xy / Xl;
    Xz = Xz / Xl;

    float t =- ((x1-x3)*(x2-x1)+(y1-y3)*(y2-y1)+(z1-z3)*(z2-z1))/(LB1*LB1);
    float Yx = (x1+(x2-x1)*t-x3);
    float Yy = (y1+(y2-y1)*t-y3);
    float Yz = (z1+(z2-z1)*t-z3);
    float Yl = sqrt(Yx*Yx+Yy*Yy+Yz*Yz);
    Yx =- (Yx/Yl);
    Yy =- (Yy/Yl);
    Yz =- (Yz/Yl);

    float Zx = (Xy * Yz - Xz * Yy);
    float Zy = (Xz * Yx - Xx * Yz);
    float Zz = (Xx * Yy - Xy * Yx);

    posi[0] = (uint16)(x1 + X * Xx + Y * Yx - Z * Zx);
    posi[1] = (uint16)(y1 + X * Xy + Y * Yy - Z * Zy);
    posi[2] = (uint16)(z1 + X * Xz + Y * Yz - Z * Zz);

    if (posi[0] > 1060 || posi[1] > 1910) return 0;
    if (posi[0] <= 0 || posi[1] <= 0) return 0;

    return 1;
}

uint8 trilaterate4(uint16 *P1, uint16 *P2, uint16 *P3, uint16 *P4, float *L, uint16 *posi) {
	double x1 = (double)P1[0];
	double y1 = (double)P1[1];
	double z1 = (double)P1[2];

	double x2 = (double)P2[0];
	double y2 = (double)P2[1];
	double z2 = (double)P2[2];

	double x3 = (double)P3[0];
	double y3 = (double)P3[1];
	double z3 = (double)P3[2];

	double x4 = (double)P4[0];
	double y4 = (double)P4[1];
	double z4 = (double)P4[2];

	double L1 = L[0]*100;
	double L2 = L[1]*100;
	double L3 = L[2]*100;
	double L4 = L[3]*100;

	double A1 = 2*(x4-x1);
	double A2 = 2*(y4-y1);
	double A3 = 2*(z4-z1);		// |-	     -|
	double A4 = 2*(x4-x2);		// | A1	A2 A3 |
	double A5 = 2*(y4-y2);		// | A4	A5 A6 |
	double A6 = 2*(z4-z2);		// | A7	A8 A9 |
	double A7 = 2*(x4-x3);		// |-        -|
	double A8 = 2*(y4-y3);
	double A9 = 2*(z4-z3);
																									// |-  -|
	double B1 = (L1*L1) - (L4*L4) - (x1*x1) - (y1*y1) - (z1*z1) + (x4*x4) + (y4*y4) + (z4*z4);		// | B1 |
	double B2 = (L2*L2) - (L4*L4) - (x2*x2) - (y2*y2) - (z2*z2) + (x4*x4) + (y4*y4) + (z4*z4);		// | B2 |
	double B3 = (L3*L3) - (L4*L4) - (x3*x3) - (y3*y3) - (z3*z3) + (x4*x4) + (y4*y4) + (z4*z4);		// | B3 |
																									// |-  -|
	// C = trans(A)*A
	double C1 = A1*A1 + A2*A2 + A3*A3;
	double C2 = A1*A4 + A2*A5 + A3*A6;
	double C3 = A1*A7 + A2*A8 + A3*A9;
	double C4 = A4*A1 + A5*A2 + A6*A3;
	double C5 = A4*A4 + A5*A5 + A6*A6;
	double C6 = A4*A7 + A5*A8 + A6*A9;
	double C7 = A7*A1 + A8*A2 + A9*A3;
	double C8 = A7*A4 + A8*A5 + A9*A6;
	double C9 = A7*A7 + A8*A8 + A9*A9;

	// D = trans(A)*B
	double D1 = A1*B1 + A4*B2 + A7*B3;
	double D2 = A2*B1 + A5*B2 + A8*B3;
	double D3 = A3*B1 + A6*B2 + A9*B3;

	// E = inv(C)
	double det = C1*(C5*C9 - C8*C6) - C2*(C4*C9 - C6*C7) + C3*(C4*C8 - C5*C7);
	if (det == 0) {
		return 0;
	}
	double invdet = 1 / det;

	double E1 = (C5*C9 - C8*C6) * invdet;
	double E2 = (C3*C8 - C2*C9) * invdet;
	double E3 = (C2*C6 - C3*C5) * invdet;
	double E4 = (C6*C7 - C4*C9) * invdet;
	double E5 = (C1*C9 - C3*C7) * invdet;
	double E6 = (C4*C3 - C1*C6) * invdet;
	double E7 = (C4*C8 - C7*C5) * invdet;
	double E8 = (C7*C2 - C1*C8) * invdet;
	double E9 = (C1*C5 - C4*C2) * invdet;

	// posi = E*D
	posi[0] = (uint16)(E1*D1 + E2*D2 + E3*D3);
	posi[1] = (uint16)(E4*D1 + E5*D2 + E6*D3);
	posi[2] = (uint16)(E7*D1 + E8*D2 + E9*D3);

	printf("Location is X:%f, Y:%f Z:%f\r\n", (E1*D1 + E2*D2 + E3*D3), (E4*D1 + E5*D2 + E6*D3), (E7*D1 + E8*D2 + E9*D3));

	return 1;
}

uint8 trilaterate4_2(uint16 *P1, uint16 *P2, uint16 *P3, uint16 *P4, float *L, uint16 *posi) {
	double x1 = (double)P1[0];
	double y1 = (double)P1[1];
	double z1 = (double)P1[2];

	double x2 = (double)P2[0];
	double y2 = (double)P2[1];
	double z2 = (double)P2[2];

	double x3 = (double)P3[0];
	double y3 = (double)P3[1];
	double z3 = (double)P3[2];

	double x4 = (double)P4[0];
	double y4 = (double)P4[1];
	double z4 = (double)P4[2];

	double L1 = L[0]*100;
	double L2 = L[1]*100;
	double L3 = L[2]*100;
	double L4 = L[3]*100;

	double A1 = (L1*L1 - L2*L2) - (x1*x1 - x2*x2) - (y1*y1 - y2*y2) - (z1*z1 - z2*z2);
	double A2 = 2*(y2-y1);
	double A3 = 2*(z2-z1);
	double A4 = (L1*L1 - L3*L3) - (x1*x1 - x3*x3) - (y1*y1 - y3*y3) - (z1*z1 - z3*z3);
	double A5 = 2*(y3-y1);
	double A6 = 2*(z3-z1);
	double A7 = (L1*L1 - L4*L4) - (x1*x1 - x4*x4) - (y1*y1 - y4*y4) - (z1*z1 - z4*z4);
	double A8 = 2*(y4-y1);
	double A9 = 2*(z4-z1);
	double detA = A1*(A5*A9 - A8*A6) - A2*(A4*A9 - A6*A7) + A3*(A4*A8 - A5*A7);

	double B1 = 2*(x2-x1);
	double B2 = 2*(y2-y1);
	double B3 = 2*(z2-z1);
	double B4 = 2*(x3-x1);
	double B5 = 2*(y3-y1);
	double B6 = 2*(z3-z1);
	double B7 = 2*(x4-x1);
	double B8 = 2*(y4-y1);
	double B9 = 2*(z4-z1);
	double detB = B1*(B5*B9 - B8*B6) - B2*(B4*B9 - B6*B7) + B3*(B4*B8 - B5*B7);

	double C2 = (L1*L1 - L2*L2) - (x1*x1 - x2*x2) - (y1*y1 - y2*y2) - (z1*z1 - z2*z2);
	double C1 = 2*(x2-x1);
	double C3 = 2*(z2-z1);
	double C5 = (L1*L1 - L3*L3) - (x1*x1 - x3*x3) - (y1*y1 - y3*y3) - (z1*z1 - z3*z3);
	double C4 = 2*(x3-x1);
	double C6 = 2*(z3-z1);
	double C8 = (L1*L1 - L4*L4) - (x1*x1 - x4*x4) - (y1*y1 - y4*y4) - (z1*z1 - z4*z4);
	double C7 = 2*(x4-x1);
	double C9 = 2*(z4-z1);
	double detC = C1*(C5*C9 - C8*C6) - C2*(C4*C9 - C6*C7) + C3*(C4*C8 - C5*C7);

	if (detA/detB > 0 && detA/detB < 1100 && detC/detB > 0 && detC/detB < 2000) {
		posi[0] = (uint16)(detA/detB);
		posi[1] = (uint16)(detC/detB);
	}

	return 1;
}

uint8 trilaterate4_3(uint16 *P1, uint16 *P2, uint16 *P3, uint16 *P4, float *L, uint16 *posi) {
	double x1 = (double)P1[0];
	double y1 = (double)P1[1];
	double z1 = (double)P1[2];

	double x2 = (double)P2[0];
	double y2 = (double)P2[1];
	double z2 = (double)P2[2];

	double x3 = (double)P3[0];
	double y3 = (double)P3[1];
	double z3 = (double)P3[2];

	double x4 = (double)P4[0];
	double y4 = (double)P4[1];
	double z4 = (double)P4[2];

	double L1 = L[0]*100;
	double L2 = L[1]*100;
	double L3 = L[2]*100;
	double L4 = L[3]*100;

	double A1 = x2-x1;
	double A2 = y2-y1;
	double A3 = z2-z1;		// |-	     -|
	double A4 = x3-x1;		// | A1	A2 A3 |
	double A5 = y3-y1;		// | A4	A5 A6 |
	double A6 = z3-z1;		// | A7	A8 A9 |
	double A7 = x4-x1;		// |-        -|
	double A8 = y4-y1;
	double A9 = z4-z1;

	double D21 = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1) + (z2-z1)*(z2-z1));
	double D31 = sqrt((x3-x1)*(x3-x1) + (y3-y1)*(y3-y1) + (z3-z1)*(z3-z1));
	double D41 = sqrt((x4-x1)*(x4-x1) + (y4-y1)*(y4-y1) + (z4-z1)*(z4-z1));

	double B1 = 0.5*(L1*L1 - L2*L2 + D21*D21);
	double B2 = 0.5*(L1*L1 - L3*L3 + D31*D31);
	double B3 = 0.5*(L1*L1 - L4*L4 + D41*D41);

	// C = trans(A)*A
	double C1 = A1*A1 + A2*A2 + A3*A3;
	double C2 = A1*A4 + A2*A5 + A3*A6;
	double C3 = A1*A7 + A2*A8 + A3*A9;
	double C4 = A4*A1 + A5*A2 + A6*A3;
	double C5 = A4*A4 + A5*A5 + A6*A6;
	double C6 = A4*A7 + A5*A8 + A6*A9;
	double C7 = A7*A1 + A8*A2 + A9*A3;
	double C8 = A7*A4 + A8*A5 + A9*A6;
	double C9 = A7*A7 + A8*A8 + A9*A9;

	// D = trans(A)*B
	double D1 = A1*B1 + A4*B2 + A7*B3;
	double D2 = A2*B1 + A5*B2 + A8*B3;
	double D3 = A3*B1 + A6*B2 + A9*B3;

	// E = inv(C)
	double det = C1*(C5*C9 - C8*C6) - C2*(C4*C9 - C6*C7) + C3*(C4*C8 - C5*C7);
	if (det == 0) {
		return 0;
	}
	double invdet = 1 / det;

	double E1 = (C5*C9 - C8*C6) * invdet;
	double E2 = (C3*C8 - C2*C9) * invdet;
	double E3 = (C2*C6 - C3*C5) * invdet;
	double E4 = (C6*C7 - C4*C9) * invdet;
	double E5 = (C1*C9 - C3*C7) * invdet;
	double E6 = (C4*C3 - C1*C6) * invdet;
	double E7 = (C4*C8 - C7*C5) * invdet;
	double E8 = (C7*C2 - C1*C8) * invdet;
	double E9 = (C1*C5 - C4*C2) * invdet;

	// posi = E*D
	posi[0] = (uint16)(E1*D1 + E2*D2 + E3*D3);
	posi[1] = (uint16)(E4*D1 + E5*D2 + E6*D3);
	posi[2] = (uint16)(E7*D1 + E8*D2 + E9*D3);

	printf("Location is X:%f, Y:%f Z:%f\r\n", (E1*D1 + E2*D2 + E3*D3), (E4*D1 + E5*D2 + E6*D3), (E7*D1 + E8*D2 + E9*D3));

	return 1;
}

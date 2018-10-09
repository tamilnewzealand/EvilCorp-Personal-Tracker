#include "trilateration.h"

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

    float L1 = L[0]*100;
    float L2 = L[1]*100;
    float L3 = L[2]*100;

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

    return 1;
}

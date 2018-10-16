#ifndef ALGORITHMS_COMPASS_H_
#define ALGORITHMS_COMPASS_H_

#include <math.h>
#include <stdint.h>
#include "Drivers/includes.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef union {
    float array[3];

    struct {
        float x;
        float y;
        float z;
    } axis;
} Vector3;

/**
 * @brief Converts radians to degrees.
 * @param radians Radians.
 * @return Degrees.
 */
static inline __attribute__((always_inline)) float RadiansToDegrees(const float radians) {
    return radians * (180.0f / (float) M_PI);
}

/**
 * @brief Calculates the reciprocal of the square root.
 * @see http://en.wikipedia.org/wiki/Fast_inverse_square_root
 * @param x Operand.
 * @return Reciprocal of the square root of x.
 */
static inline __attribute__((always_inline)) float FastInverseSqrt(const float x) {
    float halfx = 0.5f * x;
    float y = x;
    int32_t i = *(int32_t*) & y;
    i = 0x5f3759df - (i >> 1);
    y = *(float*) &i;
    y = y * (1.5f - (halfx * y * y));
    return y;
}

/**
 * @brief Multiplies vector by a scalar.
 * @param vector Vector to be multiplied.
 * @param scalar Scalar to be multiplied.
 * @return Vector multiplied by scalar.
 */
static inline __attribute__((always_inline)) Vector3 VectorMultiplyScalar(const Vector3 vector, const float scalar) {
    Vector3 result;
    result.axis.x = vector.axis.x * scalar;
    result.axis.y = vector.axis.y * scalar;
    result.axis.z = vector.axis.z * scalar;
    return result;
}

/**
 * @brief Calculates the cross-product of two vectors.
 * @param vectorA First vector of the operation.
 * @param vectorB Second vector of the operation.
 * @return Cross-product of vectorA and vectorB.
 */
static inline __attribute__((always_inline)) Vector3 VectorCrossProduct(const Vector3 vectorA, const Vector3 vectorB) {
    Vector3 result;
    result.axis.x = vectorA.axis.y * vectorB.axis.z - vectorA.axis.z * vectorB.axis.y;
    result.axis.y = vectorA.axis.z * vectorB.axis.x - vectorA.axis.x * vectorB.axis.z;
    result.axis.z = vectorA.axis.x * vectorB.axis.y - vectorA.axis.y * vectorB.axis.x;
    return result;
}

/**
 * @brief Normalises a vector to be of unit magnitude using the fast inverse
 * square root approximation.
 * @param vector Vector to be normalised.
 * @return Normalised vector.
 */
static inline __attribute__((always_inline)) Vector3 VectorFastNormalise(const Vector3 vector) {
    const float normReciprocal = FastInverseSqrt(vector.axis.x * vector.axis.x + vector.axis.y * vector.axis.y + vector.axis.z * vector.axis.z);
    return VectorMultiplyScalar(vector, normReciprocal);
}

float calculateHeading(Vector3 uncalibratedAccelerometer, Vector3 uncalibratedMagnetometer);

#endif /* ALGORITHMS_COMPASS_H_ */

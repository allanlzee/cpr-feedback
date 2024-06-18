#include <math.h>

struct quaternion {
    private:
        double aw; // gw (real)
        double bx; // gx (imaginary)
        double cy; // gy (imaginary)
        double dz; // gz (imaginary)

    public: 
        // Constructor (default) 
        quaternion() : aw(1.0), bx(0.0), cy(0.0), dz(0.0) {}
        // Constructor (parameterized)
        quaternion(double w, double x, double y, double z) 
            : aw(w), bx(x), cy(y), dz(z) 
        {}

        // Getters
        double a() const {
            return aw; 
        }

        double b() const {
            return bx; 
        }

        double c() const {
            return cy; 
        }

        double d() const {
            return dz;
        }

        // Setters
        void setA(double w) {
            aw = w; 
        }

        void setB(double x) {
            bx = x; 
        }

        void setC(double y) {
            cy = y;
        }

        void setD(double z) {
            dz = z; 
        }

        // Operator + to add two quaternions.
        quaternion operator+(quaternion const& q) const 
        {
            return quaternion(aw + q.a(), bx + q.b(), cy + q.c(), dz + q.d());
        }

        // Operator - to subtract two quaternions.
        quaternion operator-(quaternion const& q) const 
        {
            return quaternion(aw - q.a(), bx - q.b(), cy - q.c(), dz - q.d());
        }

        // Operator * to multiply quaternion by a scalar.
        quaternion operator*(double const& scalar) const
        {
            return quaternion(aw * scalar, bx * scalar, cy * scalar, dz * scalar);
        }

        friend quaternion operator*(double scalar, const quaternion& q) 
        {
            return q * scalar;
        }

        // Operator * to multiply two quaternions.
        quaternion operator*(quaternion const& q) const 
        {
            return quaternion(
                aw * q.a() - bx * q.b() - cy * q.c() - dz * q.d(),
                aw * q.b() + bx * q.a() + cy * q.d() - dz * q.c(),
                aw * q.c() - bx * q.d() + cy * q.a() + dz * q.b(),
                aw * q.d() + bx * q.c() - cy * q.b() + dz * q.a()
            );
        }

        quaternion operator/(double const scale) const 
        {
            return quaternion(
                aw / scale, 
                bx / scale, 
                cy / scale, 
                dz / scale
            );
        }

        // Conjugate of quaternion struct. 
        quaternion conj() const
        {
            return quaternion(
                aw,
                -bx,
                -cy, 
                -dz
            );
        }
};

/* Computing the norm (absolute value) of quaternion.
*/
double qabs(quaternion const& q) 
{
    return sqrt(
        pow(q.a(), 2) + pow(q.b(), 2) + pow(q.c(), 2) + pow(q.d(), 2)       
    );
}


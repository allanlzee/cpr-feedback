namespace aacm {

class quaternion {
    
private:
    
    double n, ni, nj, nk;
    
public:
    
    using value_type = double; ///< value_type trait for STL compatibility
    
    /**
     * Default constuctor
     */
    quaternion(const double& n = static_cast<double>(0), const double& ni = static_cast<double>(0), const double& nj = static_cast<double>(0), const double& nk = static_cast<double>(0))
        : n(n), ni(ni), nj(nj), nk(nk) {}

     /**
     * Copy constructor 
     */
    quaternion(const quaternion& rhs)
        : n(rhs.a()), ni(rhs.b()), nj(rhs.c()), nk(rhs.d()) {}
    
    /**
     * Default copy assignment operator
     */
    quaternion& operator=(const quaternion& rhs) {
        this->n = rhs.a();
        this->ni = rhs.b();
        this->nj = rhs.c();
        this->nk = rhs.d();
    }
    
    double a() const {
        return this->n;
    }
    
    double b() const {
        return this->ni;
    }
    
    double c() const {
        return this->nj;
    }
    
    double d() const {
        return this->nk;
    }
        
    double real() const {
        return this->n;
    }
    
    quaternion unreal() const {
        return {static_cast<double>(0), this->ni, this->nj, this->nk};
    }
       
};

/**
 * Norm of quaternion
 */
double norm(const quaternion& quat) {
   return (quat.a() * quat.a()) + (quat.b() * quat.b()) + (quat.c() * quat.c()) + (quat.d() * quat.d());
}


/**
 * Implementation of abs with double sqrt 
 */
double qabs(const quaternion& quat) {
   return sqrt(norm(quat));
}

/**
 * The conjugate of quaternion
 */
quaternion conj(const quaternion& quat) {
   return {quat.a(), quat.b() * -1, quat.c() * -1, quat.d() * -1};
}

/**
 * Is not a number?
 */
bool qisnan(quaternion quat) {
   return isnan(quat.a()) || isnan(quat.b()) || isnan(quat.c()) || isnan(quat.d());
}

/**
 * Is infinite?
 */
bool qisinf(quaternion quat) {
   return isinf(quat.a()) || isinf(quat.b()) || isinf(quat.c()) || isinf(quat.d());
}

/**
 * Is finite?
 */
bool qisfinite(quaternion quat) {
   return isfinite(quat.a()) && isfinite(quat.b()) && isfinite(quat.c()) && isfinite(quat.d());
}

/**
 * Add operator between two quaternios.
 */
quaternion operator+(const quaternion& lhs, const quaternion& rhs) {
    return {lhs.a() + rhs.a(), lhs.b() + rhs.b(), lhs.c() + rhs.c(), lhs.d() + rhs.d()};
}


/**
 * Add operator between quaternion and scalar.
 */
quaternion operator+(const quaternion& lhs, const double& rhs) {
    return {lhs.a() + rhs, lhs.b(), lhs.c(), lhs.d()};
}

/**
 * Add operator between scalar and quaternion.
 */
quaternion operator+(const double& lhs, const quaternion& rhs) {
    return operator+(rhs, lhs);
}

/**
 * Sub operator between two quaternions.
 */
quaternion operator-(const quaternion& lhs, const quaternion& rhs) {
    return {lhs.a() - rhs.a(), lhs.b() - rhs.b(), lhs.c() - rhs.c(), lhs.d() - rhs.d()};
}

/**
 * Mul operator between two quaternions.
 */
quaternion operator*(const quaternion& lhs, const quaternion& rhs) {
    double tn = (lhs.a() * rhs.a()) - (lhs.b() * rhs.b()) - (lhs.c() * rhs.c()) - (lhs.d() * rhs.d());
    double tni = (lhs.a() * rhs.b()) + (lhs.b() * rhs.a()) + (lhs.c() * rhs.d()) - (lhs.d() * rhs.c());
    double tnj = (lhs.a() * rhs.c()) + (lhs.c() * rhs.a()) + (lhs.d() * rhs.b()) - (lhs.b() * rhs.d());
    double tnk = (lhs.a() * rhs.d()) + (lhs.d() * rhs.a()) + (lhs.b() * rhs.c()) - (lhs.c() * rhs.b());
    return {tn, tni, tnj, tnk};
}

/**
 * Mul operator between quaternion and scalar.
 */
quaternion operator*(const quaternion& lhs, const double& rhs) {
    return {lhs.a() * rhs, lhs.b() * rhs, lhs.c() * rhs, lhs.d() * rhs};
}

/**
 * Mul operator between scalar and quaternion.
 */
quaternion operator*(const double& lhs, const quaternion& rhs) {
    return operator*(rhs, lhs);
}

/**
 * Div operator between two quaternions.
 */
quaternion operator/(const quaternion& lhs, const quaternion& rhs) {
    double tn  = (lhs.a() * rhs.a()) + (lhs.b() * rhs.b()) + (lhs.c() * rhs.c()) + (lhs.d() * rhs.d());
    double tni = - (lhs.a() * rhs.b()) + (lhs.b() * rhs.a()) - (lhs.c() * rhs.d()) + (lhs.d() * rhs.c());
    double tnj = - (lhs.a() * rhs.c()) + (lhs.c() * rhs.a()) - (lhs.d() * rhs.b()) + (lhs.b() * rhs.d());
    double tnk = - (lhs.a() * rhs.d()) + (lhs.d() * rhs.a()) - (lhs.b() * rhs.c()) + (lhs.c() * rhs.b());
    double dnorm = norm(rhs);
    return {tn / dnorm, tni / dnorm, tnj / dnorm, tnk / dnorm};
}

/**
 * Div operator between quaternion and scalar.
 */
quaternion operator/(const quaternion& lhs, const double& rhs){
    return {lhs.a() / rhs, lhs.b() / rhs, lhs.c() / rhs, lhs.d() / rhs};
}

/**
 * Div operator between scalar and quaternion.
 */
quaternion operator/(const double& lhs, const quaternion& rhs) {
    double dnorm = norm(rhs);
    return {(rhs.a() * lhs) / dnorm, -(rhs.b() * lhs) / dnorm, -(rhs.c() * lhs) / dnorm, -(rhs.d() * lhs) / dnorm};
}

/**
 * Trivial comparison operator between quaternions.
 */
bool operator==(const quaternion& lhs, const quaternion& rhs) {
    return lhs.a() == rhs.a() && lhs.b() == rhs.b() && lhs.c() == rhs.c() && lhs.d() == rhs.d();
}

/**
 * Stream operator for quaternion.
ostream& operator<< (ostream& os, const quaternion& obj) {
    os << "(" << obj.a() << "," << obj.b() << "," << obj.c() << "," << obj.d() << ")"; 
    return os;
}
*/

/**
 * Normalization function
 */
quaternion normalized(const quaternion& quat) {
    return quat / qabs(quat);
}

/**
 * Inverse function
 */
quaternion inverse(const quaternion& quat) {
    return conj(quat) / norm(quat);
}

}
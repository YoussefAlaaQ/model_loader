inline void 
swap(float *a, float *b){
	float tmp = *a;
	*a = *b;
	*b = tmp; 
}

inline void
reflectRectAroundY(Rect* reflectedRect, float reflectAroundPoint){
	float newX = reflectedRect->x - (reflectAroundPoint - reflectedRect->w);
	newX = -newX;
	newX += reflectAroundPoint;
	reflectedRect->x = newX;
}

#ifdef SCREEN_HEIGHT
inline float
flipY(float posY, float height){
    return SCREEN_HEIGHT - posY - height; 
}
#endif

inline float
clamp_float(float value, float min_val, float max_val) {
    if (value < min_val) {
        return min_val;
    } else if (value > max_val) {
        return max_val;
    } else {
        return value;
    }
}
    
inline internal float 
ConvertToScreenSpace(float cameraDim, float targetDim){
    return targetDim - cameraDim;
}

inline internal b32
point_in_box(v2 point, Rect box){
	//NOTE: this assume that the y is fliped in both
	if((point.x > box.x && point.x < box.x + box.w) && 
		(point.y > box.y && point.y < box.y + box.h))
	{
		return true;
	}
	else{
		return false;
	}
}

internal bool 
checkCollision(Rect a, Rect b){
    float leftA = a.x;
    float rightA = a.x + a.w;
    float bottomA = a.y;
    float topA = a.y + a.h;

    float leftB = b.x;
    float rightB = b.x + b.w;
    float bottomB = b.y;
    float topB = b.y + b.h;

    if(topA <= bottomB) return false;
    if(bottomA >= topB) return false;
    if(leftA >= rightB) return false;
    if(rightA <= leftB) return false;

    return true;
}

inline float
square(float x){
	return x * x;
}

inline float
radians(float degrees){
	float rad = degrees * (PI / 180.0f);
	
	return rad;
}

inline f32
lerp(f32 a, f32 b, f32 t){
	return a + t * (b - a);
}

inline v2
make_vec2(float x, float y){
	v2 result = {0};
	result.x = x;
	result.y = y;
	return result;
}

inline v3
make_vec3(float x, float y, float z){
	v3 result = {0};
	result.x = x;
	result.y = y;
	result.z = z;
	return result;
}

inline v3
make_vec3(float xyz){
	v3 result = {};
	result.x = xyz;
	result.y = xyz;
	result.z = xyz;
	return result;
}

inline v3
make_vec3(v4 v){
	v3 result = {};
	result.x = v.x;
	result.y = v.y;
	result.z = v.z;
	return result;
}

inline v4
make_vec4(float x, float y, float z, float w){
	v4 result = {0};
	result.x = x;
	result.y = y;
	result.z = z;
	result.w = w;
	return result;
}

inline v4
make_vec4(float xyzw){
	v4 result = {0};
	result.x = xyzw;
	result.y = xyzw;
	result.z = xyzw;
	result.w = xyzw;
	return result;
}

inline v3 
v3_cross(v3 vec1, v3 vec2){
	v3 result = {};
	result.x = vec1.y * vec2.z - vec1.z * vec2.y;
	result.y = vec1.z * vec2.x - vec1.x * vec2.z;
	result.z = vec1.x * vec2.y - vec1.y * vec2.x;
	
	return result;
}

inline f32
v3_dot(v3 a, v3 b){
	return (a.x * b.x + a.y * b.y + a.z * b.z);
}

inline v3 
v3_subtract(v3 vec1, v3 vec2){
	v3 result = {};
	result.x = vec1.x - vec2.x;
	result.y = vec1.y - vec2.y;
	result.z = vec1.z - vec2.z;
	
	return result;
}

inline v3
v3_lerp(v3 a, v3 b, f32 t){
	v3 result = {};
	result.x = lerp(a.x, b.x, t);
	result.y = lerp(a.y, b.y, t);
	result.z = lerp(a.z, b.z, t);
	return result;
}

inline v4 
v4_subtract(v4 vec1, v4 vec2){
	v4 result = {};
	result.x = vec1.x - vec2.x;
	result.y = vec1.y - vec2.y;
	result.z = vec1.z - vec2.z;
	result.w = vec1.w - vec2.w;
	
	return result;
}

inline v3 
v3_add(v3 vec1, v3 vec2){
	v3 result = {};
	result.x = vec1.x + vec2.x;
	result.y = vec1.y + vec2.y;
	result.z = vec1.z + vec2.z;
	
	return result;
}

inline v3 
v3_scale(v3 v, float scale){
	v3 result = make_vec3(v.x * scale, v.y * scale, v.z * scale);
	return result;
}

inline v4 
v4_scale(v4 v, float scale){
	v4 result = make_vec4(v.x * scale, v.y * scale, v.z * scale, v.w * scale);
	return result;
}

inline v3
v3_divide(v3 v, float dividor){
	v3 result = {};
	result.x = v.x / dividor;
	result.y = v.y / dividor;
	result.z = v.z / dividor;

	return result;
}

inline v4
v4_divide(v4 v, float dividor){
	v4 result = {};
	result.x = v.x / dividor;
	result.y = v.y / dividor;
	result.z = v.z / dividor;
	result.w = v.w / dividor;

	return result;
}

inline v3
v4_to_v3(v4 v){
	v3 result = {};
	result.x = v.x;
	result.y = v.y;
	result.z = v.z;
	return result;
}

inline v3
v3_component_multiply(v3 vec1, v3 vec2){
	return make_vec3(vec1.x * vec2.x, vec1.y * vec2.y, vec1.z * vec2.z);
}

inline float
v3_length(v3 vec){
	float result = sqrt(square(vec.x) + square(vec.y) + square(vec.z));
	return result;
}

inline v3 
operator*(v3 v, float scale){
	return v3_scale(v, scale);
}

inline v3 
operator*(f32 scale, v3 v){
	return v3_scale(v, scale);
}

inline v4 
operator*(v4 v, float scale){
	return v4_scale(v, scale);
}

inline v4 
operator*(f32 scale, v4 v){
	return v4_scale(v, scale);
}

inline v3 
operator*(v3 vec1, v3 vec2){
	return v3_component_multiply(vec1, vec2);
}

inline v3
operator/(v3 v, float dividor){
	return v3_divide(v, dividor);
}

inline v4
operator/(v4 v, float dividor){
	return v4_divide(v, dividor);
}

v3 
operator + (v3 vec1, v3 vec2){
	return v3_add(vec1, vec2);
}

v3 
operator - (v3 vec1, v3 vec2){
	return v3_subtract(vec1, vec2);
}

v4
operator - (v4 vec1, v4 vec2){
	return v4_subtract(vec1, vec2);
}

v3&
operator += (v3& vec1, const v3& vec2){
	vec1 = v3_add(vec1, vec2);
    return vec1;
}

v3&
operator -= (v3& vec1, const v3& vec2){
	vec1 = v3_subtract(vec1, vec2);
    return vec1;
}

v3&
operator /= (v3& vec1, const float f){
	vec1 = v3_divide(vec1, f);
    return vec1;
}

inline mat4
mat4_identity(){
	mat4 result = {0};
	result.m[0][0] = 1.0f; 
	result.m[1][1] = 1.0f; 
	result.m[2][2] = 1.0f; 
	result.m[3][3] = 1.0f;
	
	return result; 
}

inline mat4
mat4_multiply(mat4 m1, mat4 m2){
	mat4 result = {0};
	
	for(int row = 0; row < 4; ++row){
		for(int col = 0; col < 4; ++col){
			for(int k = 0; k < 4; ++k){
				result.m[row][col] += m1.m[row][k] * m2.m[k][col]; 
			}
		}
	}
	
	return result;
}

inline mat4
mat4_multiply(mat4 m1, float n){
	mat4 result = {};
	for(int row = 0; row < 4; ++row){
		for(int col = 0; col < 4; ++col){
			result.m[row][col] = m1.m[row][col] * n;
		}
	}
	return result;
}

mat4
operator * (mat4 m1, mat4 m2){
	return mat4_multiply(m1, m2);
}

mat4&
operator *= (mat4& m1, const mat4& m2){
	m1 = mat4_multiply(m1, m2);
    return m1;
}


mat4
operator * (mat4 m1, float n){
	return mat4_multiply(m1, n);
}

mat4
operator * (float n, mat4 m1){
	return mat4_multiply(m1, n);
}

float
mat2_determinant(mat2 mat){
	float determinant = 0.0f;
	determinant = mat.m[0][0] * mat.m[1][1] - mat.m[1][0] * mat.m[0][1];
	return determinant;
}

float
mat3_determinant(mat3 mat){
	return
        mat.m[0][0] * (mat.m[1][1] * mat.m[2][2] - mat.m[1][2] * mat.m[2][1])
      - mat.m[0][1] * (mat.m[1][0] * mat.m[2][2] - mat.m[1][2] * mat.m[2][0])
      + mat.m[0][2] * (mat.m[1][0] * mat.m[2][1] - mat.m[1][1] * mat.m[2][0]);
}

float
mat4_determinant(mat4 mat){
	float determinant = 0.0f;
	mat3 m1 = {};
	mat3 m2 = {};
	mat3 m3 = {};
	mat3 m4 = {};
	
	m1.m[0][0] = mat.m[1][1];
	m1.m[0][1] = mat.m[1][2];
	m1.m[0][2] = mat.m[1][3];
	m1.m[1][0] = mat.m[2][1];
	m1.m[1][1] = mat.m[2][2];
	m1.m[1][2] = mat.m[2][3];
	m1.m[2][0] = mat.m[3][1];
	m1.m[2][1] = mat.m[3][2];
	m1.m[2][2] = mat.m[3][3];
	
	m2.m[0][0] = mat.m[1][0];
	m2.m[0][1] = mat.m[1][2];
	m2.m[0][2] = mat.m[1][3];
	m2.m[1][0] = mat.m[2][0];
	m2.m[1][1] = mat.m[2][2];
	m2.m[1][2] = mat.m[2][3];
	m2.m[2][0] = mat.m[3][0];
	m2.m[2][1] = mat.m[3][2];
	m2.m[2][2] = mat.m[3][3];
	
	m3.m[0][0] = mat.m[1][0];
	m3.m[0][1] = mat.m[1][1];
	m3.m[0][2] = mat.m[1][3];
	m3.m[1][0] = mat.m[2][0];
	m3.m[1][1] = mat.m[2][1];
	m3.m[1][2] = mat.m[2][3];
	m3.m[2][0] = mat.m[3][0];
	m3.m[2][1] = mat.m[3][1];
	m3.m[2][2] = mat.m[3][3];
	
	m4.m[0][0] = mat.m[1][0];
	m4.m[0][1] = mat.m[1][1];
	m4.m[0][2] = mat.m[1][2];
	m4.m[1][0] = mat.m[2][0];
	m4.m[1][1] = mat.m[2][1];
	m4.m[1][2] = mat.m[2][2];
	m4.m[2][0] = mat.m[3][0];
	m4.m[2][1] = mat.m[3][1];
	m4.m[2][2] = mat.m[3][2];
	
	determinant = mat.m[0][0] * mat3_determinant(m1)
				- mat.m[0][1] * mat3_determinant(m2)
				+ mat.m[0][2] * mat3_determinant(m3)
				- mat.m[0][3] * mat3_determinant(m4);
	
	return determinant;
}

mat4
mat4_transpose(mat4 mat){
	mat4 transpose = {};
	for(int row = 0; row < 4; ++row){
		for(int col = 0; col < 4; ++col){
			transpose.m[row][col] = mat.m[col][row];
		}
	}
	return transpose;
}

// mat4
// mat4_minors(mat4 mat){
	// mat4 minors = {};
	// for(int row = 0; row < 4; ++row){
		// for(int col = 0; col < 4; ++col){
			// int cRow = 0;
			// int cCol = 0;
			// mat3 tmp = {};
			// for(int r = 0; r < 4; ++r){
				// if(r == row) continue;
				// cCol = 0;
				// for(int c = 0; c < 4; ++c){
					// if(c == col) continue;
					// tmp.m[cRow][cCol] = mat.m[r][c]; 
					// cCol++;
				// }
				// cRow++;
			// }
			// float det = mat3_determinant(tmp);
			// minors.m[row][col] = det;
		// }
	// }

	// return minors;
// }

// mat4
// mat4_inverse(mat4 mat){
	// mat4 inverse = {};
	// mat4 minors = mat4_minors(mat);
	
	// minors.m[0][1] = -minors.m[0][1];
	// minors.m[0][3] = -minors.m[0][3];
	// minors.m[1][0] = -minors.m[1][0];
	// minors.m[1][2] = -minors.m[1][2];
	// minors.m[2][1] = -minors.m[2][1];
	// minors.m[2][3] = -minors.m[2][3];
	// minors.m[3][0] = -minors.m[3][0];
	// minors.m[3][2] = -minors.m[3][2];
	
	// mat4 adjoint = mat4_transpose(minors);

	// float det = 0.0f;
	// det = mat.m[0][0] * minors.m[0][0]
		// + mat.m[0][1] * minors.m[0][1]
		// + mat.m[0][2] * minors.m[0][2]
		// + mat.m[0][3] * minors.m[0][3];
	// if(fabsf(det) < 1e-6f){
		// printf("The matrix has no inverse \n");
		// return {0};
	// }
	// float invDet = 1.0f / det;  
	// inverse = invDet * adjoint;
	
	// return inverse;
// }

// mat4
// new_mat4_inverse(mat4 m){
    // float A2323 = m.m[2][2] * m.m[3][3] - m.m[2][3] * m.m[3][2];
    // float A1323 = m.m[2][1] * m.m[3][3] - m.m[2][3] * m.m[3][1];
    // float A1223 = m.m[2][1] * m.m[3][2] - m.m[2][2] * m.m[3][1];
    // float A0323 = m.m[2][0] * m.m[3][3] - m.m[2][3] * m.m[3][0];
    // float A0223 = m.m[2][0] * m.m[3][2] - m.m[2][2] * m.m[3][0];
    // float A0123 = m.m[2][0] * m.m[3][1] - m.m[2][1] * m.m[3][0];
    // float A2313 = m.m[1][2] * m.m[3][3] - m.m[1][3] * m.m[3][2];
    // float A1313 = m.m[1][1] * m.m[3][3] - m.m[1][3] * m.m[3][1];
    // float A1213 = m.m[1][1] * m.m[3][2] - m.m[1][2] * m.m[3][1];
    // float A2312 = m.m[1][2] * m.m[2][3] - m.m[1][3] * m.m[2][2];
    // float A1312 = m.m[1][1] * m.m[2][3] - m.m[1][3] * m.m[2][1];
    // float A1212 = m.m[1][1] * m.m[2][2] - m.m[1][2] * m.m[2][1];
    // float A0313 = m.m[1][0] * m.m[3][3] - m.m[1][3] * m.m[3][0];
    // float A0213 = m.m[1][0] * m.m[3][2] - m.m[1][2] * m.m[3][0];
    // float A0312 = m.m[1][0] * m.m[2][3] - m.m[1][3] * m.m[2][0];
    // float A0212 = m.m[1][0] * m.m[2][2] - m.m[1][2] * m.m[2][0];
    // float A0113 = m.m[1][0] * m.m[3][1] - m.m[1][1] * m.m[3][0];
    // float A0112 = m.m[1][0] * m.m[2][1] - m.m[1][1] * m.m[2][0];
    // float A0012 = m.m[0][0] * m.m[1][1] - m.m[0][1] * m.m[1][0];

    // float det = m.m[0][0] * (m.m[1][1] * A2323 - m.m[1][2] * A1323 + m.m[1][3] * A1223)
              // - m.m[0][1] * (m.m[1][0] * A2323 - m.m[1][2] * A0323 + m.m[1][3] * A0223)
              // + m.m[0][2] * (m.m[1][0] * A1323 - m.m[1][1] * A0323 + m.m[1][3] * A0123)
              // - m.m[0][3] * (m.m[1][0] * A1223 - m.m[1][1] * A0223 + m.m[1][2] * A0123);

    // if(fabsf(det) < 1e-6f){
        // printf("no inverse\n");
        // return {};
    // }

    // float invDet = 1.0f / det;

    // mat4 result = {};
    // result.m[0][0] =  invDet * (m.m[1][1] * A2323 - m.m[1][2] * A1323 + m.m[1][3] * A1223);
    // result.m[0][1] = -invDet * (m.m[0][1] * A2323 - m.m[0][2] * A1323 + m.m[0][3] * A1223);
    // result.m[0][2] =  invDet * (m.m[0][1] * A2313 - m.m[0][2] * A1313 + m.m[0][3] * A1213);
    // result.m[0][3] = -invDet * (m.m[0][1] * A2312 - m.m[0][2] * A1312 + m.m[0][3] * A1212);
    // result.m[1][0] = -invDet * (m.m[1][0] * A2323 - m.m[1][2] * A0323 + m.m[1][3] * A0223);
    // result.m[1][1] =  invDet * (m.m[0][0] * A2323 - m.m[0][2] * A0323 + m.m[0][3] * A0223);
    // result.m[1][2] = -invDet * (m.m[0][0] * A2313 - m.m[0][2] * A0313 + m.m[0][3] * A0213);
    // result.m[1][3] =  invDet * (m.m[0][0] * A2312 - m.m[0][2] * A0312 + m.m[0][3] * A0212);
    // result.m[2][0] =  invDet * (m.m[1][0] * A1323 - m.m[1][1] * A0323 + m.m[1][3] * A0123);
    // result.m[2][1] = -invDet * (m.m[0][0] * A1323 - m.m[0][1] * A0323 + m.m[0][3] * A0123);
    // result.m[2][2] =  invDet * (m.m[0][0] * A1313 - m.m[0][1] * A0313 + m.m[0][3] * A0113);
    // result.m[2][3] = -invDet * (m.m[0][0] * A1312 - m.m[0][1] * A0312 + m.m[0][3] * A0112);
    // result.m[3][0] = -invDet * (m.m[1][0] * A1223 - m.m[1][1] * A0223 + m.m[1][2] * A0123);
    // result.m[3][1] =  invDet * (m.m[0][0] * A1223 - m.m[0][1] * A0223 + m.m[0][2] * A0123);
    // result.m[3][2] = -invDet * (m.m[0][0] * A1213 - m.m[0][1] * A0213 + m.m[0][2] * A0113);
    // result.m[3][3] =  invDet * (m.m[0][0] * A1212 - m.m[0][1] * A0112 + m.m[0][2] * A0012);

    // return result;
// }

inline void
v3_normalize(v3* vec){
	//todo: see if i can optimize the power and sqrt
	float length = sqrtf(vec->x * vec->x +
                         vec->y * vec->y +
                         vec->z * vec->z);
    if (length != 0.0f){
        vec->x /= length;
        vec->y /= length;
        vec->z /= length;
    } 
}

inline v3
v3_normalize(v3 vec){
	//todo: see if i can optimize the power and sqrt
	v3 normalized = {};
	float length = sqrtf(vec.x * vec.x +
                         vec.y * vec.y +
                         vec.z * vec.z);
    if (length != 0.0f){
        normalized.x = vec.x / length;
        normalized.y = vec.y / length;
        normalized.z = vec.z / length;
		
		return normalized;
    }
    
    return vec;
}

inline mat4
mat4_translate(v3 v){
	mat4 result = mat4_identity();
	result.m[3][0] = v.x;
	result.m[3][1] = v.y;
	result.m[3][2] = v.z;
	
	return result;
}

inline mat4
mat4_translate(float x, float y, float z){
	mat4 result = mat4_identity();
	result.m[3][0] = x;
	result.m[3][1] = y;
	result.m[3][2] = z;
	
	return result;
}

inline mat4
mat4_scale(float xyz){
	mat4 result = mat4_identity();
	result.m[0][0] = xyz;
	result.m[1][1] = xyz;
	result.m[2][2] = xyz;
	
	return result;
}

inline mat4
mat4_scale(float x, float y, float z){
	mat4 result = mat4_identity();
	result.m[0][0] = x;
	result.m[1][1] = y;
	result.m[2][2] = z;
	
	return result;
}

inline mat4
mat4_scale(v3 scaler){
	mat4 result = mat4_identity();
	result.m[0][0] = scaler.x;
	result.m[1][1] = scaler.y;
	result.m[2][2] = scaler.z;
	
	return result;
}

inline bool 
v3_isNormalized(v3 vec){
	float lengthSq = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
	
	const float epsilon = 1e-6f;
	
	//normaly we will check if lengthSq == 1 but the vectorlength may be very close to 1 ex: 0.99999998
	//we will handle it as a normalized vector 
	if (fabsf(lengthSq - 1.0f) > epsilon) {
		return false;
	}
	return true;
}

//this is implementation of Rodrigue's Rotation Formula
//i really don't understand this
//study: rodrigue's rotation Formula

inline mat4
mat4_rotate(v3 axis, float radians){
	if(!v3_isNormalized(axis)){
		v3_normalize(&axis);
	}
 
    mat4 Result = mat4_identity();
    float c = cosf(radians);
    float s = sinf(radians);
    float oneMinusC = 1.0f - c;

    float x = axis.x;
    float y = axis.y;
    float z = axis.z;

    Result.m[0][0] = c + oneMinusC * x * x;
    Result.m[0][1] = oneMinusC * x * y + s * z;
    Result.m[0][2] = oneMinusC * x * z - s * y;

    Result.m[1][0] = oneMinusC * y * x - s * z;
    Result.m[1][1] = c + oneMinusC * y * y;
    Result.m[1][2] = oneMinusC * y * z + s * x;

    Result.m[2][0] = oneMinusC * z * x + s * y;
    Result.m[2][1] = oneMinusC * z * y - s * x;
    Result.m[2][2] = c + oneMinusC * z * z;

    return Result;
}

inline v4
v4_multiply_mat4(v4 vec, mat4 mat){
    v4 result = {0};
    result.x = vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + vec.w * mat.m[3][0];
    result.y = vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + vec.w * mat.m[3][1];
    result.z = vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + vec.w * mat.m[3][2];
    result.w = vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + vec.w * mat.m[3][3];
    return result;
}

inline v4 
operator*(v4 v, mat4 mat){
	return v4_multiply_mat4(v, mat);
}

//NOTE: this implementation will only work for a right handed system meaning
//it will give wrong result for something like directx or vulken  
inline mat4
mat4_perspective_infinite(float fov, float aspectRatio, float nearPlane){
	mat4 result = {0};
	
	float tanHalfView = tanf(fov / 2.0f);
	float top = nearPlane * tanHalfView;
	float right = top * aspectRatio;
	result.m[0][0] = nearPlane / right;
	result.m[1][1] = nearPlane / top;
	result.m[2][2] = -1;
	
	result.m[2][3] = -1;
	result.m[3][2] = -2 * nearPlane;

	
	return result;
}

inline mat4
mat4_perspective(float fov, float aspectRatio, float nearPlane, float farPlane){
    mat4 result = {0};
    
    float tanHalfFov = tanf(fov / 2.0f);
	float top   =  nearPlane * tanHalfFov;
	float bottom = -nearPlane * tanHalfFov;
	float right  =  top * aspectRatio;
	float left   = -top * aspectRatio;

    // result.m[0][0] =  nearPlane / right;
    // result.m[1][1] =  nearPlane / top;
    // result.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
    // result.m[2][3] = -1.0f;
    // result.m[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);

	result.m[0][0] = 2.0f * nearPlane / (right - left);
	result.m[1][1] = 2.0f * nearPlane / (top - bottom);
	result.m[2][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);
	
	result.m[2][0] = (right + left) / (right - left);  
	result.m[2][1] = (top + bottom) / (top - bottom);
	result.m[2][3] = -1.0f;
	result.m[3][2] = -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane);
	  
    return result;
}

inline mat4
mat4_orthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane){
	mat4 result = mat4_identity();

	result.m[0][0] =  2.0f / (right - left);
    result.m[1][1] =  2.0f / (top - bottom);
    result.m[2][2] = -2.0f / (farPlane - nearPlane);
    result.m[3][3] =  1.0f;

	result.m[3][0] = -(right + left)   / (right - left);
    result.m[3][1] = -(top   + bottom) / (top   - bottom);
    result.m[3][2] = -(farPlane + nearPlane) / (farPlane - nearPlane);

	return result;
}

mat4 
mat4_lookAt(v3 camPos, v3 target, v3 up){
	mat4 lookAt = mat4_identity();
	
	v3 camDirection = v3_normalize(camPos - target);
	v3 camRight = v3_normalize(v3_cross(up, camDirection));
	v3 camUp = {};
	camUp = v3_cross(camDirection, camRight);
	
	mat4 rotate = {};
	mat4 translate = mat4_identity();
	translate = translate * mat4_translate(make_vec3(-camPos.x, -camPos.y, -camPos.z));
	
	
	rotate.m[0][0] = camRight.x;
	rotate.m[0][1] = camUp.x;
	rotate.m[0][2] = camDirection.x;
	rotate.m[0][3] = 0;
	
	rotate.m[1][0] = camRight.y;
	rotate.m[1][1] = camUp.y;
	rotate.m[1][2] = camDirection.y;
	rotate.m[1][3] = 0;
	
	rotate.m[2][0] = camRight.z;
	rotate.m[2][1] = camUp.z;
	rotate.m[2][2] = camDirection.z;
	rotate.m[2][3] = 0;
	
	 
	rotate.m[3][0] = 0;
	rotate.m[3][1] = 0;
	rotate.m[3][2] = 0;
	rotate.m[3][3] = 1;
	
	lookAt = translate * rotate;
	return lookAt;
}

inline mat3
mat4_to_mat3(mat4 m){
	mat3 result = {};
	for(int row = 0; row < 3; ++row){
		for(int col = 0; col < 3; ++col){
			result.m[row][col] = m.m[row][col];
		}
	}
	
	return result;
}

inline mat4
mat3_to_mat4(mat3 m){
	mat4 result = mat4_identity();
	for(int row = 0; row < 3; ++row){
		for(int col = 0; col < 3; ++col){
			result.m[row][col] = m.m[row][col];
		}
	}
	
	return result;
}

mat4 
mat4_inverse(mat4 mat)
{
	// code is from: https://rodolphe-vaillant.fr/entry/7/c-code-for-4x4-matrix-inversion/jumble
	
	f32 *m = mat.e;
	// convert_mat4_to_1DArray(mat, m);
    float inv[16], det;
    float invOut[16];
    int i;
  
    inv[ 0] =  m[5] * m[10] * m[15] - m[5] * m[14] * m[11] - m[6] * m[9] * m[15] + m[6] * m[13] * m[11] + m[7] * m[9] * m[14] - m[7] * m[13] * m[10];
    inv[ 1] = -m[1] * m[10] * m[15] + m[1] * m[14] * m[11] + m[2] * m[9] * m[15] - m[2] * m[13] * m[11] - m[3] * m[9] * m[14] + m[3] * m[13] * m[10];
    inv[ 2] =  m[1] * m[ 6] * m[15] - m[1] * m[14] * m[ 7] - m[2] * m[5] * m[15] + m[2] * m[13] * m[ 7] + m[3] * m[5] * m[14] - m[3] * m[13] * m[ 6];
    inv[ 3] = -m[1] * m[ 6] * m[11] + m[1] * m[10] * m[ 7] + m[2] * m[5] * m[11] - m[2] * m[ 9] * m[ 7] - m[3] * m[5] * m[10] + m[3] * m[ 9] * m[ 6];
    inv[ 4] = -m[4] * m[10] * m[15] + m[4] * m[14] * m[11] + m[6] * m[8] * m[15] - m[6] * m[12] * m[11] - m[7] * m[8] * m[14] + m[7] * m[12] * m[10];
    inv[ 5] =  m[0] * m[10] * m[15] - m[0] * m[14] * m[11] - m[2] * m[8] * m[15] + m[2] * m[12] * m[11] + m[3] * m[8] * m[14] - m[3] * m[12] * m[10];
    inv[ 6] = -m[0] * m[ 6] * m[15] + m[0] * m[14] * m[ 7] + m[2] * m[4] * m[15] - m[2] * m[12] * m[ 7] - m[3] * m[4] * m[14] + m[3] * m[12] * m[ 6];
    inv[ 7] =  m[0] * m[ 6] * m[11] - m[0] * m[10] * m[ 7] - m[2] * m[4] * m[11] + m[2] * m[ 8] * m[ 7] + m[3] * m[4] * m[10] - m[3] * m[ 8] * m[ 6];
    inv[ 8] =  m[4] * m[ 9] * m[15] - m[4] * m[13] * m[11] - m[5] * m[8] * m[15] + m[5] * m[12] * m[11] + m[7] * m[8] * m[13] - m[7] * m[12] * m[ 9];
    inv[ 9] = -m[0] * m[ 9] * m[15] + m[0] * m[13] * m[11] + m[1] * m[8] * m[15] - m[1] * m[12] * m[11] - m[3] * m[8] * m[13] + m[3] * m[12] * m[ 9];
    inv[10] =  m[0] * m[ 5] * m[15] - m[0] * m[13] * m[ 7] - m[1] * m[4] * m[15] + m[1] * m[12] * m[ 7] + m[3] * m[4] * m[13] - m[3] * m[12] * m[ 5];
    inv[11] = -m[0] * m[ 5] * m[11] + m[0] * m[ 9] * m[ 7] + m[1] * m[4] * m[11] - m[1] * m[ 8] * m[ 7] - m[3] * m[4] * m[ 9] + m[3] * m[ 8] * m[ 5];
    inv[12] = -m[4] * m[ 9] * m[14] + m[4] * m[13] * m[10] + m[5] * m[8] * m[14] - m[5] * m[12] * m[10] - m[6] * m[8] * m[13] + m[6] * m[12] * m[ 9];
    inv[13] =  m[0] * m[ 9] * m[14] - m[0] * m[13] * m[10] - m[1] * m[8] * m[14] + m[1] * m[12] * m[10] + m[2] * m[8] * m[13] - m[2] * m[12] * m[ 9];
    inv[14] = -m[0] * m[ 5] * m[14] + m[0] * m[13] * m[ 6] + m[1] * m[4] * m[14] - m[1] * m[12] * m[ 6] - m[2] * m[4] * m[13] + m[2] * m[12] * m[ 5];
    inv[15] =  m[0] * m[ 5] * m[10] - m[0] * m[ 9] * m[ 6] - m[1] * m[4] * m[10] + m[1] * m[ 8] * m[ 6] + m[2] * m[4] * m[ 9] - m[2] * m[ 8] * m[ 5];
  
    det = m[0] * inv[0] + m[4] * inv[1] + m[8] * inv[2] + m[12] * inv[3];
  
    if(det == 0){
        printf("no inverse \n");
        return {{}};
	}
    det = 1.f / det;
  
    for(i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

	mat4 result;
	int c = 0;
	for(int row = 0; row < 4; ++row){
		for(int col = 0; col < 4; ++col){
			result.m[row][col] = invOut[c++];
		}
	}
	
    return result;
}


inline internal void
get_frustum_corners_world_space(mat4 vp, v4* corners){
	mat4 inv = mat4_inverse(vp);
	int i = 0;
	v4 corner = {};
	for(int x = 0; x < 2; ++x){
		for(int y = 0; y < 2; ++y){
			for(int z = 0; z < 2; ++z){
				corner = make_vec4(2.0f * x - 1.0f
								, 2.0f * y - 1.0f
								, 2.0f * z - 1.0f
								, 1.0f)
								* inv;
				
				corners[i++] = corner / corner.w;
			}
		}
	}
}

inline internal Quaternion 
Quat(f32 x, f32 y, f32 z, f32 w){
	Quaternion result = {};
	result.v.x = x;
	result.v.y = y;
	result.v.z = z;
	result.s   = w;
	
	return Quat_normalize(result);
}

inline internal Quaternion 
Quat(v3 v, f32 s){
	Quaternion result = {};
	result.s = s;
	result.v = v;
	return result;
}

inline internal Quaternion
Quat_pure(v3 v){
	Quaternion result = {};
	result.v = v;
	return result;
}

inline internal Quaternion
Quat_real(f32 s){
	Quaternion result = {};
	result.s = s;
	return result;
}

inline internal Quaternion
Quat_from_v4(v4 v){
	Quaternion result = {};
	result.x = v.x;
	result.y = v.y;
	result.z = v.z;
	result.w = v.w;

	return result;
}

inline internal Quaternion
Quat_add(Quaternion a, Quaternion b){
	Quaternion result = {};
	result.s = a.s + b.s;
	result.v = a.v + b.v;
	return result;
}

inline Quaternion 
operator+(Quaternion a, Quaternion b){
	return Quat_add(a, b);
}

inline internal Quaternion
Quat_sub(Quaternion a, Quaternion b){
	Quaternion result = {};
	result.s = a.s - b.s;
	result.v = a.v - b.v;
	return result;
}

inline Quaternion 
operator-(Quaternion a, Quaternion b){
	return Quat_sub(a, b);
}

inline internal Quaternion
Quat_multiply(Quaternion a, Quaternion b){
	Quaternion result = {};
	result.s = a.s * b.s - v3_dot(a.v, b.v);
	result.v = a.s * b.v + b.s * a.v + v3_cross(a.v, b.v);
	return result;
}

inline Quaternion 
operator*(Quaternion a, Quaternion b){
	return Quat_multiply(a, b);
}

inline internal Quaternion
Quat_multiply(Quaternion a, f32 scalar){
	Quaternion result = {};
	result.s = a.s * scalar;
	result.v = a.v * scalar;
	return result;
}

inline Quaternion 
operator*(Quaternion a, f32 scalar){
	return Quat_multiply(a, scalar);
}
inline Quaternion 
operator*(f32 scalar, Quaternion a){
	return Quat_multiply(a, scalar);
}

inline internal Quaternion
Quat_conjugate(Quaternion quat){
	Quaternion result = {};
	result.w =  quat.w;
	result.x = -quat.x;
	result.y = -quat.y;
	result.z = -quat.z;
	return result;
}

inline internal Quaternion
Quat_negate(Quaternion quat){
	Quaternion result = {};
	result.w = -quat.w;
	result.x = -quat.x;
	result.y = -quat.y;
	result.z = -quat.z;
	return result;
}

inline internal f32
Quat_length_squared(Quaternion quat){
	f32 result = quat.w * quat.w + 
				 quat.x * quat.x +
				 quat.y * quat.y +
				 quat.z * quat.z;

	return result;
}

inline internal f32
Quat_length(Quaternion quat){
	return sqrtf(Quat_length_squared(quat));
}

inline internal Quaternion
Quat_normalize(Quaternion quat){
	f32 len = Quat_length(quat);
	return Quat_multiply(quat, 1.0f/len);
}

inline internal Quaternion
Quat_inverse(Quaternion quat){
	f32 lensq = Quat_length_squared(quat);
	Assert(lensq > EPSILON);
	return Quat_multiply(Quat_conjugate(quat), 1.0f/lensq);
}

inline internal f32
Quat_dot(Quaternion a, Quaternion b){
	f32 result = a.w * b.w +
			     a.x * b.x +
			     a.y * b.y +
			     a.z * b.z;
	
	return result;
}

// i (1,0,0)
// j (0,1,0)
// k (0,0,1)

inline internal Quaternion
Quat_from_axis_angle(v3 axis, f32 radians){
	axis = v3_normalize(axis);
	f32 rad = radians * 0.5f;
	f32 c = cosf(rad);
	f32 s = sinf(rad);
	return Quat(s * axis, c);
}

// inline internal Quaternion
// Quat_rotate_by_axis_angle(Quaternion orient, v3 axis, f32 radians){
	// Quaternion quat = Quat_from_axis_angle(axis, radians);
	// Quaternion conj = Quat_conjugate(quat);
	
	// Quaternion result = Quat_multiply(Quat_multiply(quat, orient)
									  // , conj);
	// return result;
// }

inline internal v3
Quat_rotate_vector(Quaternion q, v3 v){
	Quaternion p = Quat_pure(v);                    // treat v as a pure quaternion (s=0)
	Quaternion result = Quat_multiply(Quat_multiply(q, p), Quat_conjugate(q));
	return result.v;                                 // rotated vector; result.s should be ~0
}

internal mat4
Quat_to_mat4(Quaternion orient){
	mat4 result = {};
	f32 x = orient.x;
	f32 y = orient.y;
	f32 z = orient.z;
	f32 s = orient.w;
	result.m[0][0] = 1 - 2 * y * y - 2 * z * z;
	result.m[1][0] = 2 * x * y - 2 * s * z;
	result.m[2][0] = 2 * x * z + 2 * s * y;
	
	result.m[0][1] = 2 * x * y + 2 * s * z;
	result.m[1][1] = 1 - 2 * x * x - 2 * z * z;
	result.m[2][1] = 2 * y * z - 2 * s * x; 
	
	result.m[0][2] = 2 * x * z - 2 * s * y;
	result.m[1][2] = 2 * y * z + 2 * s * x;
	result.m[2][2] = 1 - 2 * x * x - 2 * y * y;
	result.m[3][3] = 1;
	return result;
}

internal Quaternion
Quat_slerp(Quaternion a, Quaternion b, f32 t){
	Quaternion result = {};
	f32 dot = Quat_dot(a, b);
	if(dot < 0.0f){
		dot = -dot;
		b = Quat_negate(b);
	}
	f32 cosTheta = dot / (Quat_length(a) * Quat_length(b));
	// f32 cosTheta = dot; 
	
	// if a=b or a=-b then theta = 0 and we can return a
	if (fabsf(cosTheta) >= 1.0){
		result.w = a.w;result.x = a.x;result.y = a.y;result.z = a.z;
		return result;
	}
	f32 sinTheta = sqrtf(1.0 - cosTheta*cosTheta);
	
	// if theta = 180 degrees then result is not fully defined
	// we could rotate around any axis normal to qa or qb
	if (fabs(sinTheta) < 0.001){ // fabs is floating point absolute
		result.w = (a.w * 0.5 + b.w * 0.5);
		result.x = (a.x * 0.5 + b.x * 0.5);
		result.y = (a.y * 0.5 + b.y * 0.5);
		result.z = (a.z * 0.5 + b.z * 0.5);
		return result;
	}
	f32 theta = acosf(cosTheta);
	result = (sinf((1.0f - t) * theta) / sinTheta) * a + (sinf(t * theta) / sinTheta) * b;
	return result;
}

void print_mat4(const mat4& mat)
{
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            printf("%f ", mat.m[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}

void print_v3(v3 v){
	printf("v.x: %f, v.y: %f, v.z: %f\n", v.x, v.y, v.z);
}

void print_v3(v2 v){
	printf("v.x: %f, v.y: %f\n", v.x, v.y);
}

void print_quat(Quaternion q){
	printf("q.x: %f, q.y: %f, q.z: %f, q.w: %f\n", q.x, q.y, q.z, q.w);
}


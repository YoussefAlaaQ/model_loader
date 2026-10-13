#include <math.h>

#define PI 3.14159265358979323846
const f32 EPSILON = 1e-8f;

struct Rect{
	f32 x, y, w, h;
};

union v2{
    f32 e[2];
    struct {
        f32 x, y;
    };
};

union v2i{
    s32 e[2];
    struct {
        s32 x, y;
    };
};

union v3{
    f32 e[3];
    struct {
        f32 x, y, z;
    };
};

union v3i{
    s32 e[3];
    struct {
        s32 x, y, z;
    };
};

union v4{
    f32 e[4];
    struct {
        f32 x, y, z, w;
    };
};

union v4i{
    s32 e[4];
    struct {
        s32 x, y, z, w;
    };
};

union mat2{
    f32 e[4];
    struct{
        f32 m[2][2];
    };
};

union mat3{
    f32 e[9];
    struct{
        f32 m[3][3];
    };
};

union mat4{
    f32 e[16];
    struct{
        f32 m[4][4];
    };
};

union Quaternion
{
    f32 e[4];
    struct
    {
        v3 v; f32 s;
    };
    struct
    {
        f32 x, y, z, w;
    };
};

inline f32 flipY(f32 posY, f32 height);

inline f32 clamp_float(f32 value, f32 min_val, f32 max_val);
inline f32 lerp(f32 a, f32 b, f32 t);
inline internal f32 ConvertToScreenSpace(f32 cameraDim, f32 targetDim);
inline internal f32 flipYInMeters(f32 posYm, f32 heightm);

inline internal b32 point_in_box(v2 point, SDL_FRect box);

internal bool checkCollision(SDL_FRect a, SDL_FRect b);

inline f32 square(f32 x);
inline f32 radians(f32 degrees);
inline v2 make_vec2(f32 x, f32 y);
inline v3 make_vec3(f32 x, f32 y, f32 z);
inline v3 make_vec3(f32 xyz);
inline v4 make_vec4(f32 x, f32 y, f32 z, f32 w);
inline v4 make_vec4(f32 xyzw);
inline v3 v3_cross(v3 vec1, v3 vec2);
inline v3 v3_subtract(v3 vec1, v3 vec2);
inline v3 v3_add(v3 vec1, v3 vec2);
inline v3 v3_scale(v3 v, f32 scale);
inline v3 v3_divide(v3 v, f32 dividor);
inline v3 v3_component_multiply(v3 vec1, v3 vec2);
inline f32 v3_length(v3 vec);
inline bool v3_isNormalized(v3 vec);
inline void v3_normalize(v3* vec);
inline v3 v3_normalize(v3 vec);
inline v3 operator*(v3 v, f32 scale);
inline v3 operator*(v3 vec1, v3 vec2);
inline v3 operator/(v3 v, f32 dividor);
v3 operator + (v3 vec1, v3 vec2);
v3 operator - (v3 vec1, v3 vec2);
v3& operator += (v3& vec1, const v3& vec2);
v3& operator -= (v3& vec1, const v3& vec2);

inline mat4 mat4_identity();
inline mat4 mat4_multiply(mat4 m1, mat4 m2);
inline mat4 mat4_translate(v3 v);
inline mat4 mat4_translate(f32 x, f32 y, f32 z);
inline mat4 mat4_scale(f32 xyz);
inline mat4 mat4_scale(f32 x, f32 y, f32 z);
inline mat4 mat4_Scale(v3 scaler);
inline mat4 mat4_rotate(v3 axis, f32 radians);
inline v4 v4_multiply_mat4(v4 vec, mat4 mat);
mat4 operator * (mat4 m1, mat4 m2);
mat4& operator *= (mat4& m1, const mat4& m2);
inline mat4 mat4_perspective(f32 fov, f32 aspectRatio, f32 nearPlane, f32 far);
inline mat4 mat4_orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 nearPlane, f32 farPlane);
mat4 mat4_lookAt(v3 camPos, v3 target, v3 up);

inline mat3 mat4_to_mat3(mat4 m);
inline mat4 mat3_to_mat4(mat3 m);

inline internal Quaternion Quat(f32 x, f32 y, f32 z, f32 w);
inline internal Quaternion Quat(v3 v, f32 s);
inline internal Quaternion Quat_pure(v3 v);
inline internal Quaternion Quat_real(f32 s);
inline internal Quaternion Quat_from_v4(v4 v);
inline internal Quaternion Quat_add(Quaternion a, Quaternion b);
inline Quaternion operator+(Quaternion a, Quaternion b);
inline internal Quaternion Quat_sub(Quaternion a, Quaternion b);
inline Quaternion operator-(Quaternion a, Quaternion b);
inline internal Quaternion Quat_multiply(Quaternion a, Quaternion b);
inline Quaternion operator*(Quaternion a, Quaternion b);
inline internal Quaternion Quat_multiply(Quaternion a, f32 scalar);
inline Quaternion operator*(Quaternion a, f32 scalar);
inline Quaternion operator*(f32 scalar, Quaternion a);
inline internal Quaternion Quat_conjugate(Quaternion quat);
inline internal Quaternion Quat_negate(Quaternion quat);
inline internal f32 Quat_length_squared(Quaternion quat);
inline internal f32 Quat_length(Quaternion quat);
inline internal Quaternion Quat_normalize(Quaternion quat);
inline internal Quaternion Quat_inverse(Quaternion quat);
inline internal f32 Quat_dot(Quaternion a, Quaternion b);
inline internal Quaternion Quat_from_axis_angle(v3 axis, f32 radians);
inline internal v3 Quat_rotate_vector(Quaternion q, v3 v);
internal mat4 Quat_to_mat4(Quaternion orient);
internal Quaternion Quat_slerp(Quaternion a, Quaternion b, f32 t);


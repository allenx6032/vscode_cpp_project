#include "matrix4.h"

#include <stdlib.h>

#include "logging.h"

matrix4 matrix4_create(vector4 row1, vector4 row2, vector4 row3, vector4 row4) {
    matrix4 ret;
    ret.row1 = row1;
    ret.row2 = row2;
    ret.row3 = row3;
    ret.row4 = row4;
    return ret;
}

matrix4 matrix4_identity() {
    matrix4 ret = matrix4_create(
        vector4_create(1.0f, 0.0f, 0.0f, 0.0f),
        vector4_create(0.0f, 1.0f, 0.0f, 0.0f),
        vector4_create(0.0f, 0.0f, 1.0f, 0.0f),
        vector4_create(0.0f, 0.0f, 0.0f, 1.0f));
    return ret;
}

float* matrix4_value_ptr(matrix4 m) {
    float* ret = malloc(sizeof(matrix4));

    ret[0] = m.row1.x;
    ret[1] = m.row1.y;
    ret[2] = m.row1.z;
    ret[3] = m.row1.w;

    ret[4] = m.row2.x;
    ret[5] = m.row2.y;
    ret[6] = m.row2.z;
    ret[7] = m.row2.w;

    ret[8] = m.row3.x;
    ret[9] = m.row3.y;
    ret[10] = m.row3.z;
    ret[11] = m.row3.w;

    ret[12] = m.row4.x;
    ret[13] = m.row4.y;
    ret[14] = m.row4.z;
    ret[15] = m.row4.w;

    return ret;
}

matrix4 matrix4_create_by_value(float val) {
    matrix4 ret = matrix4_create(
        vector4_create(val, val, val, val),
        vector4_create(val, val, val, val),
        vector4_create(val, val, val, val),
        vector4_create(val, val, val, val));
    return ret;
}

bool8 matrix4_compare(matrix4 m1, matrix4 m2) {
    return vector4_compare(m1.row1, m2.row1) &&
           vector4_compare(m1.row2, m2.row2) &&
           vector4_compare(m1.row3, m2.row3) &&
           vector4_compare(m1.row4, m2.row4);
}

matrix4 matrix4_scaler_addition(matrix4 m, float k) {
    matrix4 ret = matrix4_create(m.row1, m.row2, m.row3, m.row4);
    vector4_add_scaler(&ret.row1, k);
    vector4_add_scaler(&ret.row2, k);
    vector4_add_scaler(&ret.row3, k);
    vector4_add_scaler(&ret.row4, k);
    return ret;
}

void matrix4_add_scaler(matrix4* m, float k) {
    vector4_add_scaler(&m->row1, k);
    vector4_add_scaler(&m->row2, k);
    vector4_add_scaler(&m->row3, k);
    vector4_add_scaler(&m->row4, k);
}

matrix4 matrix4_scaler_subtraction(matrix4 m, float k) {
    matrix4 ret = matrix4_create(m.row1, m.row2, m.row3, m.row4);
    vector4_sub_scaler(&ret.row1, k);
    vector4_sub_scaler(&ret.row2, k);
    vector4_sub_scaler(&ret.row3, k);
    vector4_sub_scaler(&ret.row4, k);
    return ret;
}

void matrix4_sub_scaler(matrix4* m, float k) {
    vector4_sub_scaler(&m->row1, k);
    vector4_sub_scaler(&m->row2, k);
    vector4_sub_scaler(&m->row3, k);
    vector4_sub_scaler(&m->row4, k);
}

matrix4 matrix4_scaler_mul(matrix4 m, float k) {
    matrix4 ret = matrix4_create(m.row1, m.row2, m.row3, m.row4);
    vector4_mult_scaler(&ret.row1, k);
    vector4_mult_scaler(&ret.row2, k);
    vector4_mult_scaler(&ret.row3, k);
    vector4_mult_scaler(&ret.row4, k);
    return ret;
}

void matrix4_mul_by_scaler(matrix4* m, float k) {
    vector4_mult_scaler(&m->row1, k);
    vector4_mult_scaler(&m->row2, k);
    vector4_mult_scaler(&m->row3, k);
    vector4_mult_scaler(&m->row4, k);
}

matrix4 matrix4_div_scaler(matrix4 m, float k) {
    matrix4 ret = matrix4_create(m.row1, m.row2, m.row3, m.row4);
    vector4_div_scaler(&ret.row1, k);
    vector4_div_scaler(&ret.row2, k);
    vector4_div_scaler(&ret.row3, k);
    vector4_div_scaler(&ret.row4, k);
    return ret;
}

void matrix4_div_by_scaler(matrix4* m, float k) {
    vector4_div_scaler(&m->row1, k);
    vector4_div_scaler(&m->row2, k);
    vector4_div_scaler(&m->row3, k);
    vector4_div_scaler(&m->row4, k);
}

matrix4 matrix4_addation(matrix4 m1, matrix4 m2) {
    matrix4 ret = matrix4_create(m1.row1, m1.row2, m1.row3, m1.row4);
    vector4_addv(&ret.row1, m2.row1);
    vector4_addv(&ret.row2, m2.row2);
    vector4_addv(&ret.row3, m2.row3);
    vector4_addv(&ret.row4, m2.row4);
    return ret;
}

void matrix4_addm(matrix4* source, matrix4 m) {
    vector4_addv(&source->row1, m.row1);
    vector4_addv(&source->row2, m.row2);
    vector4_addv(&source->row3, m.row3);
    vector4_addv(&source->row4, m.row4);
}

matrix4 matrix4_subtraction(matrix4 m1, matrix4 m2) {
    matrix4 ret = matrix4_create(m1.row1, m1.row2, m1.row3, m1.row4);
    vector4_subv(&ret.row1, m2.row1);
    vector4_subv(&ret.row2, m2.row2);
    vector4_subv(&ret.row3, m2.row3);
    vector4_subv(&ret.row4, m2.row4);
    return ret;
}

vector4 matrix4_get_column(matrix4 m, u32 index) {
    u32 index_r = index--;
    ASSERT_MSG(index_r <= VECTOR4_ELEMENT_COUNT, "Matrix column index out of bounds.");

    float* matrix_values = matrix4_value_ptr(m);
    u32 x_index = 0 + index_r;
    u32 y_index = VECTOR4_ELEMENT_COUNT + index_r;
    u32 z_index = VECTOR4_ELEMENT_COUNT * 2 + index_r;
    u32 w_index = VECTOR4_ELEMENT_COUNT * 3 + index_r;
    vector4 ret = vector4_create(matrix_values[x_index], matrix_values[y_index],
                                 matrix_values[z_index], matrix_values[w_index]);

    free(matrix_values);

    return ret;
}

vector4 matrix4_get_row(matrix4 m, u32 index) {
    u32 index_r = index--;
    ASSERT_MSG(index_r <= VECTOR4_ELEMENT_COUNT, "Matrix column index out of bounds.");

    float* matrix_values = matrix4_value_ptr(m);
    u32 x_index = 0 + (index_r * VECTOR4_ELEMENT_COUNT);
    u32 y_index = 1 + (index_r * VECTOR4_ELEMENT_COUNT);
    u32 z_index = 2 + (index_r * VECTOR4_ELEMENT_COUNT);
    u32 w_index = 3 + (index_r * VECTOR4_ELEMENT_COUNT);
    vector4 ret = vector4_create(matrix_values[x_index], matrix_values[y_index],
                                 matrix_values[z_index], matrix_values[w_index]);

    free(matrix_values);

    return ret;
}

void matrix4_subm(matrix4* source, matrix4 m) {
    vector4_subv(&source->row1, m.row1);
    vector4_subv(&source->row2, m.row2);
    vector4_subv(&source->row3, m.row3);
    vector4_subv(&source->row4, m.row4);
}

matrix4 matrix4_multiply(matrix4 m1, matrix4 m2) {
    vector4 SrcA0 = m1.row1;
    vector4 SrcA1 = m1.row2;
    vector4 SrcA2 = m1.row3;
    vector4 SrcA3 = m1.row4;

    vector4 SrcB0 = m2.row1;
    vector4 SrcB1 = m2.row2;
    vector4 SrcB2 = m2.row3;
    vector4 SrcB3 = m2.row4;

    matrix4 ret;

    ret.row1 = vector4_additition(vector4_additition(
                                      vector4_scaler_multiplication(SrcA0, SrcB0.x),
                                      vector4_scaler_multiplication(SrcA1, SrcB0.y)),
                                  vector4_additition(
                                      vector4_scaler_multiplication(SrcA2, SrcB0.z),
                                      vector4_scaler_multiplication(SrcA3, SrcB0.w)));
    ret.row2 = vector4_additition(vector4_additition(
                                      vector4_scaler_multiplication(SrcA0, SrcB1.x),
                                      vector4_scaler_multiplication(SrcA1, SrcB1.y)),
                                  vector4_additition(
                                      vector4_scaler_multiplication(SrcA2, SrcB1.z),
                                      vector4_scaler_multiplication(SrcA3, SrcB1.w)));
    ret.row3 = vector4_additition(vector4_additition(
                                      vector4_scaler_multiplication(SrcA0, SrcB2.x),
                                      vector4_scaler_multiplication(SrcA1, SrcB2.y)),
                                  vector4_additition(
                                      vector4_scaler_multiplication(SrcA2, SrcB2.z),
                                      vector4_scaler_multiplication(SrcA3, SrcB2.w)));
    ret.row4 = vector4_additition(vector4_additition(
                                      vector4_scaler_multiplication(SrcA0, SrcB3.x),
                                      vector4_scaler_multiplication(SrcA1, SrcB3.y)),
                                  vector4_additition(
                                      vector4_scaler_multiplication(SrcA2, SrcB3.z),
                                      vector4_scaler_multiplication(SrcA3, SrcB3.w)));
    return ret;
}

void matrix4_multm(matrix4* source, matrix4 m) {
    matrix4 res = matrix4_multiply(*source, m);
    source = &res;
}

matrix4 matrix4_divide(matrix4 m1, matrix4 m2) {
    matrix4 ret = matrix4_create(m1.row1, m1.row2, m1.row3, m1.row4);
    vector4_divdv(&ret.row1, m2.row1);
    vector4_divdv(&ret.row2, m2.row2);
    vector4_divdv(&ret.row3, m2.row3);
    vector4_divdv(&ret.row4, m2.row4);
    return ret;
}

void matrix4_divm(matrix4* source, matrix4 m) {
    vector4_divdv(&source->row1, m.row1);
    vector4_divdv(&source->row2, m.row2);
    vector4_divdv(&source->row3, m.row3);
    vector4_divdv(&source->row4, m.row4);
}

matrix4 matrix4_transpose(matrix4 m) {
    matrix4 ret = matrix4_create_by_value(0.0f);
    ret.row1 = matrix4_get_column(m, 1);
    ret.row2 = matrix4_get_column(m, 2);
    ret.row3 = matrix4_get_column(m, 3);
    ret.row4 = matrix4_get_column(m, 4);
    return ret;
}

void matrix4_log(matrix4 m) {
    printf("| ");
    vector4_log(m.row1);
    printf(" |\n");
    printf("| ");
    vector4_log(m.row2);
    printf(" |\n");
    printf("| ");
    vector4_log(m.row3);
    printf(" |\n");
    printf("| ");
    vector4_log(m.row4);
    printf(" |\n");
}

matrix4 matrix4_inverse(matrix4 m) {
    float Coef00 = m.row3.z * m.row4.w - m.row4.z * m.row3.w;
    float Coef02 = m.row2.z * m.row4.w - m.row4.z * m.row2.w;
    float Coef03 = m.row2.z * m.row3.w - m.row3.z * m.row2.w;

    float Coef04 = m.row3.y * m.row4.w - m.row4.y * m.row3.w;
    float Coef06 = m.row2.y * m.row4.w - m.row4.y * m.row2.w;
    float Coef07 = m.row2.y * m.row3.w - m.row3.y * m.row2.w;

    float Coef08 = m.row3.y * m.row4.z - m.row4.y * m.row3.z;
    float Coef10 = m.row2.y * m.row4.z - m.row4.y * m.row2.z;
    float Coef11 = m.row2.y * m.row3.z - m.row3.y * m.row2.z;

    float Coef12 = m.row3.x * m.row4.w - m.row4.x * m.row3.w;
    float Coef14 = m.row2.x * m.row4.w - m.row4.x * m.row2.w;
    float Coef15 = m.row2.x * m.row3.w - m.row3.x * m.row2.w;

    float Coef16 = m.row3.x * m.row4.z - m.row4.x * m.row3.z;
    float Coef18 = m.row2.x * m.row4.z - m.row4.x * m.row2.z;
    float Coef19 = m.row2.x * m.row3.z - m.row3.x * m.row2.z;

    float Coef20 = m.row3.x * m.row4.y - m.row4.x * m.row3.y;
    float Coef22 = m.row2.x * m.row4.y - m.row4.x * m.row2.y;
    float Coef23 = m.row2.x * m.row3.y - m.row3.x * m.row2.y;

    vector4 Fac0 = vector4_create(Coef00, Coef00, Coef02, Coef03);
    vector4 Fac1 = vector4_create(Coef04, Coef04, Coef06, Coef07);
    vector4 Fac2 = vector4_create(Coef08, Coef08, Coef10, Coef11);
    vector4 Fac3 = vector4_create(Coef12, Coef12, Coef14, Coef15);
    vector4 Fac4 = vector4_create(Coef16, Coef16, Coef18, Coef19);
    vector4 Fac5 = vector4_create(Coef20, Coef20, Coef22, Coef23);

    vector4 Vec0 = vector4_create(m.row2.x, m.row1.x, m.row1.x, m.row1.x);
    vector4 Vec1 = vector4_create(m.row2.y, m.row1.y, m.row1.y, m.row1.y);
    vector4 Vec2 = vector4_create(m.row2.z, m.row1.z, m.row1.z, m.row1.z);
    vector4 Vec3 = vector4_create(m.row2.w, m.row1.w, m.row1.w, m.row1.w);

    vector4 Inv0 = vector4_subtraction(
        vector4_multiply(Vec1, Fac0),
        vector4_additition(
            vector4_multiply(Vec2, Fac1),
            vector4_multiply(Vec3, Fac2)));
    vector4 Inv1 = vector4_subtraction(
        vector4_multiply(Vec0, Fac0),
        vector4_additition(
            vector4_multiply(Vec2, Fac3),
            vector4_multiply(Vec3, Fac4)));
    vector4 Inv2 = vector4_subtraction(
        vector4_multiply(Vec0, Fac1),
        vector4_additition(
            vector4_multiply(Vec1, Fac3),
            vector4_multiply(Vec3, Fac5)));
    vector4 Inv3 = vector4_subtraction(
        vector4_multiply(Vec0, Fac2),
        vector4_additition(
            vector4_multiply(Vec1, Fac4),
            vector4_multiply(Vec2, Fac5)));

    vector4 SignA = vector4_create(+1, -1, +1, -1);
    vector4 SignB = vector4_create(-1, +1, -1, +1);
    matrix4 Inverse = matrix4_create(
        vector4_multiply(Inv0, SignA),
        vector4_multiply(Inv1, SignB),
        vector4_multiply(Inv2, SignA),
        vector4_multiply(Inv3, SignB));

    vector4 Row0 = vector4_create(Inverse.row1.x, Inverse.row2.x, Inverse.row3.x, Inverse.row4.x);

    vector4 Dot0 = vector4_multiply(m.row1, Row0);
    float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

    float OneOverDeterminant = 1 / Dot1;

    return matrix4_scaler_mul(Inverse, OneOverDeterminant);
}

vector4 vector4_multiply_matrix4(matrix4 m, vector4 v) {
    vector4 Mov0 = vector4_create(v.x, v.x, v.x, v.x);
    vector4 Mov1 = vector4_create(v.y, v.y, v.y, v.y);
    vector4 Mul0 = vector4_multiply(m.row1, Mov0);
    vector4 Mul1 = vector4_multiply(m.row2, Mov1);
    vector4 Add0 = vector4_additition(Mul0, Mul1);
    vector4 Mov2 = vector4_create(v.z, v.z, v.z, v.z);
    vector4 Mov3 = vector4_create(v.w, v.w, v.w, v.w);
    vector4 Mul2 = vector4_multiply(m.row3, Mov2);
    vector4 Mul3 = vector4_multiply(m.row4, Mov3);
    vector4 Add1 = vector4_additition(Mul2, Mul3);
    vector4 Add2 = vector4_additition(Add0, Add1);
    return Add2;
}
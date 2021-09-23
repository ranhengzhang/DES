#include "STRDES.hpp"

STRDES::STRDES(uint64_t word, uint64_t key) {
    for (int i = 0; i < 64; ++i) {
        mword[63 - i] = word & 1;
        mkey[0][63 - i] = key & 1;
        word >>= 1;
        key >>= 1;
    }
    makeKey();
} // 构造函数

STRDES::~STRDES() = default; // 析构函数

__attribute__((unused)) uint64_t STRDES::encode() {
    initialPermutation();
    iterate(true);
    finalPermutation();

    return getResult();
} // 一步加密

__attribute__((unused)) uint64_t STRDES::decode() {
    initialPermutation();
    iterate(false);
    finalPermutation();

    return getResult();
} // 一步解密

void STRDES::initialPermutation() {
    bit temp[64];

    for (int i = 0; i < 64; ++i) {
        temp[i] = mword[initial[i] - 1];
    }
    for (int i = 0; i < 64; ++i) {
        mword[i] = temp[i];
    }
} // 初始IP置换

void STRDES::finalPermutation() {
    bit temp[64];

    for (int i = 0; i < 64; ++i) {
        mword[i] = i < 32 ? R[16][i] : L[16][i - 32];
    }
    for (int i = 0; i < 64; ++i) {
        temp[i] = mword[final[i] - 1];
    }
    for (int i = 0; i < 64; ++i) {
        mword[i] = temp[i];
    }
} // 逆置换

void STRDES::iterate(bool flag) {
    for (int i = 0; i < 32; ++i) {
        L[0][i] = mword[i];
        R[0][i] = mword[i + 32];
    }
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 32; ++j) {
            L[i + 1][j] = R[i][j];
        }

        bit temp[32];
        F(temp, R[i], mkey[flag ? i + 1 : 16 - i]);

        for (int j = 0; j < 32; ++j) {
            R[i + 1][j] = L[i][j] ^ temp[j];
        }
    }
} // 16次迭代

void STRDES::F(bit *source, const bit *Ri, const bit *Ki) {
    bit temp[48];

    for (int i = 0; i < 48; ++i) {
        temp[i] = Ri[i];
    }
    E(temp);
    for (int i = 0; i < 48; ++i) {
        temp[i] ^= Ki[i];
    }
    S(temp);
    P(temp);
    for (int i = 0; i < 32; ++i) {
        source[i] = temp[i];
    }
} // 轮函数F

void STRDES::E(bit *Ri) {
    bit temp[48];

    for (int i = 0; i < 48; ++i) {
        temp[i] = Ri[EBox[i] - 1];
    }
    for (int i = 0; i < 48; ++i) {
        Ri[i] = temp[i];
    }
} // E扩展运算

void STRDES::S(bit *Ri) {
    bit temp[8][6] = {0};

    for (int i = 0; i < 48; ++i) {
        temp[0][i] = Ri[i];
    }
    for (int i = 0; i < 8; ++i) {
        uint8_t n = 0;
        n = SBox[i][(temp[i][0] << 1) | temp[i][5]][(temp[i][1] << 3) | (temp[i][2] << 2) | (temp[i][3] << 1) |
                                                    temp[i][4]];
        for (int j = 0; j < 4; ++j) {
            temp[i][j] = (n >> (3 - j)) & 1;
        }
    }
    for (int i = 0; i < 32; ++i) {
        Ri[i] = temp[i / 4][i % 4];
    }
} // S盒压缩

void STRDES::P(bit *Ri) {
    bit temp[32];

    for (int i = 0; i < 32; ++i) {
        temp[i] = Ri[i];
    }
    for (int i = 0; i < 32; ++i) {
        Ri[i] = temp[PBox[i] - 1];
    }
} // P盒置换

void STRDES::makeKey() {
    bit temp[56] = {0}, C[29] = {0}, D[29] = {0};

    for (int i = 0; i < 56; ++i) {
        temp[i] = mkey[0][PC1[i] - 1];
    }
    for (int i = 0; i < 28; ++i) {
        C[i] = temp[i];
        D[i] = temp[i + 28];
    }
    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < shiftTimes[i]; ++j) {
            C[28] = C[0];
            D[28] = D[0];
            for (int k = 0; k < 28; ++k) {
                C[k] = C[k + 1];
                D[k] = D[k + 1];
            }
        }
        for (int j = 0; j < 28; ++j) {
            temp[j] = C[j];
            temp[j + 28] = D[j];
        }
        for (int j = 0; j < 48; ++j) {
            mkey[i + 1][j] = temp[PC2[j] - 1];
        }
    }
} // 密钥生成

__attribute__((unused)) uint64_t STRDES::getResult() const {
    uint64_t temp = 0;

    for (bit i : mword) {
        temp <<= 1;
        temp |= (uint64_t) i;
    }

    return temp;
} // 获取运算结果

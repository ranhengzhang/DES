#include "BITDES1.hpp"

BITDES1::BITDES1(uint64_t word, uint64_t key) : mword(word) {
    mkey[0] = key;
    makeKey();
} // 构造函数

BITDES1::~BITDES1() = default; // 析构函数

__attribute__((unused)) uint64_t BITDES1::encode() {
    initialPermutation();
    iterate(true);
    finalPermutation();

    return getResult();
} // 一步加密

__attribute__((unused)) uint64_t BITDES1::decode() {
    initialPermutation();
    iterate(false);
    finalPermutation();

    return getResult();
} // 一步解密

void BITDES1::initialPermutation() {
    uint64_t temp = 0;

    for (uint8_t i : initial) {
        temp <<= 1;
        temp |= (mword >> (64 - i)) & 0x01;
    }
    mword = temp;
} // 初始IP置换

void BITDES1::finalPermutation() {
    uint64_t temp = 0;

    temp = ((uint64_t)R(16) << 32) | L(16);
    for (uint8_t i : final) {
        mword <<= 1;
        mword |= (temp >> (64 - i)) & 0x01;
    }
} // 逆置换

void BITDES1::iterate(bool flag) {
    R(0) = mword & 0xFFFFFFFF;
    L(0) = mword >> 32;
    for (int i = 0; i < 16; ++i) {
        L(i + 1) = R(i);
        R(i + 1) = L(i) ^ F(R(i), mkey[flag ? i + 1 : 16 - i]);
    }
} // 16次迭代

uint32_t BITDES1::F(uint32_t Ri, uint64_t Ki) {
    return P(S(E(Ri) ^ Ki));
} // 轮函数F

uint64_t BITDES1::E(uint32_t Ri) {
    uint64_t temp = 0;

    for (uint8_t i : EBox) {
        temp <<= 1;
        temp |= (((uint64_t)Ri) >> (32 - i)) & 0x01;
    }

    return temp;
} // E扩展运算

uint32_t BITDES1::S(uint64_t Ri) {
    uint32_t temp1 = 0;
    uint8_t temp2[8]{};

    for (int i = 0; i < 8; ++i) {
        temp2[7 - i] = Ri & 0x3F;
        Ri >>= 6;
    }
    for (int i = 0; i < 8; ++i) {
        temp2[i] = SBox[i][((temp2[i] >> 4) & 0x02) | (temp2[i] & 0x01)][(temp2[i] >> 1) & 0x0F];
    }
    for (uint8_t i : temp2) {
        temp1 <<= 4;
        temp1 |= i;
    }

    return temp1;
} // S盒压缩

uint32_t BITDES1::P(uint32_t Ri) {
    uint32_t temp = 0;

    for (uint8_t i : PBox) {
        temp <<= 1;
        temp |= (Ri >> (32 - i)) & 0x01;
    }

    return temp;
} // P盒压缩

void BITDES1::makeKey() {
    uint64_t temp = 0;
    uint32_t C = 0, D = 0;

    for (uint8_t i : PC1) {
        temp <<= 1;
        temp |= ((mkey[0]) >> (64 - i)) & 0x01;
    }
    C = temp >> 28;
    D = temp & 0x0FFFFFFF;
    for (int i = 0; i < 16; ++i) {
        MOV28(C, shiftTimes[i]);
        MOV28(D, shiftTimes[i]);
        temp = (((uint64_t)C) << 28) | D;
        for (uint8_t j : PC2) {
            mkey[i + 1] <<= 1;
            mkey[i + 1] |= (temp >> (56 - j)) & 0x01;
        }
    }
} // 密钥生成

__attribute__((unused)) uint64_t BITDES1::getResult() const {
    return mword;
} // 获取运算结果

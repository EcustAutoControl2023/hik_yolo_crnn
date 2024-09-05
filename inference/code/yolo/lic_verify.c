#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lic_verify.h"
#include "demo_comm.h"
#include <opdevsdk_common_basic.h>

void demo_syslog_print(char *format, ...);
/**
* @brief demo common function
*/
#define DEMOPRT 									 demo_syslog_print


void MD5Init(MD5_CTX *context) {
    context->count[0] = context->count[1] = 0;
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen) {
    unsigned int i, index, partLen;

    index = (unsigned int)((context->count[0] >> 3) & 0x3F);
    if ((context->count[0] += ((uint32_t)inputLen << 3)) < ((uint32_t)inputLen << 3))
        context->count[1]++;
    context->count[1] += ((uint32_t)inputLen >> 29);
    partLen = 64 - index;

    if (inputLen >= partLen) {
        memcpy(&context->buffer[index], input, partLen);
        MD5Transform(context->state, context->buffer);

        for (i = partLen; i + 63 < inputLen; i += 64)
            MD5Transform(context->state, &input[i]);

        index = 0;
    } else {
        i = 0;
    }

    memcpy(&context->buffer[index], &input[i], inputLen - i);
}

void MD5Final(unsigned char digest[16], MD5_CTX *context) {
    unsigned char bits[8];
    unsigned int index, padLen;

    MD5Encode(bits, context->count, 8);

    index = (unsigned int)((context->count[0] >> 3) & 0x3F);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    MD5Update(context, PADDING, padLen);
    MD5Update(context, bits, 8);

    MD5Encode(digest, context->state, 16);

    memset(context, 0, sizeof(*context));
}

void MD5Encode(unsigned char *output, uint32_t *input, unsigned int len) {
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4) {
        output[j] = (unsigned char)(input[i] & 0xFF);
        output[j+1] = (unsigned char)((input[i] >> 8) & 0xFF);
        output[j+2] = (unsigned char)((input[i] >> 16) & 0xFF);
        output[j+3] = (unsigned char)((input[i] >> 24) & 0xFF);
    }
}

void MD5Decode(uint32_t *output, unsigned char *input, unsigned int len) {
    unsigned int i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
        output[i] = ((uint32_t)input[j]) | (((uint32_t)input[j+1]) << 8) |
                    (((uint32_t)input[j+2]) << 16) | (((uint32_t)input[j+3]) << 24);
}

void MD5Transform(uint32_t state[4], unsigned char block[64]) {
    uint32_t a = state[0], b = state[1], c = state[2], d = state[3], x[16];

    MD5Decode(x, block, 64);

    // Round 1
    #define S11 7
    #define S12 12
    #define S13 17
    #define S14 22
    #define F(x, y, z) ((x & y) | (~x & z))
    #define FF(a, b, c, d, x, s, ac) { a += F(b,c,d) + x + ac; a = ((a << s) | (a >> (32-s))); a += b; }
    
    FF(a, b, c, d, x[ 0], S11, 0xd76aa478); // 1
    FF(d, a, b, c, x[ 1], S12, 0xe8c7b756); // 2
    FF(c, d, a, b, x[ 2], S13, 0x242070db); // 3
    FF(b, c, d, a, x[ 3], S14, 0xc1bdceee); // 4
    FF(a, b, c, d, x[ 4], S11, 0xf57c0faf); // 5
    FF(d, a, b, c, x[ 5], S12, 0x4787c62a); // 6
    FF(c, d, a, b, x[ 6], S13, 0xa8304613); // 7
    FF(b, c, d, a, x[ 7], S14, 0xfd469501); // 8
    FF(a, b, c, d, x[ 8], S11, 0x698098d8); // 9
    FF(d, a, b, c, x[ 9], S12, 0x8b44f7af); // 10
    FF(c, d, a, b, x[10], S13, 0xffff5bb1); // 11
    FF(b, c, d, a, x[11], S14, 0x895cd7be); // 12
    FF(a, b, c, d, x[12], S11, 0x6b901122); // 13
    FF(d, a, b, c, x[13], S12, 0xfd987193); // 14
    FF(c, d, a, b, x[14], S13, 0xa679438e); // 15
    FF(b, c, d, a, x[15], S14, 0x49b40821); // 16

    // Round 2
    #define S21 5
    #define S22 9
    #define S23 14
    #define S24 20
    #define G(x, y, z) ((x & z) | (y & ~z))
    #define GG(a, b, c, d, x, s, ac) { a += G(b,c,d) + x + ac; a = ((a << s) | (a >> (32-s))); a += b; }
    
    GG(a, b, c, d, x[ 1], S21, 0xf61e2562); // 17
    GG(d, a, b, c, x[ 6], S22, 0xc040b340); // 18
    GG(c, d, a, b, x[11], S23, 0x265e5a51); // 19
    GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa); // 20
    GG(a, b, c, d, x[ 5], S21, 0xd62f105d); // 21
    GG(d, a, b, c, x[10], S22,  0x2441453); // 22
    GG(c, d, a, b, x[15], S23, 0xd8a1e681); // 23
    GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8); // 24
    GG(a, b, c, d, x[ 9], S21, 0x21e1cde6); // 25
    GG(d, a, b, c, x[14], S22, 0xc33707d6); // 26
    GG(c, d, a, b, x[ 3], S23, 0xf4d50d87); // 27
    GG(b, c, d, a, x[ 8], S24, 0x455a14ed); // 28
    GG(a, b, c, d, x[13], S21, 0xa9e3e905); // 29
    GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8); // 30
    GG(c, d, a, b, x[ 7], S23, 0x676f02d9); // 31
    GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); // 32

    // Round 3
    #define S31 4
    #define S32 11
    #define S33 16
    #define S34 23
    #define H(x, y, z) (x ^ y ^ z)
    #define HH(a, b, c, d, x, s, ac) { a += H(b,c,d) + x + ac; a = ((a << s) | (a >> (32-s))); a += b; }
    
    HH(a, b, c, d, x[ 5], S31, 0xfffa3942); // 33
    HH(d, a, b, c, x[ 8], S32, 0x8771f681); // 34
    HH(c, d, a, b, x[11], S33, 0x6d9d6122); // 35
    HH(b, c, d, a, x[14], S34, 0xfde5380c); // 36
    HH(a, b, c, d, x[ 1], S31, 0xa4beea44); // 37
    HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9); // 38
    HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60); // 39
    HH(b, c, d, a, x[10], S34, 0xbebfbc70); // 40
    HH(a, b, c, d, x[13], S31, 0x289b7ec6); // 41
    HH(d, a, b, c, x[ 0], S32, 0xeaa127fa); // 42
    HH(c, d, a, b, x[ 3], S33, 0xd4ef3085); // 43
    HH(b, c, d, a, x[ 6], S34,  0x4881d05); // 44
    HH(a, b, c, d, x[ 9], S31, 0xd9d4d039); // 45
    HH(d, a, b, c, x[12], S32, 0xe6db99e5); // 46
    HH(c, d, a, b, x[15], S33, 0x1fa27cf8); // 47
    HH(b, c, d, a, x[ 2], S34, 0xc4ac5665); // 48

    // Round 4
    #define S41 6
    #define S42 10
    #define S43 15
    #define S44 21
    #define I(x, y, z) (y ^ (x | ~z))
    #define II(a, b, c, d, x, s, ac) { a += I(b,c,d) + x + ac; a = ((a << s) | (a >> (32-s))); a += b; }
    
    II(a, b, c, d, x[ 0], S41, 0xf4292244); // 49
    II(d, a, b, c, x[ 7], S42, 0x432aff97); // 50
    II(c, d, a, b, x[14], S43, 0xab9423a7); // 51
    II(b, c, d, a, x[ 5], S44, 0xfc93a039); // 52
    II(a, b, c, d, x[12], S41, 0x655b59c3); // 53
    II(d, a, b, c, x[ 3], S42, 0x8f0ccc92); // 54
    II(c, d, a, b, x[10], S43, 0xffeff47d); // 55
    II(b, c, d, a, x[ 1], S44, 0x85845dd1); // 56
    II(a, b, c, d, x[ 8], S41, 0x6fa87e4f); // 57
    II(d, a, b, c, x[15], S42, 0xfe2ce6e0); // 58
    II(c, d, a, b, x[ 6], S43, 0xa3014314); // 59
    II(b, c, d, a, x[13], S44, 0x4e0811a1); // 60
    II(a, b, c, d, x[ 4], S41, 0xf7537e82); // 61
    II(d, a, b, c, x[11], S42, 0xbd3af235); // 62
    II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb); // 63
    II(b, c, d, a, x[ 9], S44, 0xeb86d391); // 64

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    memset(x, 0, sizeof(x));
}

void _get_mac(unsigned char* mac_addr)
{
    OP_DEVSDK_DEVICE_INFO* p_device_info;
    p_device_info = (OP_DEVSDK_DEVICE_INFO*)malloc(sizeof(OP_DEVSDK_DEVICE_INFO));
    opdevsdk_get_device_info(p_device_info);
    sprintf((char *)mac_addr, "%s", p_device_info->mac_addr);
    DEMOPRT((char *)"mac_addr: %s\n", mac_addr);
    free(p_device_info);
}

void _mac_to_zcm(unsigned char *str, unsigned char *output) {
    MD5_CTX context;
    unsigned char digest[16];
    int i;

    MD5Init(&context);
    MD5Update(&context, str, strlen((char *)str));
    MD5Final(digest, &context);

    for (i = 0; i < 16; i++) {
        sprintf((char *)&output[i * 2], "%02x", digest[i]);
    }
}

void generate_license(char *license_file)
{
  // 生成要加密的字符串，可以是 MAC 地址或其他信息
  unsigned char input[18]; // 假设MAC地址长度为17+1('\0')
  unsigned char output[33];
  _get_mac(input);

  // 调用 md5 函数生成 MD5 加密字符串
  _mac_to_zcm(input, output);

  // 打开文件以写入
  FILE *file = fopen(license_file, "w");
  if (file == NULL) {
      perror("Failed to open file");
      return;
  }

  // 将 MD5 加密字符串写入文件
  fprintf(file, "%s\n", output);

  // 关闭文件
  fclose(file);
  printf("License key written to %s\n", license_file);
}

int verify_license(char *license_file)
{
 unsigned char expected_mac[33];
    unsigned char current_mac[33];
    unsigned char mac_address[18]; // 假设MAC地址长度为17+1('\0')

    // 获取当前机器的MAC地址
    _get_mac(mac_address);

    // 使用 md5 函数生成当前 MAC 地址的 MD5
    _mac_to_zcm(mac_address, current_mac);

    // 打开许可证文件以读取
    FILE *file = fopen(license_file, "r");
    if (file == NULL) {
        perror("Failed to open license file");
        return 0; // 文件打开失败，返回非法
    }

    // 从文件中读取 MD5 字符串
    if (fgets((char *)expected_mac, sizeof(expected_mac), file) == NULL) {
        fclose(file);
        perror("Failed to read from license file");
        return 0; // 读取失败，返回非法
    }

    // 去掉可能存在的换行符
    expected_mac[strcspn((char *)expected_mac, "\n")] = 0;

    // 关闭文件
    fclose(file);

    // 比较当前生成的 MAC 地址 MD5 和文件中的 MD5 是否匹配
    if (strcmp((char *)current_mac, (char *)expected_mac) == 0) {
        return 1; // 匹配，返回合法
    } else {
        return 0; // 不匹配，返回非法
    }
}

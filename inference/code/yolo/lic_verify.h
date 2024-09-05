#include <stdint.h>


typedef struct {
    uint32_t state[4];
    uint32_t count[2];
    unsigned char buffer[64];
} MD5_CTX;

static unsigned char PADDING[64] = {
    0x80
};


void MD5Init(MD5_CTX *context);
void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen);
void MD5Final(unsigned char digest[16], MD5_CTX *context);
void MD5Encode(unsigned char *output, uint32_t *input, unsigned int len);
void MD5Decode(uint32_t *output, unsigned char *input, unsigned int len);
void MD5Transform(uint32_t state[4], unsigned char block[64]);

// 拿到设备的mac地址
// mac_addr: 用于存储mac地址的数组
// 返回值: 无
void _get_mac(unsigned char* mac_addr);

// mac地址转换为zcm，加密
// mac_addr: mac地址
// zcm: 用于存储zcm的数组
// 返回值: 无
void _mac_to_zcm(unsigned char* mac_addr, unsigned char* zcm);

// 生成license文件
// license_file: 生成的license文件名
void generate_license(char* license_file);

// 验证license文件
// license_file: 验证的license文件名
int verify_license(char* license_file);

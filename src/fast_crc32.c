// SPDX-License-Identifier: BSD-2-Clause

#include "fast_crc32.h"
#include <stdlib.h>
#include <stdio.h>

#define CRC_POLYNOMIAL     (0x04C11DB7)
#define CRC_POLYNOMIAL_REF (0xEDB88320)
#define CRC32_BITS         (32)

static const uint32_t crc32a_table[256] = {
    0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,  //   0 [0x00 .. 0x07]
    0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61, 0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,  //   8 [0x08 .. 0x0F]
    0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9, 0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,  //  16 [0x10 .. 0x17]
    0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011, 0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,  //  24 [0x18 .. 0x1F]
    0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039, 0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,  //  32 [0x20 .. 0x27]
    0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81, 0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,  //  40 [0x28 .. 0x2F]
    0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49, 0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,  //  48 [0x30 .. 0x37]
    0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1, 0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,  //  56 [0x38 .. 0x3F]
    0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE, 0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,  //  64 [0x40 .. 0x47]
    0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16, 0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,  //  72 [0x48 .. 0x4F]
    0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE, 0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,  //  80 [0x50 .. 0x57]
    0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066, 0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,  //  88 [0x58 .. 0x5F]
    0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E, 0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,  //  96 [0x60 .. 0x67]
    0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6, 0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,  // 104 [0x68 .. 0x6F]
    0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E, 0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,  // 112 [0x70 .. 0x77]
    0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686, 0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,  // 120 [0x78 .. 0x7F]
    0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637, 0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,  // 128 [0x80 .. 0x87]
    0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F, 0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,  // 136 [0x88 .. 0x8F]
    0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47, 0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,  // 144 [0x90 .. 0x97]
    0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF, 0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,  // 152 [0x98 .. 0x9F]
    0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7, 0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,  // 160 [0xA0 .. 0xA7]
    0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F, 0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,  // 168 [0xA8 .. 0xAF]
    0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7, 0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,  // 176 [0xB0 .. 0xB7]
    0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F, 0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,  // 184 [0xB8 .. 0xBF]
    0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640, 0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,  // 192 [0xC0 .. 0xC7]
    0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8, 0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,  // 200 [0xC8 .. 0xCF]
    0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30, 0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,  // 208 [0xD0 .. 0xD7]
    0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088, 0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,  // 216 [0xD8 .. 0xDF]
    0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0, 0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,  // 224 [0xE0 .. 0xE7]
    0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18, 0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,  // 232 [0xE8 .. 0xEF]
    0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0, 0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,  // 240 [0xF0 .. 0xF7]
    0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668, 0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4,  // 248 [0xF8 .. 0xFF]
};

static const uint32_t crc32b_table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,  //   0 [0x00 .. 0x07]
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,  //   8 [0x08 .. 0x0F]
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,  //  16 [0x10 .. 0x17]
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,  //  24 [0x18 .. 0x1F]
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,  //  32 [0x20 .. 0x27]
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,  //  40 [0x28 .. 0x2F]
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,  //  48 [0x30 .. 0x37]
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,  //  56 [0x38 .. 0x3F]
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,  //  64 [0x40 .. 0x47]
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,  //  72 [0x48 .. 0x4F]
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,  //  80 [0x50 .. 0x57]
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,  //  88 [0x58 .. 0x5F]
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,  //  96 [0x60 .. 0x67]
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,  // 104 [0x68 .. 0x6F]
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,  // 112 [0x70 .. 0x77]
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,  // 120 [0x78 .. 0x7F]
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,  // 128 [0x80 .. 0x87]
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,  // 136 [0x88 .. 0x8F]
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,  // 144 [0x90 .. 0x97]
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,  // 152 [0x98 .. 0x9F]
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,  // 160 [0xA0 .. 0xA7]
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,  // 168 [0xA8 .. 0xAF]
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,  // 176 [0xB0 .. 0xB7]
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,  // 184 [0xB8 .. 0xBF]
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,  // 192 [0xC0 .. 0xC7]
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,  // 200 [0xC8 .. 0xCF]
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,  // 208 [0xD0 .. 0xD7]
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,  // 216 [0xD8 .. 0xDF]
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,  // 224 [0xE0 .. 0xE7]
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,  // 232 [0xE8 .. 0xEF]
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,  // 240 [0xF0 .. 0xF7]
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,  // 248 [0xF8 .. 0xFF]
};

uint32_t crc32a_bitwise(uint8_t* buff, uint16_t len)
{
    const uint8_t s = 8;
    const uint8_t hs = CRC32_BITS - 8;
    uint32_t crc = -1;

    for(uint16_t i = 0; i < len; i++)
    {
        crc = crc ^ ((uint32_t)buff[i] << hs);

        for(uint8_t j = 0; j < s; j++) {
            if(0 != (0x80000000 & crc))
                crc = (crc << 1) ^ CRC_POLYNOMIAL;
            else
                crc = crc << 1;
        }
    }

    return ~crc;
}

uint32_t crc32b_bitwise(uint8_t* buff, uint16_t len)
{
    const uint8_t s = 8;
    uint32_t crc = -1;

    for(uint16_t i = 0; i < len; i++) {
        crc = crc ^ buff[i];

        for(uint16_t j = 0; j < s; j++) {
            if(0 != (0x00000001 & crc))
                crc = (crc >> 1) ^ CRC_POLYNOMIAL_REF;
            else
                crc = crc >> 1;
        }
    }
    return ~crc;
}

uint32_t crc32a_lookup_table(uint8_t* buff, uint32_t len)
{
    const uint8_t s = 8;
    const uint8_t hs = CRC32_BITS - 8;
    uint32_t crc = -1;

    for(uint32_t i = 0; i < len; i++) {
        crc = crc ^ buff[i] << hs;
        crc = (crc << s) ^ crc32a_table[(crc >> hs) & 0xFF];
    }
    return ~crc;
}

uint32_t crc32b_lookup_table(uint8_t* buff, uint16_t len)
{
    const uint8_t s = 8;
    uint32_t crc = -1;

    for(uint16_t i = 0; i < len; i++) {
        crc = crc ^ buff[i];
        crc = crc32b_table[crc & 0xFF] ^ (crc >> s);
    }
    return ~crc;
}

uint32_t crc32a_fast_bytewise(uint8_t* buff, uint32_t len)
{
    const uint8_t s = 8;
    const uint8_t hs = CRC32_BITS - s;
    uint32_t crc = -1;

    for(uint32_t i = 0; i < len; i++)
    {
        uint32_t temp = buff[i] ^ (crc >> hs);

        uint32_t temp26 = temp << hs;
        for(uint16_t j = 0; j < 2; j++) {
            if (0 != (temp26 & 0x80000000))
                temp26 = (temp26 << 1) ^ CRC_POLYNOMIAL;
            else
                temp26 <<= 1;
        }

        temp = temp26 ^ (temp << 23) ^ (temp << 22) ^ (temp << 16) ^
               (temp << 12) ^ (temp << 11) ^ (temp << 10) ^
               (temp << 8) ^ (temp << 7) ^ (temp << 5) ^
               (temp << 4) ^ (temp << 2) ^ (temp << 1) ^
               temp;

        /* Current CRC */
        crc = temp ^ (crc << s);
    }

    return ~crc;
}

uint32_t crc32b_fast_bytewise(uint8_t* buff, uint32_t len)
{
    const uint8_t s = 8;
    uint32_t crc = -1;

    for(uint32_t i = 0; i < len; i++) {
        uint32_t temp = buff[i] ^ (crc & 0xFF);

        uint32_t temp8 = temp;
        for(uint16_t j = 0; j < 2; j++) {
            if (0 != (temp8 & 0x00000001))
                temp8 = (temp8 >> 1) ^ CRC_POLYNOMIAL_REF;
            else
                temp8 >>= 1;
        }

        temp = temp8 ^ (temp << 24) ^ (temp << 23) ^ (temp << 22) ^
        (temp << 20) ^ (temp << 19) ^ (temp << 17) ^ (temp << 16) ^
        (temp << 14) ^ (temp << 13) ^ (temp << 12) ^ (temp << 8) ^
        (temp << 2) ^ (temp << 1);

        /* Current CRC */
        crc = temp ^ (crc >> s);
    }

    return ~crc;
}

uint32_t crc32a_fast_nibblewise(uint8_t* buff, uint32_t len)
{
    const uint8_t s = 4;
    const uint8_t hs = CRC32_BITS - s;
    uint32_t crc = -1;

    for(uint32_t i = 0; i < len; i++) {
        uint32_t temp = (buff[i]>> s) ^ (crc >> hs);
        temp = (temp << 26) ^ (temp << 23) ^ (temp << 22) ^ (temp << 16) ^
               (temp << 12) ^ (temp << 11) ^ (temp << 10) ^ (temp << 8) ^
               (temp << 7) ^ (temp << 5) ^ (temp << 4) ^ (temp << 2) ^
               (temp << 1) ^ temp;
        crc = temp ^ (crc << s);
        temp = (buff[i]& 0x0F) ^ (crc >> hs);

        temp = (temp << 26) ^ (temp << 23) ^ (temp << 22) ^ (temp << 16) ^
               (temp << 12) ^ (temp << 11) ^ (temp << 10) ^ (temp << 8) ^
               (temp << 7) ^ (temp << 5) ^ (temp << 4) ^ (temp << 2) ^
               (temp << 1) ^ temp;

        crc = temp ^ (crc << s);
    }

    return ~crc;
}

uint32_t crc32b_fast_nibblewise(uint8_t* buff, uint32_t len)
{
    const uint8_t s = 4;
    const uint8_t hs = CRC32_BITS - s;
    uint32_t crc = -1;

    for(uint32_t i = 0; i < len; i++) {
        uint32_t temp = ((buff[i]) ^ (crc & 0x0F)) << hs;
        temp = (temp >> 26) ^ (temp >> 23) ^ (temp >> 22) ^ (temp >> 16) ^
               (temp >> 12) ^ (temp >> 11) ^ (temp >> 10) ^ (temp >> 8) ^
               (temp >> 7) ^ (temp >> 5) ^ (temp >> 4) ^ (temp >> 2) ^
               (temp >> 1) ^ temp;
        crc = temp ^ (crc >> s);

        temp = ((buff[i] >> 4) ^ (crc & 0x0F)) << hs;
        temp = (temp >> 26) ^ (temp >> 23) ^ (temp >> 22) ^ (temp >> 16) ^
               (temp >> 12) ^ (temp >> 11) ^ (temp >> 10) ^ (temp >> 8) ^
               (temp >> 7) ^ (temp >> 5) ^ (temp >> 4) ^ (temp >> 2) ^
               (temp >> 1) ^ temp;

        crc = temp ^ (crc >> s);
    }

    return ~crc;
}

void crc32a_table_init(uint32_t *table)
{
    for(uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for(uint8_t j = 0; j < 8; j++) {
            if(0 != (crc & 0x80000000))
                crc = (crc << 1 ) ^ CRC_POLYNOMIAL;
            else
                crc = crc << 1;
        }
    }
}

void crc32b_table_init(uint32_t *table)
{
    for(uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for(uint8_t j = 0; j < 8; j++) {
            if(0 != (crc & 0x00000001))
                crc = (crc >> 1 ) ^ CRC_POLYNOMIAL_REF;
            else
                crc = crc >> 1;
        }
    }
}

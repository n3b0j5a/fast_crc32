#include "../src/fast_crc32.h"
#include <criterion/criterion.h>

uint8_t test_array_byte[9]=
{
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39
};

Test(fast_crc32, crc32a_bitwise)
{
    cr_expect(0xfc891918 == crc32a_bitwise(test_array_byte, sizeof(test_array_byte)));
}

Test(fast_crc32, crc32a_lookup_table)
{
    cr_expect(0xfc891918 == crc32a_lookup_table(test_array_byte, sizeof(test_array_byte)));
}

Test(fast_crc32, crc32a_fast_bytewise)
{
    cr_expect(0xfc891918 == crc32a_fast_bytewise(test_array_byte, sizeof(test_array_byte)));
}

Test(fast_crc32, crc32a_fast_nibblewise)
{
    cr_expect(0xfc891918 == crc32a_fast_nibblewise(test_array_byte, sizeof(test_array_byte)));
}

Test(fast_crc32, crc32b_bitwise)
{
    cr_expect(0xcbf43926 == crc32b_bitwise(test_array_byte, sizeof(test_array_byte)));
}

Test(fast_crc32, crc32b_lookup_table)
{
    cr_expect(0xcbf43926 == crc32b_lookup_table(test_array_byte, sizeof(test_array_byte)));
}

Test(fast_crc32, crc32b_fast_bytewise)
{
    cr_expect(0xcbf43926 == crc32b_fast_bytewise(test_array_byte, sizeof(test_array_byte)));
}

Test(fast_crc32, crc32b_fast_nibblewise)
{
    cr_expect(0xcbf43926 == crc32b_fast_nibblewise(test_array_byte, sizeof(test_array_byte)));
}

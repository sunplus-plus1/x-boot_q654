/******************************************************************************
*                          Include File
*******************************************************************************/
#include <common.h>
#include <bootmain.h>
#include <otp/sp_otp.h>

#ifdef CONFIG_SECURE_BOOT_SIGN

// u-boot.img =|header+data+sbinf0|
#define SB_INFO_SIZE	200

#define __ALIGN4       __attribute__((aligned(4)))
#define __ALIGN8       __attribute__((aligned(8)))

void prn_dump(const char *title, const unsigned char *buf, int len)
{
	int i;

	if (title) {
		prn_string(title);
		prn_string(" "); prn_dword((u32)buf);
	}

	for (i = 0; i < len; i++) {
		if (i && !(i & 0xf)) {
			prn_string("\n");
		}
		prn_byte(buf[i]);
	}
	prn_string("\n");
}

#include "verify_image_sp7350_hw.c"  //by hw
//#include "verify_image_sp7350.c"   //by software

int xboot_verify_next_image(const struct image_header  *hdr)
{
	int ret;

	ret = sp7350_image_verify_decrypt(hdr);
	if (ret) {
		prn_string(image_get_name(hdr));
		prn_string(" verify fail !!\nhalt!");
	}
	return ret;
}
#endif

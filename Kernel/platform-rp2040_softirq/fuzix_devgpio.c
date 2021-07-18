#include "platform.h"

// TODO gpio device
int gpio_ioctl(uarg_t request, char *data) {
    if (valaddr((unsigned char *)data, 2))
		switch (request){
//			case IOCTL:
//				return 0;
//			break;
			default:
			break;
		}
	udata.u_error = EINVAL;
	return -1;
}

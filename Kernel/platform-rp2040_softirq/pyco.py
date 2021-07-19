import sys, os, usb.core
from usb.backend import libusb0
from typing_extensions import ParamSpecKwargs

# make it global to clean on CTRL-C
pico = None

class PicoUSB:
    # device id and conf
    VID=0x2e8a
    PID=0x0e1e
    CONF=1
    INTF=0
    SETT=0
    EP_OUT=0
    EP_IN=1
    # usb objects
    device = None
    ep_in = None
    ep_out = None
    def __init__(self):
        # 1. Device
        be = libusb0.get_backend()
        self.device = usb.core.find(backend=be, idVendor=self.VID, idProduct=self.PID)
        if self.device is None:
            print("Is the Pico connected and turned on?")
            sys.exit(1)
        # 2. Configuration
        self.device.set_configuration(self.CONF)
        configuration = self.device.get_active_configuration()
        # 3. Interface
        # 4. Alternate setting
        interface = configuration[(self.INTF, self.SETT)]
        # 5. Endpoints
        self.ep_in = interface[self.EP_IN]
        self.ep_out = interface[self.EP_OUT]
        # done, tell to user
        print("Device found, waiting for device connection...")
    def bulk_read(self, size_or_buffer, timeout = None):
        try:
            return self.ep_in.read(size_or_buffer, timeout)
        except usb.core.USBError as e:
            pass
    def bulk_write(self, data, timeout):
        self.ep_out.write(data, timeout)
    def clean(self):
        pass

class PicoBlockdev:
    BLOCKSIZE = 512
    filename = None
    dev = None
    def __init__(self, filename):
        self.filename = filename
    def open(self):
        self.dev = open(self.filename, "rb+")
    def lba(self):
        return os.stat(self.filename).st_size / self.BLOCKSIZE
    def read(self, blocknum):
        self.dev.seek(blocknum*self.BLOCKSIZE, 0)
        return self.dev.read(512)
    def write(self, blocknum, data):
        self.dev.seek(blocknum*self.BLOCKSIZE, 0)
        self.dev.write()
    def close(self):
        self.dev.close()

class PicoChardev:
    pass

class Pyco:
    usb = PicoUSB()
    disk1 = PicoBlockdev("root.img")
    disk2 = PicoBlockdev("swap.img")
    disk3 = PicoBlockdev("scratch.img")
    core1 = PicoChardev()
    tty1 = PicoChardev()
    tty2 = PicoChardev()
    tty3 = PicoChardev()
    def __init__(self):
        pass
    def close(self):
        self.disk1.close()
        self.disk2.close()
        self.disk3.close()
        self.usb.clean()
    def packet_rx(self, len):
        packet_id = self.usb.bulk_read(1)
        packet_data = self.usb.bulk_read(len)
        self.USB_PACKET_ID[packet_id](packet_data, len)
    def packet_gen(self, packet_id, op_id, data, len):
        pkt = bytearray(packet_id, op_id)
        for i in range(len):
            pkt += data[i]
        return pkt
    def packet_ctrl(self, data, len):
        print("packet control")
        command = data.pop(1)
        self.USB_CTRL_ID[command](data, len-1)
    def packet_disk1(self, data, len):
        print("packet disk1")
        pkt = self.packet_gen(1, 0, 0, 1)
        command = data.pop(1)
        if (command == 0): # control
            pass
        elif (command == 1):
            pkt = self.packet_gen(1, 1, self.disk1.lba(), 4)
        elif (command == 2):
            pkt = self.packet_gen(1, 2, self.disk1.read(), 512)
        elif (command == 3):
            pkt = self.packet_gen(1, 3, self.disk1.write(), 1)
        usb.write(pkt)
    def packet_disk2(self, data, len):
        print("packet disk2")
        pkt = self.packet_gen(2, 0, 0, 1)
        command = data.pop(1)
        if (command == 0): # control
            pass
        elif (command == 1):
            pkt = self.packet_gen(2, 1, self.disk1.lba(), 4)
        elif (command == 2):
            pkt = self.packet_gen(2, 2, self.disk1.read(), 512)
        elif (command == 3):
            pkt = self.packet_gen(2, 3, self.disk1.write(), 1)
        usb.write(pkt)
    def packet_disk3(self, data, len):
        print("packet disk3")
        pkt = self.packet_gen(4, 0, 0, 1)
        command = data.pop(1)
        if (command == 0): # control
            pass
        elif (command == 1):
            pkt = self.packet_gen(4, 1, self.disk1.lba(), 4)
        elif (command == 2):
            pkt = self.packet_gen(4, 2, self.disk1.read(), 512)
        elif (command == 3):
            pkt = self.packet_gen(4, 3, self.disk1.write(), 1)
        usb.write(pkt)
    def packet_core1(self, data, len):
        print("packet core1")
        pkt = self.packet_gen(8, 0, 0, 1)
        command = data.pop(1)
        if (command == 0): # control
            pass
        elif (command == 1):
            pkt = self.packet_gen(8, 1, 0, 1)
        elif (command == 2):
            pkt = self.packet_gen(8, 2, 0, 1)
        usb.write(pkt)
    def packet_tty1(self, data, len):
        print("packet tty1")
        pkt = self.packet_gen(16, 0, 0, 1)
        command = data.pop(1)
        if (command == 0): # control
            pass
        elif (command == 1):
            pkt = self.packet_gen(16, 1, 0, 1)
        elif (command == 2):
            pkt = self.packet_gen(16, 2, 0, 1)
        usb.write(pkt)
    def packet_tty2(self, data, len):
        print("packet tty2")
        pkt = self.packet_gen(32, 0, 0, 1)
        command = data.pop(1)
        if (command == 0): # control
            pass
        elif (command == 1):
            pkt = self.packet_gen(32, 1, 0, 1)
        elif (command == 2):
            pkt = self.packet_gen(32, 2, 0, 1)
        usb.write(pkt)
    def packet_tty3(self, data, len):
        print("packet tty3")
        pkt = self.packet_gen(64, 0, 0, 1)
        command = data.pop(1)
        if (command == 0): # control
            pass
        elif (command == 1):
            pkt = self.packet_gen(64, 1, 0, 1)
        elif (command == 2):
            pkt = self.packet_gen(64, 2, 0, 1)
        usb.write(pkt)
    def ctrl_syn(self, data, len):
        print("control syn")
    def ctrl_connect(self, data, len):
        print("control connect")
    def ctrl_disconnect(self, data, len):
        print("control disconnect")
    def ctrl_datetime(self, data, len):
        print("control datetime")
    def ctrl_blockdev_connect(self, data, len):
        print("control blockdev connect")
    def ctrl_blockdev_disconnect(self, data, len):
        print("control blockdev disconnect")
    def ctrl_chardev_connect(self, data, len):
        print("control chardev connect")
    def ctrl_chardev_disconnect(self, data, len):
        print("control chardev disconnect")
    def ctrl_reboot(self, data, len):
        print("control reboot")
    def usb_disk_op_lba(self, data, len):
        print("usb_disk_op_lba")
    def usb_disk_op_read(self, data, len):
        print("usb_disk_op_read")
    def usb_disk_op_write(self, data, len):
        print("usb_disk_op_write")
    def usb_disk_op_trim(self, data, len):
        print("usb_disk_op_trim")
    # protocol: packet IDs
    USB_PACKET_CTRL_BIT=8
    USB_PACKET_ID = {
        0 : packet_ctrl,
        1 : packet_disk1,
        2 : packet_disk2,
        4 : packet_disk3,
        8 : packet_core1,
        16 : packet_tty1,
        32 : packet_tty2,
        64 : packet_tty3
    }
    # protocol: control packets IDs
    USB_CTRL_ID = {
        0 : ctrl_syn,
        1 : ctrl_connect,
        2 : ctrl_disconnect,
        3 : ctrl_datetime,
        4 : ctrl_blockdev_connect,
        5 : ctrl_blockdev_disconnect,
        6 : ctrl_chardev_connect,
        7 : ctrl_chardev_disconnect,
        255 : ctrl_reboot
    }
    # disk operations
    USB_DISK_OP = {
        0 : usb_disk_op_lba,
        0 : usb_disk_op_read,
        0 : usb_disk_op_write,
        0 : usb_disk_op_trim
    }

def main():
    'Program main()'
    global pico
    pico  = Pyco()
    while (1):
        rx_packet_len = pico.usb.bulk_read(1)
        if (rx_packet_len):
            pico.packet_rx(rx_packet_len-1)

def close():
    'Clean program termination'
    print('User requested exit (CTRL-C), bye!')
    try:
        pico.close()
        sys.exit(0)
    except SystemExit:
        os._exit(0)

if __name__ == '__main__':
    'Catch CTRL-C'
    try:
        main()
    except KeyboardInterrupt:
        close()
    
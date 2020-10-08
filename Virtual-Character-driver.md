# Virtual Character driver

This project is a driver for a virtual character device that supports an unlimited amount of read/writes.

### Installation

To run the program first initlize the device file using 

```sh
$ ./init.sh
```
To check if this was successfull:
```sh
$ ls /dev
```

If it works than a virt300 file should appear.

To run the driver use
```sh
$ make
$ sudo insmod virt.ko
```

To dismount the driver and remove the small make residue use
```sh
$ sudo rmmod virt
$ make clean
```


To use the driver:
  - Use python an example script is FileTest.py, use "/dev/virt300" for the open function
  - C can also be used, use "/dev/virt300" for the open function

### Testing

  - Use ./testCH.sh to test the driver, and dmesg to see if any errors occured.
  - cat /proc/devices can be used to check if there was any initlization error. 

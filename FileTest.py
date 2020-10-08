
mw = open("/dev/virt300","r+")
mw.write("SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS")
print(mw.read())

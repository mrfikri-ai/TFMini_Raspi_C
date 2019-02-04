import serial

ser = serial.Serial("dev/ttyAMA0", 115200)

def getTFminiData():
    while True:
        count = ser.in_waiting
        if count > 8:
            recv = ser.read(9)
            ser.reset_input_buffer()
            
            if recv[0] == 'Y' and recv[1] == 'Y':
                low = int(recv[2].encode('hex'),16)
                high = int(recv[3].encode('hex'), 16)
                distance = low + high * 256
                print(distance)


if __name__ == '__main__':
    try:
        if ser.is_open == False:
            ser.open()
            getTFminiData()
    except KeyboardInterrupt:
        if ser != None:
            ser.close()

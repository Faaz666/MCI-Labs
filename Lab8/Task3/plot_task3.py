import serial
import matplotlib.pyplot as plt
from drawnow import *

# === Setup Serial ===
sinWaveData = serial.Serial('/dev/ttyACM0', 115200)
plt.ion()

tempValues = []
xValues = []
yValues = []
zValues = []
time_ms = []
cnt = 0

# === Plotting Function ===
def makeFig():
    plt.clf()
    plt.subplots_adjust(hspace=0.4)
    plt.subplot(2,1,1)
    plt.title('Live Temperature')
    plt.grid(True)
    plt.ylabel('Temp (C)')
    plt.ylim(10, 40)
    plt.plot(time_ms, tempValues, 'r.-', label='Temperature')
    plt.legend(loc='upper left')
    plt.subplot(2,1,2)
    plt.title('Gyro X Y Z')
    plt.grid(True)
    plt.xlabel('Time')
    plt.ylabel('dps')

    plt.plot(time_ms, xValues, 'g.-', label='X')
    plt.plot(time_ms, yValues, 'b.-', label='Y')
    plt.plot(time_ms, zValues, 'm.-', label='Z')
    plt.legend(loc='upper left')

# === Main Loop ===
while True:
    while sinWaveData.inWaiting() == 0:
        pass

    try:
        line = sinWaveData.readline().decode().strip()
        if line == "":
            continue

        values = line.split(',')

        temp = float(values[0])
        x = float(values[1])
        y = float(values[2])
        z = float(values[3])

        tempValues.append(temp)
        xValues.append(x)
        yValues.append(y)
        zValues.append(z)

        time_ms.append(cnt)
        cnt += 1

        drawnow(makeFig)
        plt.pause(0.0001)

        # keep last 500
        if len(tempValues) > 500:
            tempValues.pop(0)
            xValues.pop(0)
            yValues.pop(0)
            zValues.pop(0)
            time_ms.pop(0)

    except Exception as e:
        print("Error:", e)

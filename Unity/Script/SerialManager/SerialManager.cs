using System.IO.Ports;
using UnityEngine;

public class SerialManager : MonoBehaviour
{   
    // SUPER IMPORTANT: Change COM10 to your own serial port name (check Arduino IDE), like /dev/cu.usbmodem1101
    // and make sure the baud rate "115200" is the same as the one in Arduino's Serial.Begin(115200)
    SerialPort port = new SerialPort("COM10", 115200); 
    public float potVal;
    public int buttonVal;

    public float roll;
    public float pitch;
    public float yaw;
    public float accZ;
    void Start()
    {
        if (!port.IsOpen)
        {
            port.Open();
        }
    }

    void Update()
    {
        if (port.IsOpen && port.BytesToRead > 0)
        {
            string data = port.ReadLine().Trim(); // Trim() to remove \r and \n

            if (string.IsNullOrEmpty(data))
                return;

            string[] values = data.Split(','); // split data by comma like "523", "0"

            buttonVal = int.Parse(values[0]); // make sure you are acutally sending these values in the exact order in Arduino otherwise it will mess up!
            potVal = int.Parse(values[1]);
            pitch = float.Parse(values[2]);
            roll = float.Parse(values[3]);
            yaw = float.Parse(values[4]);
            accZ = float.Parse(values[5]);

            Debug.Log("buttonVal: " + buttonVal +
                    " | potVal: " + potVal + 
                    " | pitch: " + pitch + 
                    " | roll: " + roll + 
                    " | yaw: " + yaw + 
                    " | accZ: " + accZ);
        }
    }

    void OnApplicationQuit()
    {
        if (port.IsOpen)
        {
            port.Close();
        }
    }
}

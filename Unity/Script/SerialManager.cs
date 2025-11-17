using System.IO.Ports;
using UnityEngine;

public class SerialManager : MonoBehaviour
{
    SerialPort port = new SerialPort("COM10", 9600);
    public float potVal;
    public int buttonVal;

    public float roll;
    public float pitch;
    public float yaw;

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

            string[] values = data.Split(','); // split data by comma like "523", "0"

            buttonVal = int.Parse(values[0]);
            potVal = float.Parse(values[1]);
            pitch = float.Parse(values[2]);
            roll = float.Parse(values[3]);
            yaw = float.Parse(values[4]);
            
            Debug.Log("buttonVal: "+ buttonVal + "| potVal: " + potVal + " | pitch: " + pitch + " | roll: " + roll + " | yaw: " + yaw);
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

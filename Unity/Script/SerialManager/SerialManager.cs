using System.IO.Ports;
using UnityEngine;
using System; // This allows us to use "Exceptions" for error catching

public class SerialManager : MonoBehaviour
{
    // Anything 'public' shows up in the Unity window so you can change it easily.
    [Header("Connection Settings")]
    public string portName = "COM10"; // CHANGE THIS BASED ON YOUR ARDUINO'S NAME!!!
    public int baudRate = 9600; // make sure it's the same as "Serial.begin(9600)" in your arduino code
    private SerialPort port; // a private connection to arduino

    // These hold the numbers coming from the Arduino. 
    // You can watch these change in the Inspector while the game plays!
    [Header("Incoming Sensor Data")]
    public float roll;
    public float pitch;
    public float yaw;
    public float accZ;

    // Start is called once when the "Play" button is pressed
    void Start()
    {
        port = new SerialPort(portName, baudRate); // Setup the connection with the name we typed in the Inspector

        port.ReadTimeout = 10; // Unity will only wait 10ms for data, then give up and keep running. Prevents the Computer Freeze if Arduino stops talking.

        try
        {
            if (!port.IsOpen)
            {
                port.Open();
                port.DiscardInBuffer(); // Throw away any old junk data sitting in the wire so we start fresh.
                Debug.Log("Port Opened!");
            }
        }
        catch (Exception e)
        {
            Debug.LogWarning("Could not open port: " + e.Message);
        }
    }

    // Update is called every single frame of the game
    void Update()
    {
        // Check if the port is open and fully ready
        if (port != null && port.IsOpen)
        {
            // If the Arduino sent too much data, the buffer gets clogged. We dump it all.
            if (port.BytesToRead > 1024)
            {
                port.DiscardInBuffer();
            }


            if (port.BytesToRead > 0) // Only read if there is data waiting
            {
                try
                {
                    string dataLine = port.ReadLine().Trim(); // Read the line of text until it hits a new line

                    if (string.IsNullOrEmpty(dataLine)) return; // If the line is empty, just skip this frame

                    string[] splitValues = dataLine.Split(','); // Arduino sends 10.5, 5.2, 180.1, 1.0, so we split that string at every comma into a list of strings

                    if (splitValues.Length == 4) // Only try to read if we have exactly 4 pieces of data
                    {
                        // Convert the "Text" numbers into "Math" numbers
                        roll = float.Parse(splitValues[0]);
                        pitch = float.Parse(splitValues[1]);
                        yaw = float.Parse(splitValues[2]);
                        accZ = float.Parse(splitValues[3]);
                    }
                }
                catch (TimeoutException)
                {
                    // This is normal! Because we set a short timeout before, sometimes a line isn't finished yet. We just ignore it.
                }
                catch (Exception)
                {
                    // If something else went wrong, we just ignore this frame instead of crashing
                }
            }
        }
    }

    // This runs when you stop the game
    void OnApplicationQuit()
    {
        ClosePort();
    }

    void OnDisable()
    {
        ClosePort();
    }

    void ClosePort()
    {
        if (port != null && port.IsOpen)
        {
            port.DiscardInBuffer(); // Tell the port to stop reading immediately
            port.Close(); // Always close the serial port when you quit cuz you can't have two port open at the same time
            Debug.Log("Port closed safely.");
        }
    }
}
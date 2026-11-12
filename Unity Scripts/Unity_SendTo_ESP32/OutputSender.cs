// ============================================================
//  Alt.Ctrl — The Game Controls the Real World (Unity side)
//
//  Press a number key and Unity sends that character to the
//  ESP32 over the USB cable:
//     press '1' -> LED      press '2' -> buzzer      press '3' -> servo
//
//  SETUP (or it won't work):
//   - Project Settings > Player > Api Compatibility Level -> ".NET Framework"
//   - Set the correct COM port below (check the Arduino IDE)
//   - CLOSE the Arduino Serial Monitor before pressing Play
//     (only one program can use the port at a time)
// ============================================================

using System.IO.Ports;   // lets Unity talk to the serial (USB) port
using UnityEngine;

public class OutputSender : MonoBehaviour
{
    [Header("Connection")]
    public string portName = "COM5";   // CHANGE to your board's port
    public int baudRate = 115200;      // must match Serial.begin() in the Arduino
    private SerialPort port;           // our connection to the Arduino

    // Start() runs once when you press Play
    void Start()
    {
        // Set up the connection using the port name and speed above
        port = new SerialPort(portName, baudRate);

        // IMPORTANT: these stop Unity from freezing if the port is slow.
        // Without them, Unity waits forever and the whole game locks up.
        port.ReadTimeout = 50;    // give up on a read after 50 ms
        port.WriteTimeout = 50;   // give up on a write after 50 ms

        try
        {
            port.Open();                 // open the connection to the board
            port.DiscardInBuffer();      // throw away any old leftover data
            Debug.Log("Port opened!");   // show a message if it worked
        }
        catch (System.Exception e)
        {
            // If it can't open (wrong port, port in use), warn instead of crashing
            Debug.LogWarning("Could not open port: " + e.Message);
        }
    }

    // Update() runs every single frame of the game
    void Update()
    {
        // If the port isn't open, don't try to send anything
        if (port == null || !port.IsOpen) return;

        // Each key is independent, so you can add components one at a time.
        if (Input.GetKeyDown(KeyCode.Alpha1)) SendChar("1");   // '1' key -> LED
        if (Input.GetKeyDown(KeyCode.Alpha2)) SendChar("2");   // '2' key -> buzzer
        if (Input.GetKeyDown(KeyCode.Alpha3)) SendChar("3");   // '3' key -> servo
    }

    // A small helper that sends one character safely
    void SendChar(string s)
    {
        try
        {
            port.Write(s);   // send the character down the USB cable to the board
        }
        catch (System.Exception e)
        {
            // If the write fails (board not ready), warn instead of freezing
            Debug.LogWarning("Write failed: " + e.Message);
        }
    }

    // This runs when you stop the game
    void OnApplicationQuit()
    {
        // Always close the port when done, so it's free next time
        if (port != null && port.IsOpen) port.Close();
    }
}
// ============================================================
//  Alt.Ctrl — The Game Controls the Real World (Unity side)
//
//  Press a number key in Unity and it sends that character to
//  the ESP32 over the USB cable:
//     press '1' -> LED      press '2' -> buzzer      press '3' -> servo
//
//  Add components one at a time. Each key works on its own, so
//  pressing '1' works whether or not the buzzer/servo are wired.
//
//  BEFORE IT WORKS:
//   - Edit > Project Settings > Player > Api Compatibility Level
//       -> set to ".NET Framework" (so SerialPort works)
//   - Set the correct COM port below (check the Arduino IDE)
//   - Close the Arduino Serial Monitor (only one program can use the port)
// ============================================================

using System.IO.Ports;   // lets Unity talk to the serial (USB) port
using UnityEngine;

public class OutputSender : MonoBehaviour
{
    [Header("Connection")]
    public string portName = "COM5";   // CHANGE this to your board's port
    public int baudRate = 115200;      // must match Serial.begin() in the Arduino code
    private SerialPort port;           // our connection to the Arduino

    // Start() runs once when you press Play
    void Start()
    {
        // Set up the connection using the port name and speed above
        port = new SerialPort(portName, baudRate);

        try
        {
            port.Open();                 // open the connection to the board
            Debug.Log("Port opened!");   // show a message if it worked
        }
        catch (System.Exception e)
        {
            // If it couldn't open (wrong port, port in use), warn instead of crashing
            Debug.LogWarning("Could not open port: " + e.Message);
        }
    }

    // Update() runs every single frame of the game
    void Update()
    {
        // If the port isn't open, don't try to send anything
        if (port == null || !port.IsOpen) return;

        // Each key is checked on its own, so you can add components one at a time.

        // Press the "1" key -> send "1" -> ESP32 toggles the LED
        if (Input.GetKeyDown(KeyCode.Alpha1)) port.Write("1");

        // Press the "2" key -> send "2" -> ESP32 beeps the buzzer
        if (Input.GetKeyDown(KeyCode.Alpha2)) port.Write("2");

        // Press the "3" key -> send "3" -> ESP32 sweeps the servo
        if (Input.GetKeyDown(KeyCode.Alpha3)) port.Write("3");
    }

    // This runs when you stop the game
    void OnApplicationQuit()
    {
        // Always close the port when done, so it's free for next time
        if (port != null && port.IsOpen) port.Close();
    }
}
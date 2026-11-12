// ============================================================
//  Alt.Ctrl — Clickable 3D Button (Unity side)
//
//  Click the 3D button -> it visibly presses down -> sends 'P'
//  to the ESP32, which flashes the LED, beeps, and sweeps the
//  servo all at once.
//
//  SETUP:
//   - Put this script on a 3D object (e.g. a Cube) that has a Collider
//   - Project Settings > Player > Api Compatibility Level -> .NET Framework
//   - Set the correct COM port below; close the Arduino Serial Monitor
// ============================================================

using System.IO.Ports;
using UnityEngine;

public class ButtonPress : MonoBehaviour
{
    [Header("Connection")]
    public string portName = "COM5";   // CHANGE to your board's port
    public int baudRate = 115200;      // must match the Arduino
    private SerialPort port;

    [Header("Button press motion")]
    public float pressDepth = 0.2f;    // how far the button moves down when pressed
    public float pressSpeed = 10f;     // how fast it moves

    private Vector3 upPosition;        // the button's resting position
    private Vector3 downPosition;      // the pressed-down position
    private bool isPressed = false;    // is the button currently held down?

    void Start()
    {
        // Remember where the button starts, and where "down" is
        upPosition = transform.position;
        downPosition = upPosition - new Vector3(0, pressDepth, 0);

        // Open the serial connection
        port = new SerialPort(portName, baudRate);
        try
        {
            port.Open();
            Debug.Log("Port opened!");
        }
        catch (System.Exception e)
        {
            Debug.LogWarning("Could not open port: " + e.Message);
        }
    }

    // OnMouseDown runs when the player clicks this object's collider
    void OnMouseDown()
    {
        isPressed = true;   // start moving the button down

        // Send one character to the board
        if (port != null && port.IsOpen)
        {
            port.Write("P");
            Debug.Log("Sent P");
        }
    }

    // OnMouseUp runs when the mouse button is released
    void OnMouseUp()
    {
        isPressed = false;  // let the button come back up
    }

    void Update()
    {
        // Smoothly move the button toward down (if pressed) or up (if not)
        Vector3 target = isPressed ? downPosition : upPosition;
        transform.position = Vector3.Lerp(transform.position, target, Time.deltaTime * pressSpeed);
    }

    void OnApplicationQuit()
    {
        if (port != null && port.IsOpen) port.Close();
    }
}
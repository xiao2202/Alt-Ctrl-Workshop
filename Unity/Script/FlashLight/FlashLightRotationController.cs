using UnityEngine;

public class FlashLightRotationController : MonoBehaviour
{
    // Link to our SerialManager V2 script
    public SerialManager serial; 

    [Header("Settings")]
    public bool useZAxis = false; // can toggle this in Inspector
    public float smoothness = 5f; // Higher number = faster movement

    void Update()
    {
        // Make sure we found the SerialManager
        if (serial == null) return;

        // Get the angles and clean them up using a normalize function
        float pitch = Normalize180(serial.pitch);
        float yaw   = Normalize180(serial.yaw);
        float roll  = Normalize180(serial.roll);

        // use Quaternion.Euler to turn our 3 numbers into a Rotation
        Quaternion targetRotation = Quaternion.Euler(pitch, -yaw, roll);
        // Smoothing
        transform.rotation = Quaternion.Lerp(transform.rotation, targetRotation, Time.deltaTime * smoothness);
    }

    float Normalize180(float angle)
    {
        // This keeps the angle between -180 and 180
        return Mathf.Repeat(angle + 180f, 360f) - 180f;
    }
}
using UnityEngine;

public class FlashLightRotationController : MonoBehaviour
{
    public SerialManager serial;

    void Update()
    {
        // get roll, pitch, yaw from serial
        float pitch = Normalize180(serial.pitch);
        float yaw   = Normalize180(serial.yaw);
        float roll  = Normalize180(serial.roll);

        // apply to cube rotation
        transform.rotation = Quaternion.Euler(pitch, -yaw, 0);
    }

    float Normalize180(float angle)
    {
        angle = Mathf.Repeat(angle + 180f, 360f) - 180f;
        return angle;
    }
}

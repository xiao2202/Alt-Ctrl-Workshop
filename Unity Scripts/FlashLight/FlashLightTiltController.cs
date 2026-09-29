using UnityEngine;

// Controls the flashlight using only ROLL and PITCH from the Arduino Nano 33 IoT.
// Yaw is ignored because the IMU has no magnetometer, so yaw drifts over time.
//   Roll  (tilt wrist left/right) -> flashlight turns left/right (Y axis)
//   Pitch (tilt forward/back)     -> flashlight tilts up/down   (X axis)
// Roll and pitch are referenced to gravity, so they stay stable.
public class FlashLightTiltController : MonoBehaviour
{
    // Link to our SerialManager script
    public SerialManager serial;

    [Header("Left / Right (from Roll)")]
    public float rollSensitivity = 1.5f;  // How many degrees the light turns per degree of roll
    public float maxTurnAngle = 70f;      // Limit how far left/right the light can turn
    public bool invertRoll = false;

    [Header("Up / Down (from Pitch)")]
    public float pitchSensitivity = 1f;
    public float maxTiltAngle = 45f;      // Limit how far up/down the light can tilt
    public bool invertPitch = false;

    [Header("Feel")]
    public float deadZone = 2f;           // Ignore tiny hand shakes (in degrees)
    public float smoothness = 5f;         // Higher number = faster movement

    [Header("Calibration")]
    public KeyCode recenterKey = KeyCode.Space; // Press to set the current hand pose as "center"
    public bool calibrateOnStart = true;

    float rollOffset = 0f;
    float pitchOffset = 0f;
    bool calibrated = false;
    Quaternion startRotation;

    void Start()
    {
        // Remember how the flashlight was placed in the scene, and rotate relative to that
        startRotation = transform.localRotation;
    }

    void Update()
    {
        // Make sure we actually found the SerialManager
        if (serial == null) return;

        float roll  = Normalize180(serial.roll);
        float pitch = Normalize180(serial.pitch);

        // Calibrate once on the first frame of data, or whenever the key is pressed
        if ((calibrateOnStart && !calibrated) || Input.GetKeyDown(recenterKey))
        {
            rollOffset = roll;
            pitchOffset = pitch;
            calibrated = true;
        }

        // Angle relative to the calibrated "center" pose
        float rollDelta  = Normalize180(roll - rollOffset);
        float pitchDelta = Normalize180(pitch - pitchOffset);

        rollDelta  = ApplyDeadZone(rollDelta);
        pitchDelta = ApplyDeadZone(pitchDelta);

        if (invertRoll)  rollDelta  = -rollDelta;
        if (invertPitch) pitchDelta = -pitchDelta;

        // Roll -> turn left/right, Pitch -> tilt up/down
        float turn = Mathf.Clamp(rollDelta * rollSensitivity, -maxTurnAngle, maxTurnAngle);
        float tilt = Mathf.Clamp(pitchDelta * pitchSensitivity, -maxTiltAngle, maxTiltAngle);

        // Apply yaw (turn) first, then pitch (tilt), so the light doesn't twist sideways
        Quaternion targetRotation = startRotation * Quaternion.Euler(tilt, turn, 0f);

        // Smoothing
        transform.localRotation = Quaternion.Slerp(transform.localRotation, targetRotation, Time.deltaTime * smoothness);
    }

    float ApplyDeadZone(float angle)
    {
        // Remove the dead zone, but keep movement continuous past its edge
        if (Mathf.Abs(angle) < deadZone) return 0f;
        return angle - Mathf.Sign(angle) * deadZone;
    }

    float Normalize180(float angle)
    {
        // This keeps the angle between -180 and 180
        return Mathf.Repeat(angle + 180f, 360f) - 180f;
    }
}

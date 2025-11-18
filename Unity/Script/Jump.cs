using UnityEngine;

public class Jump : MonoBehaviour
{
    public SerialManager serial;  // drag your SerialManager here
    public float jumpForce = 10f;
    public float threshold = 30f;

    private Rigidbody rb;
    private bool canJump = true;

    void Start()
    {
        rb = GetComponent<Rigidbody>();
    }

    void Update()
    {
        if (serial == null) return;

        // When Arduino’s Z acceleration exceeds threshold
        if (serial.accZ > threshold && canJump)
        {
            rb.AddForce(Vector3.up * jumpForce, ForceMode.Impulse);
            canJump = false;
        }

        // Allow jumping again when value falls back down
        if (serial.accZ < threshold * 0.5f)
        {
            canJump = true;
        }
    }
}

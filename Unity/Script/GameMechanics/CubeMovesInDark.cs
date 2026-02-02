using UnityEngine;

public class CubeMovesInDark : MonoBehaviour
{
    public Light flashLight;            // Drag your Spotlight here
    public float moveSpeed = 3f;
    public Vector3 moveDirection = Vector3.forward;

    private Rigidbody rb;

    void Start()
    {
        rb = GetComponent<Rigidbody>();
        // Add Rigidbody if missing, enable Gravity
        if (rb == null) rb = gameObject.AddComponent<Rigidbody>();
        rb.useGravity = true;
    }

    void FixedUpdate()
    {
        // LOGIC: Is the light ON and BRIGHT enough?
        // We check if the component is enabled AND intensity is greater than 0.1
        bool isLightOn = flashLight.enabled && flashLight.intensity > 0.1f;

        if (isLightOn)
        {
            StopCube(); // Light is ON -> Freeze!
        }
        else
        {
            MoveCube(); // Light is OFF/Dim -> Move!
        }
    }

    void MoveCube()
    {
        // Use rb.velocity for older Unity, rb.linearVelocity for Unity 6+
        rb.linearVelocity = moveDirection.normalized * moveSpeed;
    }

    void StopCube()
    {
        rb.linearVelocity = Vector3.zero;
    }
}
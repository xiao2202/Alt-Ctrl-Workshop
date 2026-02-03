using UnityEngine;

public class CubeMovesInDark : MonoBehaviour
{
    public Light flashLight;            // drag your Spotlight here in inspector
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
        // is the light on and bright enough?
        // check if the component is enabled and intensity is greater than 0.1
        bool isLightOn = flashLight.enabled && flashLight.intensity > 0.1f;

        if (isLightOn)
        {
            StopCube(); // Light is on - cube stop moving
        }
        else
        {
            MoveCube(); // Light is off - cube start moving
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
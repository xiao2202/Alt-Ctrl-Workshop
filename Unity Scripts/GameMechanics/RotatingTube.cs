using System;
using UnityEngine;

public class RotatingTube : MonoBehaviour
{
    public SerialManager serial;
    public float rotationSpeed = 1f;
    
    void Update()
    {
        if (serial == null) return;

        //float yRotation = serial.pitch * rotationSpeed;
        float yRotation = Mathf.Lerp(0f, 360, serial.potVal / 1023f);
        transform.rotation = Quaternion.Euler(0f, yRotation, 90f);
    }
}

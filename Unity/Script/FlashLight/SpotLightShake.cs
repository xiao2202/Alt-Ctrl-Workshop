using UnityEngine;

public class SpotLightShake : MonoBehaviour
{
    public SerialManager serial;
    private Light spotLight;

    public GameObject emissiveCylinder;

    [Header("Light behavior")]
    public float maxIntensity = 20f;      // highest it can go
    public float minIntensity = 0f;       // lowest it can go
    public float fadeSpeed = 2f;          // how fast it fades per second
    public float shakeThreshold = 1.5f;    // accZ above this counts as a shake
    public float shakeBoost = 1f;         // how much intensity to add per shake
    private float currentIntensity;

    void Start()
    {
        spotLight = GetComponent<Light>();
        currentIntensity = maxIntensity * 0.5f;   // start at half brightness
    }

    void Update()
    {
        // Fade out over time
        currentIntensity -= fadeSpeed * Time.deltaTime;

        // If shaken hard enough, increase intensity a bit
        float accZ = serial.accZ;        // get data from SerialManager
        if (Mathf.Abs(accZ) > shakeThreshold)
        {
            currentIntensity += shakeBoost;
        }

        // Clamp between min and max
        currentIntensity = Mathf.Clamp(currentIntensity, minIntensity, maxIntensity);

        // Apply to light
        spotLight.intensity = currentIntensity;

        bool isOn = currentIntensity > 0.05f;
        spotLight.enabled = isOn;

        if (emissiveCylinder != null)
        {
            emissiveCylinder.SetActive(isOn);
        }
    }
}


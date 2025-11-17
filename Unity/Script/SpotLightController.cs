using UnityEngine;

public class SpotLightController : MonoBehaviour
{
    public SerialManager serial;
    private Light spotLight;

    private bool lightOn = false;      // current toggle state
    private int lastButton = 0;        // previous button value (0 or 1)

    void Start()
    {
        spotLight = GetComponent<Light>();
    }

    void Update()
    {   
        int button = serial.buttonVal;

        // if button is pressed and is not pressed before
        if (lastButton == 0 && button == 1)
        {
            lightOn = !lightOn;  // toggle the state of light
        }

        // apply toggle result
        spotLight.enabled = lightOn;

        // update lastButton for the next frame
        lastButton = button;

        // Pot controls intensity
        float pot = Mathf.Clamp(serial.potVal, 0f, 1023f);
        spotLight.intensity = Mathf.Lerp(0f, 10f, pot / 1023f);
    }
}
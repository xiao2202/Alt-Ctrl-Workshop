// ============================================================
//  Move a Circle with the FireBeetle + BNO055 (smoothed)
//  Tilt the board -> the circle glides. Turn it -> color changes.
//  Chrome or Edge only. Click the canvas to connect.
//  Expects serial: roll,pitch,yaw  at 115200 baud
// ============================================================


let port;
let reader;

// The RAW numbers coming from the board (a bit jumpy).
let roll = 0;
let pitch = 0;
let yaw = 0;

// The SMOOTHED values we actually draw (these glide, not jump).
let smoothX = 300;
let smoothY = 300;
let smoothHue = 0;

// How much to smooth. Lower = calmer, higher = snappier. Try 0.05 to 0.3.
let smoothing = 0.15;

let textFromArduino = "";
let connected = false;


function setup() {
  createCanvas(600, 600);
  textAlign(CENTER, CENTER);
  textSize(16);
}


function draw() {
  background(20);

  if (connected === false) {
    fill(200);
    text("Click to connect to FireBeetle", width / 2, height / 2);
    return;
  }

  // Work out where the circle SHOULD be (the target).
  // BNO055 ranges: roll -90..90, pitch -180..180, yaw 0..360
  let targetX = map(roll, -90, 90, 0, width, true);
  let targetY = map(pitch, -180, 180, 0, height, true);
  let targetHue = ((yaw % 360) + 360) % 360;

  // Glide the smoothed values a little toward the target.
  smoothX = lerp(smoothX, targetX, smoothing);
  smoothY = lerp(smoothY, targetY, smoothing);
  smoothHue = lerp(smoothHue, targetHue, smoothing);

  // Draw the circle at the SMOOTHED position.
  colorMode(HSB, 360, 100, 100);
  fill(smoothHue, 80, 100);
  noStroke();
  circle(smoothX, smoothY, 80);

  // Show the raw numbers at the bottom.
  colorMode(RGB, 255);
  fill(255);
  text(
    "roll " + roll.toFixed(1) +
    "   pitch " + pitch.toFixed(1) +
    "   yaw " + yaw.toFixed(1),
    width / 2, height - 30
  );
}


async function mousePressed() {
  if (connected === true) {
    return;
  }
  try {
    port = await navigator.serial.requestPort();
    await port.open({ baudRate: 115200 });
    reader = port.readable.getReader();
    connected = true;
    readFromArduino();
  } catch (err) {
    console.log("Could not connect:", err);
  }
}


async function readFromArduino() {
  const decoder = new TextDecoder();
  try {
    const result = await reader.read();
    if (result.done === true || connected === false) {
      return;
    }
    textFromArduino += decoder.decode(result.value);
    let lines = textFromArduino.split("\n");
    textFromArduino = lines.pop();
    for (let line of lines) {
      let parts = line.trim().split(",");
      if (parts.length === 3) {
        let r = parseFloat(parts[0]);
        let p = parseFloat(parts[1]);
        let yw = parseFloat(parts[2]);
        if (!isNaN(r) && !isNaN(p) && !isNaN(yw)) {
          roll = r;
          pitch = p;
          yaw = yw;
        }
      }
    }
  } catch (err) {
    console.log("Error reading data:", err);
    return;
  }
  readFromArduino();
}
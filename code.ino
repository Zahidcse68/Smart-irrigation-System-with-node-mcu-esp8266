#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "your hotspot name ";
const char* password = "your hotspot passwod ";

ESP8266WebServer server(80);

const int relayPin = D1;
const int soilPin = A0;

// =====================================================
// CALIBRATION
// =====================================================

const int DRY_VALUE = 1024;
const int WET_VALUE = 374;

// Automatic irrigation threshold
const int MOISTURE_THRESHOLD = 70;

// true  = automatic mode
// false = manual mode
bool automaticMode = true;

// Manual pump state
bool manualPump = false;


// =====================================================
// READ MOISTURE
// =====================================================

int getMoisture() {

  int rawValue = analogRead(soilPin);

  int moisture = map(
    rawValue,
    DRY_VALUE,
    WET_VALUE,
    1,
    100
  );

  moisture = constrain(
    moisture,
    1,
    100
  );

  return moisture;
}


// =====================================================
// APPLY PUMP CONTROL
// =====================================================

void updatePump() {

  int moisture = getMoisture();

  if (automaticMode) {

    if (moisture < MOISTURE_THRESHOLD) {

      digitalWrite(
        relayPin,
        LOW
      );

    }
    else {

      digitalWrite(
        relayPin,
        HIGH
      );

    }

  }
  else {

    if (manualPump) {

      digitalWrite(
        relayPin,
        LOW
      );

    }
    else {

      digitalWrite(
        relayPin,
        HIGH
      );

    }

  }

}


// =====================================================
// DASHBOARD
// =====================================================

void handleRoot() {

  String html = R"rawliteral(
<!DOCTYPE html>

<html>

<head>

<meta name="viewport" content="width=device-width, initial-scale=1.0">

<title>Smart Irrigation</title>

<style>

* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
}

body {

  font-family:
    Arial,
    Helvetica,
    sans-serif;

  background:
    #f1f5f9;

  color:
    #17202a;

  min-height:
    100vh;

}


/* HEADER */

.header {

  background:
    linear-gradient(
      135deg,
      #087f5b,
      #045d43
    );

  color:
    white;

  padding:
    28px 20px;

  text-align:
    center;

  box-shadow:
    0 5px 20px
    rgba(0,0,0,0.15);

}

.header h1 {

  font-size:
    30px;

  font-weight:
    700;

}

.header p {

  margin-top:
    7px;

  font-size:
    14px;

  opacity:
    0.85;

}


/* CONTAINER */

.container {

  max-width:
    1050px;

  margin:
    30px auto;

  padding:
    0 18px;

}


/* GRID */

.grid {

  display:
    grid;

  grid-template-columns:
    repeat(
      auto-fit,
      minmax(320px,1fr)
    );

  gap:
    22px;

}


/* CARD */

.card {

  background:
    white;

  border-radius:
    20px;

  padding:
    25px;

  box-shadow:
    0 8px 30px
    rgba(0,0,0,0.07);

}

.title {

  font-size:
    19px;

  font-weight:
    700;

  color:
    #263238;

}

.subtitle {

  margin-top:
    5px;

  font-size:
    13px;

  color:
    #89939d;

}


/* GAUGE */

.gauge-area {

  position:
    relative;

  height:
    250px;

  display:
    flex;

  justify-content:
    center;

  align-items:
    center;

}

#gauge {

  width:
    300px;

  height:
    200px;

}

.gauge-center {

  position:
    absolute;

  left:
    0;

  right:
    0;

  bottom:
    35px;

  text-align:
    center;

}

.gauge-number {

  font-size:
    42px;

  font-weight:
    800;

  color:
    #087f5b;

}

.gauge-label {

  font-size:
    13px;

  color:
    #89939d;

}


/* CONDITION */

.condition {

  text-align:
    center;

  margin-top:
    5px;

}

.badge {

  display:
    inline-block;

  padding:
    9px 22px;

  border-radius:
    30px;

  font-size:
    14px;

  font-weight:
    700;

}

.dry {

  background:
    #ffe1df;

  color:
    #c0392b;

}

.medium {

  background:
    #fff0cf;

  color:
    #a86d00;

}

.wet {

  background:
    #d8f5e8;

  color:
    #087f5b;

}


/* INFORMATION */

.info-grid {

  display:
    grid;

  grid-template-columns:
    repeat(2,1fr);

  gap:
    12px;

  margin-top:
    20px;

}

.info {

  background:
    #f7f9fb;

  padding:
    16px;

  border-radius:
    12px;

  text-align:
    center;

}

.info-label {

  color:
    #89939d;

  font-size:
    12px;

}

.info-value {

  margin-top:
    5px;

  font-size:
    21px;

  font-weight:
    700;

}


/* MODE */

.mode-box {

  margin-top:
    25px;

  padding:
    18px;

  background:
    #f7f9fb;

  border-radius:
    14px;

}

.mode-title {

  font-size:
    13px;

  color:
    #89939d;

  margin-bottom:
    12px;

}


/* SWITCH */

.switch-row {

  display:
    flex;

  align-items:
    center;

  justify-content:
    space-between;

}

.switch-label {

  font-size:
    18px;

  font-weight:
    700;

}

.switch {

  position:
    relative;

  width:
    62px;

  height:
    34px;

}

.switch input {

  opacity:
    0;

  width:
    0;

  height:
    0;

}

.slider {

  position:
    absolute;

  cursor:
    pointer;

  inset:
    0;

  background:
    #bdc3c7;

  border-radius:
    34px;

  transition:
    0.25s;

}

.slider:before {

  content:
    "";

  position:
    absolute;

  width:
    26px;

  height:
    26px;

  left:
    4px;

  bottom:
    4px;

  background:
    white;

  border-radius:
    50%;

  transition:
    0.25s;

}

input:checked + .slider {

  background:
    #087f5b;

}

input:checked + .slider:before {

  transform:
    translateX(28px);

}


/* MODE TEXT */

.mode-status {

  margin-top:
    12px;

  font-size:
    13px;

  color:
    #68737c;

}


/* PUMP */

.pump-box {

  text-align:
    center;

  margin-top:
    28px;

}

.pump-status {

  font-size:
    28px;

  font-weight:
    800;

}

.pump-on {

  color:
    #087f5b;

}

.pump-off {

  color:
    #69747d;

}

.pump-description {

  margin-top:
    7px;

  font-size:
    13px;

  color:
    #89939d;

}


/* MANUAL CONTROL */

.manual-control {

  margin-top:
    22px;

  display:
    none;

}

.manual-button {

  width:
    100%;

  border:
    none;

  border-radius:
    12px;

  padding:
    15px;

  background:
    #087f5b;

  color:
    white;

  font-size:
    16px;

  font-weight:
    700;

  cursor:
    pointer;

}

.manual-button.off {

  background:
    #c0392b;

}


/* CONNECTION */

.connection {

  margin-top:
    22px;

  padding:
    14px;

  border-radius:
    12px;

  background:
    #f7f9fb;

  display:
    flex;

  justify-content:
    space-between;

}

.online {

  color:
    #087f5b;

  font-weight:
    700;

}


/* FOOTER */

.footer {

  text-align:
    center;

  padding:
    25px;

  color:
    #89939d;

  font-size:
    12px;

}


/* MOBILE */

@media(max-width:600px) {

  .header h1 {

    font-size:
      24px;

  }

  .container {

    padding:
      0 12px;

  }

  .card {

    padding:
      20px;

  }

}

</style>

</head>


<body>


<div class="header">

  <h1>
    Smart Irrigation
  </h1>

  <p>
    Automatic Soil Moisture Based Water Management
  </p>

</div>


<div class="container">


<div class="grid">


<!-- =================================================
     SOIL CARD
     ================================================= -->

<div class="card">

  <div class="title">
    Soil Moisture
  </div>

  <div class="subtitle">
    Real-time sensor monitoring
  </div>


  <div class="gauge-area">

    <canvas
      id="gauge"
      width="300"
      height="200">
    </canvas>


    <div class="gauge-center">

      <div
        id="moisture"
        class="gauge-number">

        --

      </div>

      <div class="gauge-label">
        Moisture
      </div>

    </div>

  </div>


  <div class="condition">

    <span
      id="condition"
      class="badge dry">

      Checking

    </span>

  </div>


  <div class="info-grid">


    <div class="info">

      <div class="info-label">
        RAW SENSOR
      </div>

      <div
        id="raw"
        class="info-value">

        ---

      </div>

    </div>


    <div class="info">

      <div class="info-label">
        AUTO THRESHOLD
      </div>

      <div class="info-value">

        70%

      </div>

    </div>


  </div>


</div>


<!-- =================================================
     CONTROL CARD
     ================================================= -->

<div class="card">

  <div class="title">
    Irrigation Control
  </div>

  <div class="subtitle">
    Automatic and manual pump control
  </div>


  <!-- MODE -->

  <div class="mode-box">

    <div class="mode-title">
      CONTROL MODE
    </div>


    <div class="switch-row">

      <div
        id="modeLabel"
        class="switch-label">

        AUTOMATIC

      </div>


      <label class="switch">

        <input
          id="modeSwitch"
          type="checkbox"
          checked
          onchange="changeMode()">

        <span class="slider"></span>

      </label>

    </div>


    <div
      id="modeStatus"
      class="mode-status">

      Pump is controlled automatically
      according to soil moisture.

    </div>

  </div>


  <!-- PUMP -->

  <div class="pump-box">

    <div
      id="pumpStatus"
      class="pump-status pump-off">

      OFF

    </div>


    <div
      id="pumpDescription"
      class="pump-description">

      Pump is stopped

    </div>


    <!-- MANUAL BUTTON -->

    <div
      id="manualControl"
      class="manual-control">

      <button
        id="manualButton"
        class="manual-button"
        onclick="toggleManualPump()">

        TURN PUMP ON

      </button>

    </div>

  </div>


  <div class="connection">

    <span>
      ESP8266 Status
    </span>

    <span class="online">
      ONLINE
    </span>

  </div>


</div>


</div>


</div>


<div class="footer">

  Smart Irrigation System |
  ESP8266 NodeMCU

</div>



<script>


// =====================================================
// VARIABLES
// =====================================================

let automaticMode = true;

let manualPump = false;


// =====================================================
// GAUGE
// =====================================================

const canvas =
  document.getElementById(
    "gauge"
  );

const ctx =
  canvas.getContext(
    "2d"
  );


function drawGauge(
  value
) {


  ctx.clearRect(
    0,
    0,
    canvas.width,
    canvas.height
  );


  const centerX = 150;

  const centerY = 155;

  const radius = 110;


  let percentage =
    (value - 1) / 99;


  percentage =
    Math.max(
      0,
      Math.min(
        1,
        percentage
      )
    );


  const startAngle =
    Math.PI;


  const valueAngle =
    Math.PI +
    (
      Math.PI *
      percentage
    );


  // Background

  ctx.beginPath();

  ctx.arc(
    centerX,
    centerY,
    radius,
    startAngle,
    0
  );

  ctx.lineWidth =
    28;

  ctx.lineCap =
    "round";

  ctx.strokeStyle =
    "#e7ecef";

  ctx.stroke();


  // Value

  ctx.beginPath();

  ctx.arc(
    centerX,
    centerY,
    radius,
    startAngle,
    valueAngle
  );

  ctx.lineWidth =
    28;


  if (
    value < 30
  ) {

    ctx.strokeStyle =
      "#e74c3c";

  }

  else if (
    value < 70
  ) {

    ctx.strokeStyle =
      "#f39c12";

  }

  else {

    ctx.strokeStyle =
      "#20b957";

  }


  ctx.stroke();


  // Needle

  const angle =
    Math.PI +
    (
      Math.PI *
      percentage
    );


  const needleLength =
    80;


  const x =
    centerX +
    Math.cos(angle) *
    needleLength;


  const y =
    centerY +
    Math.sin(angle) *
    needleLength;


  ctx.beginPath();

  ctx.moveTo(
    centerX,
    centerY
  );

  ctx.lineTo(
    x,
    y
  );

  ctx.lineWidth =
    5;

  ctx.lineCap =
    "round";

  ctx.strokeStyle =
    "#263238";

  ctx.stroke();


  // Center

  ctx.beginPath();

  ctx.arc(
    centerX,
    centerY,
    8,
    0,
    Math.PI * 2
  );

  ctx.fillStyle =
    "#263238";

  ctx.fill();


  // Labels

  ctx.font =
    "12px Arial";

  ctx.fillStyle =
    "#89939d";

  ctx.textAlign =
    "center";


  ctx.fillText(
    "1%",
    38,
    177
  );


  ctx.fillText(
    "70%",
    150,
    45
  );


  ctx.fillText(
    "100%",
    262,
    177
  );

}



// =====================================================
// CONDITION
// =====================================================

function updateCondition(
  moisture
) {


  const element =
    document.getElementById(
      "condition"
    );


  if (
    moisture < 30
  ) {

    element.innerHTML =
      "DRY";

    element.className =
      "badge dry";

  }

  else if (
    moisture < 70
  ) {

    element.innerHTML =
      "MOISTURE LOW";

    element.className =
      "badge medium";

  }

  else {

    element.innerHTML =
      "MOISTURE GOOD";

    element.className =
      "badge wet";

  }

}



// =====================================================
// PUMP DISPLAY
// =====================================================

function updatePump(
  relay
) {


  const status =
    document.getElementById(
      "pumpStatus"
    );


  const description =
    document.getElementById(
      "pumpDescription"
    );


  if (
    relay === "ON"
  ) {

    status.innerHTML =
      "ON";

    status.className =
      "pump-status pump-on";

    description.innerHTML =
      "Pump is running";

  }

  else {

    status.innerHTML =
      "OFF";

    status.className =
      "pump-status pump-off";

    description.innerHTML =
      "Pump is stopped";

  }

}



// =====================================================
// CHANGE MODE
// =====================================================

function changeMode() {


  const switchElement =
    document.getElementById(
      "modeSwitch"
    );


  automaticMode =
    switchElement.checked;


  fetch(
    "/mode?auto=" +
    (
      automaticMode
      ? "1"
      : "0"
    )
  )


  .then(
    function() {

      updateModeUI();

    }
  );

}



// =====================================================
// MODE UI
// =====================================================

function updateModeUI() {


  const label =
    document.getElementById(
      "modeLabel"
    );


  const status =
    document.getElementById(
      "modeStatus"
    );


  const manual =
    document.getElementById(
      "manualControl"
    );


  if (
    automaticMode
  ) {


    label.innerHTML =
      "AUTOMATIC";


    status.innerHTML =
      "Pump turns ON below 70% moisture and OFF at 70% or above.";


    manual.style.display =
      "none";


  }

  else {


    label.innerHTML =
      "MANUAL";


    status.innerHTML =
      "Pump is controlled using the manual switch below.";


    manual.style.display =
      "block";


    updateManualButton();

  }

}



// =====================================================
// MANUAL PUMP
// =====================================================

function toggleManualPump() {


  manualPump =
    !manualPump;


  fetch(
    "/manual?pump=" +
    (
      manualPump
      ? "1"
      : "0"
    )
  );


  updateManualButton();

}



// =====================================================
// MANUAL BUTTON
// =====================================================

function updateManualButton() {


  const button =
    document.getElementById(
      "manualButton"
    );


  if (
    manualPump
  ) {


    button.innerHTML =
      "TURN PUMP OFF";


    button.className =
      "manual-button off";


  }

  else {


    button.innerHTML =
      "TURN PUMP ON";


    button.className =
      "manual-button";

  }

}



// =====================================================
// GET DASHBOARD DATA
// =====================================================

function updateDashboard() {


  fetch(
    "/data",
    {
      cache:
        "no-store"
    }
  )


  .then(
    function(response) {

      return response.json();

    }
  )


  .then(
    function(data) {


      const moisture =
        Number(
          data.moisture
        );


      document.getElementById(
        "moisture"
      ).innerHTML =
        moisture + "%";


      document.getElementById(
        "raw"
      ).innerHTML =
        data.raw;


      drawGauge(
        moisture
      );


      updateCondition(
        moisture
      );


      updatePump(
        data.relay
      );


      automaticMode =
        data.auto;


      manualPump =
        data.manual;


      document.getElementById(
        "modeSwitch"
      ).checked =
        automaticMode;


      updateModeUI();

    }
  )


  .catch(
    function(error) {

      console.log(
        error
      );

    }
  );

}



// =====================================================
// START
// =====================================================

drawGauge(1);

updateDashboard();


// Update every 2 seconds

setInterval(
  updateDashboard,
  2000
);


</script>

</body>

</html>

)rawliteral";


  server.send(
    200,
    "text/html",
    html
  );

}



// =====================================================
// SENSOR DATA
// =====================================================

void handleData() {


  int rawValue =
    analogRead(
      soilPin
    );


  int moisture =
    map(
      rawValue,
      DRY_VALUE,
      WET_VALUE,
      1,
      100
    );


  moisture =
    constrain(
      moisture,
      1,
      100
    );


  String relayStatus =
    (
      digitalRead(
        relayPin
      ) == HIGH
    )
    ? "ON"
    : "OFF";


  String json =
    "{\"moisture\":" +
    String(moisture) +
    ",\"raw\":" +
    String(rawValue) +
    ",\"relay\":\"" +
    relayStatus +
    "\",\"auto\":" +
    String(
      automaticMode
      ? "true"
      : "false"
    ) +
    ",\"manual\":" +
    String(
      manualPump
      ? "true"
      : "false"
    ) +
    "}";


  server.send(
    200,
    "application/json",
    json
  );

}



// =====================================================
// CHANGE MODE
// =====================================================

void handleMode() {


  if (
    server.hasArg(
      "auto"
    )
  ) {


    automaticMode =
      server.arg(
        "auto"
      ) == "1";


    if (
      automaticMode
    ) {

      manualPump =
        false;

    }


    updatePump();

  }


  server.send(
    200,
    "text/plain",
    "OK"
  );

}



// =====================================================
// MANUAL PUMP
// =====================================================

void handleManual() {


  if (
    server.hasArg(
      "pump"
    )
  ) {


    manualPump =
      server.arg(
        "pump"
      ) == "1";


    if (
      !automaticMode
    ) {

      updatePump();

    }

  }


  server.send(
    200,
    "text/plain",
    "OK"
  );

}



// =====================================================
// SETUP
// =====================================================

void setup() {


  pinMode(
    relayPin,
    OUTPUT
  );


  // Pump OFF at startup

  digitalWrite(
    relayPin,
    LOW
  );


  Serial.begin(
    115200
  );


  // WiFi

  WiFi.begin(
    ssid,
    password
  );


  while (
    WiFi.status()
    != WL_CONNECTED
  ) {

    delay(500);

  }


  // Routes

  server.on(
    "/",
    handleRoot
  );


  server.on(
    "/data",
    handleData
  );


  server.on(
    "/mode",
    handleMode
  );


  server.on(
    "/manual",
    handleManual
  );


  server.begin();

}



// =====================================================
// LOOP
// =====================================================

void loop() {


  server.handleClient();


  // Automatic irrigation

  if (
    automaticMode
  ) {

    updatePump();

  }


  delay(500);

}

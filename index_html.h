const char index_html[] = R"=="==(
<!DOCTYPE HTML><html>
<head>
  <title>Sizable7 Clock</title>
  <meta name="apple-mobile-web-app-status-bar-style" content="black-translucent"/>  
  <meta name="viewport" content="initial-scale=1, viewport-fit=cover, user-scalable=no" />
  <META HTTP-EQUIV="CACHE-CONTROL" CONTENT="NO-CACHE">
  <META HTTP-EQUIV="PRAGMA" CONTENT="NO-CACHE">  
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <link rel="apple-touch-icon" sizes="180x180" href="data:image/png;base64,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">
  <link rel="icon" type="image/png" sizes="32x32" href="data:image/png;base64,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">
  <link rel="icon" type="image/png" sizes="16x16" href="data:image/png;base64,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">
  <link rel="manifest" href='data:application/manifest+json,{"display": "standalone"}' />
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    p { font-size: 1.2rem;}
    body { margin: 0; -webkit-tap-highlight-color: transparent;}
    .topnav { overflow: hidden; background-color: #50B8B4; color: white; font-size: 1rem; }
    .content { padding: 20px; }
    .card { background-color: white; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); }
    .cards { max-width: 800px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); }
    .reading { font-size: 1.4rem; }
    .button {background-color: #50B8B4; /* Green */ border: none; color: white; padding: 20px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; border-radius: 8px;}
  </style>
</head>
<body>
  <div id="topnav" class="topnav">
    <h1>Sizable7 Clock</h1>
  </div>
  <div class="content">
    <div id="info" class="cards">
      <div class="card" onclick="postChange('actionVal', 1)">
        <p><i class="fas fa-clock" style="color:#059e8a;"></i> Time</p><p><span class="reading"><span id="time">%TIME%</span></span></p>
      </div>
      <div class="card">
        <p><i class="fas fa-sun" style="color:#ffe602;"></i> Ambient Light</p><p><span class="reading"><span id="ambientLight">%AMBIENTLIGHT%</span></span></p>
      </div>
      <div class="card" onclick="postChange('actionVal', 2)">
        <p><i class="fas fa-temperature-high" style="color:#ff3b3b;"></i> TEMPERATURE</p><p><span class="reading"><span id="temperature">%TEMPERATURE%</span> &#8451;</span></p>
      </div>
      <div class="card" onclick="postChange('actionVal', 2)">
        <p><i class="fas fa-water" style="color:#37bee4;"></i> HUMIDITY</p><p><span class="reading"><span id="humidity">%HUMIDITY%</span> &#37;</span></p>
      </div>
    </div>
    <div id="settings" style="display: none;">
        <div style="text-align:left; margin-top: 2em">
            <label for="digitHue">Color (between 0 and 255):</label><br>
            <input type="range" id="digitHue" name="digitHue" min="0" max="255"><span id="digitHueVal" style="padding-left:1em;"></span><span style="padding-left:1em;"><button class="button" onclick="postChange('digitHueVal', document.getElementById('digitHue').value)">SET</button></span><br>
            <label for="stdcolor">Or pick:</label>
            <select name="stdcolor" id="stdcolor" onchange="setStdColor()">
              <option value="0">Standard Color</option>
              <option value="0">Red</option>
              <option value="32">Orange</option>
              <option value="64">Yellow</option>
              <option value="96">Green</option>
              <option value="128">Aqua</option>
              <option value="160">Blue</option>
              <option value="192">Purple</option>
              <option value="224">Pink</option>
            </select>      
          </div>
          <div style="text-align:left; margin-top: 2em">
            <label for="brightness">Brightness (between 0 and 255):</label><br>
            <input type="range" id="brightness" name="brightness" min="0" max="255"><span id="brightnessVal" style="padding-left:1em;"></span><span style="padding-left:1em;"><button class="button" onclick="postChange('brightnessVal', document.getElementById('brightness').value)">SET</button></span>
          </div>
          <div style="text-align:left; margin-top: 2em">
              <input type="checkbox" id="smoothTransition" name="smoothTransition" onclick="onClick_smoothTransition()">
              <label for="smoothTransition"> Smooth Transition</label>
          </div>
          <div id="smoothSpeedSliderDiv" style="text-align:left; margin-top: 2em">
            <label for="smoothSpeed">Transition Speed (between 0 and 50):</label><br>
            <input type="range" id="smoothSpeed" name="smoothSpeed" min="0" max="50"><span id="smoothSpeedVal" style="padding-left:1em;"></span><span style="padding-left:1em;"><button class="button" onclick="postChange('smoothSpeedVal', document.getElementById('smoothSpeed').value)">SET</button></span>
          </div>
          <div style="text-align:left; margin-top: 2em">
              <input type="checkbox" id="pacifica" name="pacifica" onclick="postChange('pacificaVal', document.getElementById('pacifica').checked)">
              <label for="pacifica"> Pacifica</label>
          </div>
          <div style="text-align:left; margin-top: 2em">
              <input type="checkbox" id="fire" name="fire" onclick="postChange('fireVal', document.getElementById('fire').checked)">
              <label for="fire"> Fire</label>
          </div>
          <div style="text-align:left; margin-top: 2em">
              <input type="checkbox" id="clapDetection" name="clapDetection"  onclick="onClick_clapDetection()">
              <label for="clapDetection"> Clap Detection</label>
          </div>
          <div id="clapHelperDiv" style="text-align:left; margin-top: 2em">
              <input type="checkbox" id="clapHelper" name="clapHelper" onclick="postChange('clapHelperVal', document.getElementById('clapHelper').checked)">
              <label for="clapHelper"> Clap Helper</label>
          </div>
          <div style="text-align:left; margin-top: 2em">
              <input type="checkbox" id="beatDetection" name="beatDetection" onclick="onClick_beatDetection()">
              <label for="beatDetection"> Music/Sound Analyser</label>
          </div>
    </div>
    <br><br>
    <button id="switchButton" class="button" onclick="switchUI()">Settings</button>
  </div>
<script>

    // window.addEventListener("focus", function(){
    //   alert("Page in focus");
    // })

    let touchstartX = 0
    let touchendX = 0
    
    var topnavDiv = document.getElementById("topnav"); 
    var infoDiv = document.getElementById("info");
    var settingsDiv = document.getElementById("settings");
    var switchButton = document.getElementById("switchButton");

    var digitHueSlider = document.getElementById("digitHue");
    var digitHueOutput = document.getElementById("digitHueVal");
    var stdColorSelect = document.getElementById('stdcolor');
    digitHueSlider.value = %DIGITHUE%;
    digitHueOutput.innerHTML = digitHueSlider.value;
    digitHueSlider.oninput = function() {
        digitHueOutput.innerHTML = this.value;
        stdColorSelect.value = 0;
    }

    var brightnessSlider = document.getElementById("brightness");
    var brightnessOutput = document.getElementById("brightnessVal");
    brightnessSlider.value = %BRIGHTNESS%;
    brightnessOutput.innerHTML = brightnessSlider.value;
    brightnessSlider.oninput = function() {
        brightnessOutput.innerHTML = this.value;
    }

    var smoothTransition = document.getElementById("smoothTransition");
    smoothTransition.checked = %SMOOTHTRANSITION%

    var smoothSpeedSlider = document.getElementById("smoothSpeed");
    var smoothSpeedOutput = document.getElementById("smoothSpeedVal");
    smoothSpeedSlider.value = %SMOOTHSPEED%;
    smoothSpeedOutput.innerHTML = smoothSpeedSlider.value;
    smoothSpeedSlider.oninput = function() {
        smoothSpeedOutput.innerHTML = this.value;
    }

    var pacifica = document.getElementById("pacifica");
    pacifica.checked = %PACIFICA%;
    var fire = document.getElementById("fire");
    fire.checked = %FIRE%;
    var clapDetection = document.getElementById("clapDetection");
    clapDetection.checked = %CLAPDETECTION%;
    var clapHelper = document.getElementById("clapHelper");
    clapHelper.checked = %CLAPHELPER%;

    document.getElementById('smoothSpeedSliderDiv').style.display = document.getElementById('smoothTransition').checked ? "inline-block" : "none";
    document.getElementById('clapHelperDiv').style.display = document.getElementById('clapDetection').checked ? "inline-block" : "none";

    function checkDirection() {
      // if ((touchendX < touchstartX) && ((touchstartX - touchendX) > 50)) alert('swiped left! ' + (touchstartX - touchendX))
      // if ((touchendX > touchstartX) && ((touchendX - touchstartX) > 50)) alert('swiped right! ' + (touchendX - touchstartX))
      if (Math.abs(touchendX - touchstartX) > 50) switchUI();
    }

    document.addEventListener('touchstart', e => {
      touchstartX = e.changedTouches[0].screenX
    })

    document.addEventListener('touchend', e => {
      touchendX = e.changedTouches[0].screenX
      checkDirection()
    })

    function switchUI() {
        if (settingsDiv.style.display === "none") {
            switchButton.textContent = "Info panel";
            infoDiv.style.display = "none";
            settingsDiv.style.display = "inline-block";
        } else {
            switchButton.textContent = "Settings";
            settingsDiv.style.display = "none";
            infoDiv.style.display = "grid";
        }
    }

    function setStdColor() {
        digitHueSlider.value = stdColorSelect.value;
        digitHueOutput.innerHTML = stdColorSelect.value;
    }

    function postS7(fld, fldval) {
        const formData = new FormData();
        formData.set(fld, fldval);
        const payload = new URLSearchParams(formData);
        return fetch("http://s7.local/set", {
            method: "POST",
            body: payload,
        }).then(res => res.text());
    }

    function onClick_smoothTransition() {
      var checked = document.getElementById('smoothTransition').checked;
      postChange('smoothTransitionVal', checked);
      document.getElementById('smoothSpeedSliderDiv').style.display = checked ? "inline-block" : "none";
    }

    function onClick_clapDetection() {
      var checked = document.getElementById('clapDetection').checked;
      postChange('clapDetectionVal', checked);
      document.getElementById('clapHelperDiv').style.display = checked ? "block" : "none";
    }

    function onClick_beatDetection() {
      var checked = document.getElementById('beatDetection').checked;
      postChange('actionVal', 4 - checked); // 3 = AnimateBeats, 4 = Time
    }

    function postChange(fld, fldval) {
        topnavDiv.style.backgroundColor = "#f205bf";
        setTimeout(function() { 
          topnavDiv.style.backgroundColor = "#50B8B4";
        }, 2000);
        postS7(fld, fldval)
        .then(data => {
            console.log("Success:", data);
        })
        .catch(error => {
            console.error("Error:", error);
        });
    }    

    if (!!window.EventSource) {
        var source = new EventSource('/events');
        source.addEventListener('open', function(e) {
            console.log("Events Connected");
        }, false);
        source.addEventListener('error', function(e) {
          if (e.target.readyState != EventSource.OPEN) {
              console.log("Events Disconnected");
          }
        }, false);
        
        source.addEventListener('message', function(e) {
            console.log("message", e.data);
        }, false);
        
        source.addEventListener('time', function(e) {
            console.log("time", e.data);
            document.getElementById("time").innerHTML = e.data;
        }, false);
        
        source.addEventListener('ambientLight', function(e) {
            console.log("ambientLight", e.data);
            document.getElementById("ambientLight").innerHTML = e.data;
        }, false);
        
        source.addEventListener('brightness', function(e) {
            console.log("brightness", e.data);
            brightnessSlider.value = e.data;
            brightnessOutput.innerHTML = brightnessSlider.value;
        }, false);

        source.addEventListener('temperature', function(e) {
            console.log("temperature", e.data);
            document.getElementById("temperature").innerHTML = e.data;
        }, false);

        source.addEventListener('humidity', function(e) {
            console.log("humidity", e.data);
            document.getElementById("humidity").innerHTML = e.data;
        }, false);

        // source.addEventListener('fire', function(e) {                // can only be set from app
        //     console.log("fire", e.data);
        //     document.getElementById("fire").checked = e.data;
        // }, false);

        source.addEventListener('debug', function(e) {
            console.log("debug", e.data);
        }, false);

    }
</script>
</body>
</html>
)=="==";
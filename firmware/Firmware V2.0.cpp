#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

//////////////////// CONFIG ////////////////////
const char* AP_SSID = "Scoreboard-AP";
const char* AP_PASS = "12345678";
////////////////////////////////////////////////

ESP8266WebServer server(80);
DNSServer dns;
IPAddress apIP(192,168,4,1);


#pragma region //Region for the HTML Web-Interface
const char* INDEX_HTML PROGMEM = R"HTML(

<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width,initial-scale=1">
<!-- <meta http-equiv="refresh" content="3" > -->
<title>ScoreCore UI Test</title>

<!--
  Color Palette
  Background: #222831
  Primary:  #31363F
  Secondary:  
  Accent: #8A244B
  ON/Active: #2e7d32
  OFF/Inactive: #b71c1c
-->

<style>
body{
  font-family: Arial, sans-serif;
  color:rgb(255, 255, 255);
  background: #222831;
  margin:20px;
}

/* Row Styling */

.singlerow{
  display:grid;
  grid-template-columns:1fr;
  gap:12px;
}

.multirow{
    display:grid;
    grid-template-columns:1fr 1fr;
    gap:12px
}

/* Grid Layout */
.grid{
  display:grid;
  grid-template-columns:repeat(auto-fit,minmax(220px,1fr));
  gap:15px;
}

/* Card Styling */
.card{
  background:#31363F;
  border-radius:14px;
  padding:18px;
  box-shadow:0 4px 12px rgba(0,0,0,.1);
}

.label{
  font-size:14px;
  opacity:.6;
  margin-bottom:12px;
}

.formRow{
  display:grid;
  grid-template-columns: 1fr 120px;
  align-items:center;
  gap:14px;

  padding:10px 14px;
  margin-top:12px;

  border-radius:12px;
  background: rgba(255,255,255,0.03);
}

.formRow label{
  font-size:14px;
  opacity:.8;
}

.formRow input[type="number"]{
  width:100% - 8px;
  padding:10px 12px;

  border-radius:10px;
  border:1px solid rgba(255,255,255,0.08);

  background: rgba(0,0,0,0.25);
  color:#fff;

  font-size:16px;
  text-align:center;
  outline:none;
  transition:.15s;
}

.formRow input[type="number"]:focus{
  border-color:#2e7d32;
  box-shadow:0 0 0 3px rgba(46,125,50,.25);
}

/* Button Styling */
button{
  background: #ffffff;
  width:100%;
  padding:14px;
  border:0;
  border-radius:12px;
  font-size:18px;
  cursor:pointer;
  transition:.15s;
}

/*Toggle Button State Colors */
.on{
  background:#2e7d32;
  color:#ffffff;
}

.off{
  background:#b71c1c;
  color:#fff;
}

/*Momentary Button State Colors */
.inactive{
  background:#393E46;
  color:#fff;
}

.active{
  background:#2e7d32;
  color:#fff;
}

button {
  transition: transform .08s ease, box-shadow .08s ease;
}

button:active {
  transform: scale(.98);
  box-shadow: inset 0 3px 6px rgba(0, 0, 0, 0.25);
}
</style>

<script> // Toggle Script

/*
  UNIVERSAL TOGGLE FUNCTION
  - funktioniert für alle Buttons
  - speichert Status im data-state Attribut
*/

function toggle(btn, commandName){

  // aktuellen Status lesen (default = 0)
  let state = btn.dataset.state === "1" ? 1 : 0;

  // umschalten
  state ^= 1;

  // neuen Status speichern
  btn.dataset.state = state;

  // Farbe + Text ändern
  if(state){
    btn.classList.remove("off");
    btn.classList.add("on");
    btn.innerText = "ON";
  } else {
    btn.classList.remove("on");
    btn.classList.add("off");
    btn.innerText = "OFF";
  }

  console.log(commandName + ":", state);

  // OPTIONAL: später für ESP
  // send(commandName + "=" + state);
}


/* Dummy send Funktion */
async function send(q){
  console.log("Sending to ESP:", q);
  // await fetch('/cmd?'+q);
}

</script>

<script> // Momentary Script
function press(btn, cmd){
  btn.classList.remove("inactive");
  btn.classList.add("active");

  console.log(cmd, "pressed");
  // send(cmd + "=1");   // später für ESP
}

function release(btn, cmd){
  btn.classList.remove("active");
  btn.classList.add("inactive");

  console.log(cmd, "released");
  // send(cmd + "=0");   // später für ESP
}
</script>

<script> // Counter Logic
function setScoreText(side,v){ document.getElementById(side==='H'?'scH':'scG').textContent = v; }
async function setScore(side,v,btn){ //Reset Logic
  if (v<0) v=0; setScoreText(side,v);
  const ok = await send(side==='H' ? ('SCORE_H='+v) : ('SCORE_G='+v));
  flashBtn(btn, ok);
}
function bump(side,delta,btn){ //CTU CTD Logic
  const el=document.getElementById(side==='H'?'scH':'scG');
  let v=parseInt(el.textContent||'0',10)+delta; if (v<0) v=0; setScore(side,v,btn);
}
</script>

<script> // Countdown controls

async function startCountdown(btn){
  const t=document.getElementById('when').value || '15:30';
  const ok = await send('CD_START='+encodeURIComponent(t));
  flashBtn(btn, ok);
  setStatus('Countdown gestartet: '+t, null);
}
async function stopCountdown(btn){
  const ok = await send('CD_STOP=1');
  flashBtn(btn, ok);
  setStatus('Countdown gestoppt', null);
}

</script>

<script> // Game Time

  async function pressHold(name,on,btn){
  const ok = await send(name+'='+(on?1:0));
  flashBtn(btn, ok);
  const id = name==='ALLSEG' ? 'btnAll' : 'btnTest';
  setBtn(id,'An','Gedrückt halten',on);
}

// ---- Spieluhr controls ----
async function gmStart(btn){
  const ok = await send('GM_START=1'); flashBtn(btn, ok);
  setStatus('Spieluhr gestartet (1. Halbzeit)', null);
}
async function gmResume(btn){
  const ok = await send('GM_RESUME=1'); flashBtn(btn, ok);
  setStatus('Spieluhr fortgesetzt', null);
}
async function gmStop(btn){
  const ok = await send('GM_STOP=1'); flashBtn(btn, ok);
  setStatus('Spieluhr gestoppt', null);
}
async function gmCfg(btn){
  const h1 = parseInt(document.getElementById('half1').value||'45',10);
  const fu = parseInt(document.getElementById('full').value||'90',10);
  const ok = await send('GM_CFG_HALF1='+h1+'&GM_CFG_FULL='+fu);
  flashBtn(btn, ok);
  setStatus('Spieluhr konfiguriert: '+h1+' / '+fu+' Min', null);
}

</script>



</head>
<body>

<h1>ScoreCore UI Test</h1>

<div class="singlerow" style="margin-top:12px"> <!-- Panel power switch -->

      <div class="card">
        <div class="label">Anzeige An</div>
        <button class="off"
               data-state="0"
                onclick="toggle(this,'PWR')">
          OFF
        </button>
      </div>

</div>

<div class="multirow" style="margin-top:12px"> <!-- Debug Buttons -->

      <div class="card">
         <div class="label">Testmodus (Taster)</div>
            <button class="inactive"
                    onpointerdown="press(this,'HORN')"
                    onpointerup="release(this,'HORN')"
                    onpointercancel="release(this,'HORN')"
                    onpointerleave="release(this,'HORN')">
              Test
            </button>
      </div>

      <div class="card">
         <div class="label">Alle Segmente an (Taster)</div>
            <button class="inactive"
                    onpointerdown="press(this,'HORN')"
                    onpointerup="release(this,'HORN')"
                    onpointercancel="release(this,'HORN')"
                    onpointerleave="release(this,'HORN')">
              Test
            </button>
      </div> 

</div>

<div class="multirow" style="margin-top:12px"> <!-- Score Buttons -->

    <div class="card"> <!-- Home Score-->
      <div class="label">Heim</div>
      <div id=scH class=score>0</div>
      <div class=bar>
        <button class=on onclick="bump('H',+1,this)">+ 1</button>
        <button class=off onclick="bump('H',-1,this)">− 1</button>
        <button class=inactive onclick="setScore('H',0,this)">Reset</button>
      </div>
    </div>

    <div class="card"> <!-- Guest Score-->
      <div class="label">Gast</div>
      <div id=scG class=score>0</div>
      <div class=bar>
        <button class=on onclick="bump('G',+ 1,this)">+ 1</button>
        <button class=off onclick="bump('G',- 1,this)">− 1</button>
        <button class=inactive onclick="setScore('G',0,this)">Reset</button>
      </div>
    </div>
  </div>

<div class="card" style="margin-top:12px"> <!-- Countdown Controlls -->

    <div class="label">Spielbeginn</div>
    <div class="grid3">
      <span>Zeit</span>
      <input id=when type=time value="15:30" step="60">
      <button class=on onclick="startCD(this)">Start</button>
      <button class=off onclick="stopCD(this)">Stop</button>
    </div>
    <div class=tiny>Info: Der Countdown zählt bis 5 Minuten vor Spielbeginn</div>

</div>

<div class="card" style="margin-top:12px"> <!-- Game Time Controlls-->
    <div class="label">Spieluhr</div>
    <div class="multirow">
      <button class=on onclick="gmStart(this)">Spiel starten</button>
      <button class=inactive onclick="gmResume(this)">Fortsetzen</button>
    </div>
    <div class="multirow" style="margin-top:8px">
      <button class=off onclick="gmStop(this)">Stop</button>
      <button class=inactive onclick="gmCfg(this)">Übernehmen</button>
      <div></div>
    </div>
    <div class="formRow">
      <label for="halfTime">Halbzeit (Min)</label>
      <input id="halfTime" type="number" min="0" value="45">
    </div>

    <div class="formRow">
      <label for="fullTime">Spielende (Min)</label>
      <input id="fullTime" type="number" min="0" value="90">
    </div>

    <div class=tiny>Automatische Pause bei „Halbzeit“, Ende bei „Spielende“.</div>
</div>

</body>
</html>

)HTML";
#pragma endregion

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255,255,255,0));
  WiFi.softAP(AP_SSID, AP_PASS);


  dns.start(53, "*", apIP);


  server.on("/", [] ()  {
    server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
  });

  server.onNotFound( [] ()  {
    server.send(404, "text/plain", " Oops da ist etwas schiefgelaufen ");
  });

  server.begin();
}

void loop() {

  server.handleClient();

}

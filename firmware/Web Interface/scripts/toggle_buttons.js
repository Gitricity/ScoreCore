
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


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


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

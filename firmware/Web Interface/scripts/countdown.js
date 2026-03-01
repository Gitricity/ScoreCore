
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

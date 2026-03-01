
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

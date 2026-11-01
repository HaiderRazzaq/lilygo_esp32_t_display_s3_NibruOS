import test from 'node:test';
import assert from 'node:assert/strict';

async function withUI(serial, body) {
  const ids={};
  for (const id of ['mode','status','wifiFields','name','secret','pairing','ip','reveal','close','send']) {
    ids[id]={value:'',type:'password',textContent:'',disabled:false,handlers:{},addEventListener(event,fn){this.handlers[event]=fn;}};
  }
  ids.mode.value='usb';ids.name.value='Fixture';ids.secret.value='MY';ids.pairing.value='ABCDEFGHIJKLMNOP';
  const oldDocument=globalThis.document, oldNavigator=Object.getOwnPropertyDescriptor(globalThis,'navigator');
  globalThis.document={getElementById:id=>ids[id],querySelector:()=>({content:'fixture-token'})};
  Object.defineProperty(globalThis,'navigator',{configurable:true,value:{serial}});
  try {
    await import('../../tools/account_setup/account_setup.mjs?ui='+Math.random());
    await body(ids);
  } finally {
    if(oldDocument===undefined) delete globalThis.document; else globalThis.document=oldDocument;
    if(oldNavigator) Object.defineProperty(globalThis,'navigator',oldNavigator); else delete globalThis.navigator;
  }
}
test('busy USB open can retry without page reload',async()=>{
  let requests=0,opens=0,writes=0;
  await withUI({requestPort:async()=>{requests++;return {
    open:async()=>{if(++opens===1)throw new Error('Port busy');},
    writable:{getWriter:()=>({write:async()=>{writes++;},releaseLock(){}})},close:async()=>{}
  };}},async ids=>{
    await ids.send.handlers.click();
    assert.match(ids.status.textContent,/Port busy/);
    await ids.send.handlers.click();
    assert.equal(requests,2);assert.equal(opens,2);assert.equal(writes,1);
    assert.equal(ids.secret.value,'');assert.equal(ids.pairing.value,'');
  });
});
test('clear fields still clears when disconnected close rejects',async()=>{
  await withUI({requestPort:async()=>({open:async()=>{},writable:{getWriter:()=>({write:async()=>{},releaseLock(){}})},close:async()=>{throw new Error('Disconnected');}})},async ids=>{
    await ids.send.handlers.click();
    ids.secret.value='MY';ids.pairing.value='ABCDEFGHIJKLMNOP';
    await ids.close.handlers.click();
    assert.equal(ids.secret.value,'');assert.equal(ids.pairing.value,'');
  });
});

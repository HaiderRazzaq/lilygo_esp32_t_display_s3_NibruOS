import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';
import {parseAccount, normalizeSecret, makeFrame} from '../../tools/account_setup/account_setup.mjs';

const secret = 'GEZDGNBVGY3TQOJQGEZDGNBVGY3TQOJQ'; // RFC public fixture.
test('pasted key canonicalization and URI metadata', () => {
  assert.deepEqual(parseAccount('Fixture', secret.toLowerCase()), {name:'Fixture',secret,digits:6,period:30});
  const account = parseAccount('Work', 'otpauth://totp/Issuer%3Auser?secret=' + secret + '&digits=8&period=60');
  assert.equal(account.digits,8); assert.equal(account.period,60);
  assert.equal(normalizeSecret('MY======'),'MY');
});
test('no silent algorithm or counter downgrade', () => {
  for (const uri of ['otpauth://hotp/Work?secret='+secret, 'otpauth://totp/Work?secret='+secret+'&algorithm=SHA256',
      'otpauth://totp/Work?secret='+secret+'&digits=7', 'otpauth://totp/Work?secret='+secret+'&period=15',
      'otpauth://totp/Work?secret='+secret+'&secret=MY']) assert.throws(()=>parseAccount('Work',uri));
  for (const value of ['123456','A','MZ','MY=XX','A'.repeat(80)]) assert.throws(()=>normalizeSecret(value));
  assert.throws(()=>parseAccount('Work\nInjected',secret));
});
test('WebCrypto matches independent Windows CNG envelope', async () => {
  const nonce=new Uint8Array(12); nonce[0]=19;
  const frame=await makeFrame(parseAccount('Fixture',secret),'ABCDEFGHIJKLMNOP',nonce);
  const expected=fs.readFileSync('.testbuild/import-vector.txt','utf8');
  assert.equal(frame,expected);
  assert.equal(frame.includes(secret),false);
});

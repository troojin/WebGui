'use strict';

const send = action => window.chrome.webview.postMessage(JSON.stringify(action));
document.querySelector('#wireframe').addEventListener('change', event => send({ wireframe: event.target.checked }));
document.querySelector('#pause').addEventListener('change', event => send({ paused: event.target.checked }));
document.querySelector('#inspect').addEventListener('click', () => send({ action: 'inspect' }));
window.chrome.webview.addEventListener('message', event => {
    document.querySelector('#reply').textContent = event.data;
});

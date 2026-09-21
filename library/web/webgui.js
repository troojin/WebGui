'use strict';

// Small optional bridge for pages loaded by WebGUI.
// Include this file from your page with a relative copy, or call WebGUI directly.
window.WebGUI = {
    send(message) {
        window.chrome.webview.postMessage(typeof message === 'string' ? message : JSON.stringify(message));
    },
    onMessage(handler) {
        window.chrome.webview.addEventListener('message', event => handler(event.data));
    }
};

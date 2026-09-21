const reply = document.querySelector('#reply');
document.querySelector('#send').addEventListener('click', () => {
    window.chrome.webview.postMessage('Hello from JavaScript');
});
window.chrome.webview.addEventListener('message', event => {
    reply.textContent = event.data;
});

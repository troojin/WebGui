'use strict';

const root = document.getElementById('root');
let lastJson = '';

window.chrome.webview.addEventListener('message', event => {
    // skip dom work when nothing has changed
    if (event.data === lastJson) return;
    lastJson = event.data;

    let commands;
    try {
        commands = JSON.parse(event.data);
    } catch {
        return;
    }

    render(commands);
});

function render(commands) {
    const active = document.activeElement;
    const activeId = active?.dataset?.pwId;
    const selectionStart = active?.selectionStart;
    const selectionEnd = active?.selectionEnd;
    root.textContent = ''; // clears children without touching the root element itself

    for (const cmd of commands) {
        switch (cmd.type) {
            case 'begin':
                document.title = cmd.title;
                break;

            case 'text': {
                const el = document.createElement('span');
                el.className = 'pw-text';
                el.textContent = cmd.text;
                root.appendChild(el);
                break;
            }

            case 'button': {
                const el = document.createElement('button');
                el.className = 'pw-button';
                el.textContent = cmd.label;
                el.addEventListener('click', () => {
                    window.chrome.webview.postMessage(
                        JSON.stringify({ type: 'click', id: cmd.id })
                    );
                });
                root.appendChild(el);
                break;
            }

            case 'checkbox': {
                const label = document.createElement('label');
                label.className = 'pw-checkbox';
                const input = document.createElement('input');
                input.type = 'checkbox';
                input.checked = cmd.checked;
                input.addEventListener('change', () => {
                    window.chrome.webview.postMessage(
                        JSON.stringify({ type: 'checkbox', id: cmd.id, value: input.checked })
                    );
                });
                label.append(input, document.createTextNode(cmd.label));
                root.appendChild(label);
                break;
            }

            case 'input': {
                const label = document.createElement('label');
                label.className = 'pw-input';
                label.append(document.createTextNode(cmd.label));
                const input = document.createElement('input');
                input.type = 'text';
                input.dataset.pwId = cmd.id;
                input.value = cmd.value;
                input.addEventListener('input', () => {
                    window.chrome.webview.postMessage(
                        JSON.stringify({ type: 'input', id: cmd.id, value: input.value })
                    );
                });
                label.appendChild(input);
                root.appendChild(label);
                break;
            }

            case 'separator': {
                const el = document.createElement('hr');
                el.className = 'pw-separator';
                root.appendChild(el);
                break;
            }

            case 'end':
                break;
        }
    }

    if (activeId) {
        const input = root.querySelector(`[data-pw-id="${CSS.escape(activeId)}"]`);
        if (input) {
            input.focus();
            input.setSelectionRange(selectionStart, selectionEnd);
        }
    }
}

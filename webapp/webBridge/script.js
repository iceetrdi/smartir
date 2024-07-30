let port;
let reader;
let inputDone;
let outputDone;
let inputStream;
let outputStream;

const mqttBrokerInput = document.getElementById("mqttBroker");
const connectMQTTButton = document.getElementById("connectMQTT");
const disconnectMQTTButton = document.getElementById("disconnectMQTT");
const connectSerialButton = document.getElementById("connectSerial");
const disconnectSerialButton = document.getElementById("disconnectSerial");
const sendSerialButton = document.getElementById("sendSerial");
const serialInput = document.getElementById("serialInput");
const receiveLog = document.getElementById("receiveLog");
const clearReceiveLogButton = document.getElementById("clearReceiveLog");
const sendLog = document.getElementById("sendLog");
const clearSendLogButton = document.getElementById("clearSendLog");
const mqttStatus = document.getElementById("mqttStatus");
const serialStatus = document.getElementById("serialStatus");
const mqttTopicInput = document.getElementById("mqttTopic");

let client;

document.addEventListener("DOMContentLoaded", function () {
  mqttTopicInput.value = Math.floor(100000 + Math.random() * 900000).toString();
});

connectMQTTButton.addEventListener("click", () => {
  const brokerURL = mqttBrokerInput.value;
  if (client) {
    if (client.connected) {
      client.end();
    } else {
      client.removeAllListeners();
      client = null;
    }
  }

  client = mqtt.connect(brokerURL);

  client.on("connect", () => {
    console.log("Connected to MQTT broker");
    updateStatusBadge(mqttStatus, "connected");
    client.subscribe(mqttTopicInput.value + "/rx", (err) => {
      if (!err) {
        console.log("Subscribed to rx topic");
      }
    });
  });

  client.on("error", (err) => {
    console.error("Failed to connect to MQTT broker", err);
  });

  client.on("close", () => {
    console.log("Disconnected from MQTT broker");
    updateStatusBadge(mqttStatus, "disconnected");
    if (client) {
      client.removeAllListeners();
      client = null;
    }
  });

  client.on("message", async (topic, message) => {
    if (topic === mqttTopicInput.value + "/rx") {
      const data = new TextDecoder().decode(message) + "\n";
      sendLog.value += data;
      await sendSerialData(data);
    }
  });
});

disconnectMQTTButton.addEventListener("click", () => {
  if (client) {
    client.end();
    updateStatusBadge(mqttStatus, "disconnected");
  }
});

connectSerialButton.addEventListener("click", async () => {
  await connectSerial();
});

disconnectSerialButton.addEventListener("click", async () => {
  await disconnectSerial();
});

sendSerialButton.addEventListener("click", async () => {
  const data = serialInput.value + "\n";
  sendLog.value += data;
  await sendSerialData(data);
});

clearSendLogButton.addEventListener("click", () => {
  document.getElementById("sendLog").value = "";
});

clearReceiveLogButton.addEventListener("click", () => {
  document.getElementById("receiveLog").value = "";
});

async function connectSerial() {
  port = await navigator.serial.requestPort();
  await port.open({ baudRate: 9600 });

  updateStatusBadge(serialStatus, "connected");

  const encoder = new TextEncoderStream();
  outputDone = encoder.readable.pipeTo(port.writable);
  outputStream = encoder.writable;

  const decoder = new TextDecoderStream();
  inputDone = port.readable.pipeTo(decoder.writable);
  inputStream = decoder.readable;

  reader = inputStream.getReader();
  readSerialData();
}

async function disconnectSerial() {
  if (reader) {
    await reader.cancel();
    await inputDone.catch(() => {
      /* Ignore the error */
    });
    reader = null;
    inputDone = null;
  }
  if (outputStream) {
    await outputStream.getWriter().close();
    await outputDone.catch(() => {
      /* Ignore the error */
    });
    outputStream = null;
    outputDone = null;
  }
  if (port) {
    await port.close();
    port = null;
  }

  updateStatusBadge(serialStatus, "disconnected");
}

async function sendSerialData(data) {
  const writer = outputStream.getWriter();
  await writer.write(data);
  writer.releaseLock();
}

async function readSerialData() {
  let buffer = "";

  while (true) {
    const { value, done } = await reader.read();
    if (done) {
      reader.releaseLock();
      break;
    }

    buffer += value;
    if (buffer.includes("\n")) {
      const lines = buffer.split("\n");
      for (let i = 0; i < lines.length - 1; i++) {
        const line = lines[i] + "\n";
        receiveLog.value += line;
        if (client && client.connected) {
          client.publish(mqttTopicInput.value + "/tx", line);
        }
      }
      buffer = lines[lines.length - 1];
    }
  }
}

function updateStatusBadge(badge, status) {
  badge.classList.remove("disconnected");
  badge.classList.remove("connectionfailed");
  badge.classList.remove("connected");
  badge.classList.remove("connecting");
  switch (status) {
    case "connected":
      badge.textContent = "接続";
      badge.classList.add("connected");
      break;
    case "disconnected":
      badge.classList.add("disconnected");
      badge.textContent = "未接続";
      break;
    case "connected":
      badge.classList.add("connectionfailed");
      badge.textContent = "接続失敗";
      break;
    case "connecting":
      badge.classList.add("connecting");
      badge.textContent = "接続中";
      break;
  }
}

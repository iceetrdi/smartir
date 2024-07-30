let port;
let reader;
let inputDone;
let outputDone;
let inputStream;
let outputStream;

const mqttStatus = document.getElementById('mqttStatus');
const mqttBrokerInput = document.getElementById('mqttBroker');
const mqttTopicInput = document.getElementById("mqttTopic");
const connectMQTTButton = document.getElementById('connectMQTT');
const disconnectMQTTButton = document.getElementById('disconnectMQTT');
const sendMessage = document.getElementById('sendMessage');
const sendMQTTButton = document.getElementById('sendMessageButton');
const receiveLog = document.getElementById('receiveLog');
const clearReceiveLogButton = document.getElementById('clearReceiveLog');
const sendLog = document.getElementById('sendLog');
const clearSendLogButton = document.getElementById('clearSendLog');

let client;

connectMQTTButton.addEventListener('click', () => {
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

    client.on('connect', () => {
        console.log('Connected to MQTT broker');
        mqttStatus.classList.remove('disconnected');
        mqttStatus.classList.remove('connectionfailed');
        mqttStatus.classList.add('connected');
        mqttStatus.textContent = '接続';
        client.subscribe(mqttTopicInput.value + '/tx', (err) => {
            if (!err) {
                console.log('Subscribed to tx topic');
            }
        });
    });

    client.on('error', (err) => {
        console.error('Failed to connect to MQTT broker', err);
        mqttStatus.classList.remove('connected');
        mqttStatus.classList.remove('disconnected');
        mqttStatus.classList.add('connectionfailed');
        mqttStatus.textContent = '接続失敗';
        client.end(true);
        client.removeAllListeners();
        client = null;
    });

    client.on('close', () => {
        console.log('Disconnected from MQTT broker');
        mqttStatus.classList.remove('connected');
        mqttStatus.classList.remove('connectionfailed');
        mqttStatus.classList.add('disconnected');
        mqttStatus.textContent = '未接続';
        if (client) {
            client.removeAllListeners();
            client = null;
        }
    });

    client.on('message', async (topic, message) => {
        if (topic === mqttTopicInput.value + '/tx') {
            const data = new TextDecoder().decode(message).replace(/\r?\n/g, '') + '\n';
            receiveLog.value += data;
        }
    });
});


disconnectMQTTButton.addEventListener('click', () => {
    if (client) {
        client.end();
        mqttStatus.classList.remove('connected');
        mqttStatus.classList.add('disconnected');
        mqttStatus.textContent = '未接続';
    }
});

sendMQTTButton.addEventListener('click', () => {
    client.publish(mqttTopicInput.value + '/rx', sendMessage.value.replace(/\r?\n/g, ''));
    sendLog.value += sendMessage.value + '\n';
});

clearSendLogButton.addEventListener('click', () => {
  document.getElementById('sendLog').value = '';
});

clearReceiveLogButton.addEventListener('click', () => {
  document.getElementById('receiveLog').value = '';
});

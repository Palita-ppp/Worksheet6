const express = require('express');
const mqtt = require('mqtt');

const app = express();
const port = 4000;
const mqttBrokerUrl = 'mqtt://172.20.10.2'; // Replace with your MQTT broker address
const ledTopic = 'LED';

// Connecting to MQTT broker
const mqttClient = mqtt.connect(mqttBrokerUrl);
mqttClient.on('connect', () => console.log('Connected to MQTT broker'));
mqttClient.on('error', (error) => console.error('MQTT connection error:', error));

// Serving static files from 'public' directory
app.use(express.static('public'));

// Main route
app.get('/', (req, res) => {
  res.sendFile(`${__dirname}/index.html`);
});

// Route for toggling LED
app.get('/toggle', (req, res) => {
  const ledState = req.query.state === '1' ? 'On' : 'Off';
  
  mqttClient.publish(ledTopic, ledState, (error) => {
    if (error) {
      console.error('MQTT publish error:', error);
      return res.status(500).send('Error toggling LED');
    }
    res.send(`LED is ${ledState}`);
  });
});

// Starting the server
app.listen(port, () => console.log(`Server running at http://localhost:${port}`));

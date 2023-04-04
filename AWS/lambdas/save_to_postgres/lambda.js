const { Client } = require("pg");

// Connect to the PostgreSQL database
const client = new Client({
  user: "your_user",
  host: "your_host",
  database: "your_database",
  password: "your_password",
  port: your_port,
});

// AWS Lambda handler
exports.handler = async (event) => {
  // Parse the incoming MQTT message
  const message = JSON.parse(event.message);
  const {
    temperature,
    pump_state: pumpState,
    heater_state: heaterState,
    timestamp,
  } = message;

  try {
    await client.connect();

    // Retrieve the last state and timestamp from the database
    const lastStateResult = await client.query(
      "SELECT pump_state, heater_state, timestamp FROM your_table_name WHERE id = 1;"
    );
    const {
      pump_state: lastPumpState,
      heater_state: lastHeaterState,
      timestamp: lastTimestamp,
    } = lastStateResult.rows[0];

    // Calculate the time duration that the devices have been in their previous state
    const duration = (timestamp - lastTimestamp) / 3600; // Convert seconds to hours

    // Update the database with the new state and timestamp
    await client.query(
      "UPDATE your_table_name SET pump_state = $1, heater_state = $2, timestamp = $3 WHERE id = 1;",
      [pumpState, heaterState, timestamp]
    );

    // Calculate energy consumption based on state changes
    const pumpEnergy = (pumpState === "ON" ? 3200 : 0) * duration;
    const heaterEnergy = (heaterState === "ON" ? 2300 : 0) * duration;

    // Store the energy consumption in the energy_usage table
    await client.query(
      "INSERT INTO energy_usage (device_type, energy_consumption, timestamp) VALUES ($1, $2, $3), ($4, $5, $6);",
      ["pump", pumpEnergy, timestamp, "heater", heaterEnergy, timestamp]
    );

    // Store device state changes if there's a change
    if (pumpState !== lastPumpState) {
      await client.query(
        "INSERT INTO device_state_changes (device_type, state, timestamp) VALUES ($1, $2, $3);",
        ["pump", pumpState, timestamp]
      );
    }

    if (heaterState !== lastHeaterState) {
      await client.query(
        "INSERT INTO device_state_changes (device_type, state, timestamp) VALUES ($1, $2, $3);",
        ["heater", heaterState, timestamp]
      );
    }

    await client.end();
    return {
      statusCode: 200,
      body: "State, energy consumption, and state changes updated successfully.",
    };
  } catch (error) {
    console.error(error);
    await client.end();
    return {
      statusCode: 500,
      body: "Error updating state, energy consumption, and state changes.",
    };
  }
};

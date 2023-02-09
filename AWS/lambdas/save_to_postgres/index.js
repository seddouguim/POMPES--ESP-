const { Client } = require("pg");

exports.handler = async (event, context) => {
  // Connect to the PostgreSQL database
  const client = new Client({
    host: "iot-db-test.cwdqx89aqat4.us-east-1.rds.amazonaws.com",
    user: "postgres",
    password: "medsed00",
    database: "data",
    port: "5432",
  });

  await client.connect();

  // Extract the data from the incoming message
  let str = JSON.stringify(event, null, 2);
  let payload = JSON.parse(str);

  // Insert the data into the PostgreSQL database
  await client.query(
    "INSERT INTO sensor_data(device_id, temperature, pump_state, resistance_state, pump_kwh, resistance_kwh, pump_on_time, resistance_on_time) VALUES ($1, $2, $3, $4, $5, $6, $7, $8)",
    [
      payload.device_id,
      payload.current_temperature,
      payload.pump_state,
      payload.resistance_state,
      payload.pump_kwh,
      payload.resistance_kwh,
      payload.pump_on_time_total,
      payload.resistance_on_time_total,
    ]
  );

  // Disconnect from the PostgreSQL database
  await client.end();

  // Return a success message
  return {
    statusCode: 200,
    body: "Data saved to PostgreSQL database",
  };
};

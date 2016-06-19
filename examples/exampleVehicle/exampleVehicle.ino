#include <SCoop.h>

// --------------------------------------------------------------------
// Vehicle state
// --------------------------------------------------------------------
#include <VehicleDefs.h>
#include <Vehicle.h>
Vehicle vehicle;

// --------------------------------------------------------------------
// CAN bus interface
// --------------------------------------------------------------------
#define CAN_MSG_ID    0x07

#include <SPI.h>
#include <mcp_can.h>

#define CAN_CS_PIN    4
#define CAN_INT_PIN   2
MCP_CAN CAN(CAN_CS_PIN);

#define CAN_TX_QUEUE_LEN  8
defineFifo(canTxQueue, uint32_t, CAN_TX_QUEUE_LEN);

#define CAN_RX_QUEUE_LEN  8
defineFifo(canRxQueue, uint32_t, CAN_RX_QUEUE_LEN);

// Flag used to indicate that there is something to read
// on the CAN bus
volatile bool canRxSema = false;

// Used to convert messages between various integer formats
#define CAN_TYPE_SWITCH   0x00
#define CAN_TYPE_VALUE    0x01
union CanFrame
{
   uint8_t data[4];
   uint32_t full;
   struct {
      uint8_t type;
      uint8_t index;
      uint16_t val;
   } parts;
};
CanFrame buf;

/*
 * Sets up the interrupt to trigger reading incoming CAN messages.
 */
void setupCanbus()
{
  // Interrupt on pin D2 triggere ISR
  attachInterrupt(
    digitalPinToInterrupt(CAN_INT_PIN),
    canInterrupt,
    LOW
  );
  // Start the CAN bus at 100Kbps
  CAN.begin(CAN_100KBPS);
}

/*
 * Add a switch type CAN message to the transmit queue
 */
void queueCanSwitch(uint8_t index)
{
  buf.parts.type = CAN_TYPE_SWITCH;
  buf.parts.index = index;
  buf.parts.val = (uint16_t)vehicle.switches[index];
  canTxQueue.put(&(buf.full));
}

/*
 * Add a value type CAN message to the transmit queue
 */
void queueCanValue(uint8_t index)
{
  buf.parts.type = CAN_TYPE_VALUE;
  buf.parts.index = index;
  buf.parts.val = vehicle.values[index];
  canTxQueue.put(&(buf.full));
}

// --------------------------------------------------------------------
// ISRs
// --------------------------------------------------------------------

/*
 * ISR:  Pin change on pin D2 (INT0) means CAN interrupt. Signal
 * the CAN receive thread to process it.
 */
void canInterrupt()
{
  // Tell the read task to read.
  canRxSema = true;
}

// --------------------------------------------------------------------
// Tasks
// --------------------------------------------------------------------

/*
 * TASK:  When there's a CAN message waiting to be read, read it
 * and add it to the receieve queue.
 */
uint8_t len;
defineTaskLoop(readCanMessage)
{
  // Wait until the semaphore is set
  // (ie. when the interrupt handler has set it)
  sleepUntil(canRxSema);
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    CAN.readMsgBuf(&len, buf.data);
    canRxQueue.put(&(buf.full));
  }
}

/*
 * TASK:  When there's a message in the CAN receive buffer, process it.
 */
defineTaskLoop(processCanRxQueue)
{
  if (canRxQueue.get(&(buf.full))) {
    // Set the new state on the vehicle state
    switch (buf.parts.type) {
       case 0:
           // Don't send a can message if we changed state
           vehicle.setSwitch(buf.parts.index, buf.parts.val);
       break;
       case 1:
           // Don't send a can message if we changed state
           vehicle.setValue(buf.parts.index, buf.parts.val);
       break;
    }
  }
}

/*
 * TASK:  When there's a message in the CAN transmit buffer, send it.
 */
defineTaskLoop(processCanTxQueue)
{
  if (canTxQueue.get(&(buf.full))) {
    CAN.sendMsgBuf(CAN_MSG_ID, 0, sizeof(buf.data), buf.data);
  }
}

// Add more tasks here to handle I/O and processing

// --------------------------------------------------------------------
// Arduino setup
// --------------------------------------------------------------------

void setup()
{
  // Set up inputs / outputs / attach interrupts here

  setupCanbus();

  mySCoop.start();
}

void loop()
{
  // Empty
}

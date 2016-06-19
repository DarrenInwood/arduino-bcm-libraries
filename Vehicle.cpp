#include "Arduino.h"
#include "Vehicle.h"

// Set state of a switch
// send a CAN message if it's changed
void Vehicle::setSwitch(uint8_t index, bool state)
{
    if (switches[index] == state) {
        return; // no change
    }
    switches[index] = state;
}

// Set state of a value
// Send a CAN message if it's changed
void Vehicle::setValue(uint8_t index, uint16_t val)
{
    if (values[index] == val) {
        return; // no change
    }
    values[index] = val;
}



// // Sends a CAN message if there are any in the queue
// void Vehicle::sendCanMessage()
// {
//     if (canTxQueueLength == 0) {
//         // nothing to send
//         return;
//     }
//     union CanFrame buf;
//     canTxQueueLength--;
//     buf.full = canTxQueue[canTxQueueLength];
//     CAN.sendMsgBuf(CAN_MSG_ID, 0, 4, buf.data);
// }
//
// void Vehicle::receiveCanMessage()
// {
//     if (CAN_MSGAVAIL != CAN.checkReceive()) {
//         return; // nothing to receive
//     }
//     union CanFrame buf;
//     uint8_t len;
//     CAN.readMsgBuf(&len, buf.data);
//     // Set the new value according to the incoming message
//     switch (buf.parts.type) {
//         case 0:
//             // Don't send a can message if we changed state
//             setSwitch(buf.parts.index, buf.parts.val, false);
//         break;
//         case 1:
//             // Don't send a can message if we changed state
//             setValue(buf.parts.index, buf.parts.val, false);
//         break;
//     }
// }

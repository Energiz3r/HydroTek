import { store } from '../../stores/store';
import { socket } from './client';
import { maxTimestamp, maxMessageLength } from '../../config';
import {
  setMessageSent
} from '../../actions/actions';
import log from '../log'

//send chat messages to the server and handle the response on success or failure
export const sendChatMessage = (outboundMsg) => {

  let wasError = false;
  let errorMsg = '';
  //sanity check the timestamp
  if (isNaN(outboundMsg.sentTimestamp) || outboundMsg.sentTimestamp < 1528932507000 || outboundMsg.sentTimestamp > maxTimestamp) {
    wasError = true;
    errorMsg = 'malformed timestamp';
  };
  //sanity check the message length
  if (outboundMsg.messageText.length > maxMessageLength || outboundMsg.messageText.length < 1) {
    wasError = true;
    errorMsg = 'length out of bounds';
  }
  //if there was an error
  if (wasError) {
    store.dispatch(setMessageSent(outboundMsg.sentTimestamp, errorMsg)); //display the message in the client but show a not-sent error on it
  }
  //if there wasn't an error, send the message
  else {
    socket.emit('chat message', outboundMsg, ({ timestamp, response }) => {
      // handle the response
      if (response == "success") {
        log("Message " + timestamp + " sent successfully!");
        store.dispatch(setMessageSent(timestamp, "success"));
      } else {
        log("Message " + timestamp + " not sent: " + response);
        store.dispatch(setMessageSent(timestamp, response));
      }
    });
  }
}

export default sendChatMessage;
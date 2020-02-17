import { maxNickLength, minNickLength } from '../../config';
import { store } from '../../stores/store';
import { socket } from './client';
import {
  setNickSetFailedReason
} from '../../actions/actions';
import log from '../log'

//send the request to set the user's nickname, and handle the response on success or failure
const setNick = (responseHandler) => {
  
  const nick = store.getState().loginState.nick;
  let wasError = false;
  let errorMsg = '';
  //sanity check the nick length
  if (nick.length > maxNickLength || nick.length < minNickLength) { 
    wasError = true;
    errorMsg = 'nick was either too long or too short';
  }
  //if there was an error
  if (wasError) {
    log("setting nick failed: " + errorMsg);
    store.dispatch(setNickSetFailedReason(errorMsg)); //tell the UI setting of the nick failed
  }
  //if there was no error
  else {
    socket.emit('set nick', nick, responseHandler);
  }
}

export default setNick;
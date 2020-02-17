import { store } from '../../stores/store';
import { socket } from './client';
import {
  setCurrentChannel,
  joinChannel,
  addMessage
} from '../../actions/actions';
import { requestUserList } from './handleUserLists';
import { getNowTimestamp } from '../utils'
import { systemNick } from '../../config';
import log from '../log'

//send request to join a channel
const requestJoinChannel = (channelId) => {
  
  //check ID is a number
  if (isNaN(channelId)) {
    //tell the UI joining the channel failed
    log("requesting to join channel ID failed: not a valid ID");
  }
  //if there was no error
  else {
    log("Requesting to join channel ID: " + channelId);
    socket.emit('join channel', channelId, ({ response, channelId }) => { //send the nick to the server
      //handle the response (a string; either "success" or the reason the channel wasn't joined eg. not allowed)
      if (response == "success" || response == "already in channel") {
        log("request to join channel ID " + channelId + " succeeded!");
        store.dispatch(joinChannel(channelId)); //set the UI to show the channel as joined
        store.dispatch(setCurrentChannel(channelId));
        requestUserList();
      } else {
        log("request to join channel ID " + channelId + " failed: " + response);
        //show an error
        store.dispatch(addMessage({source: systemNick, channelId: channelId, messageSent: true, receivedTimestamp: getNowTimestamp(), messageText: "Could not join channel: " + response }));
        store.dispatch(setCurrentChannel(channelId));
      }      
    });
  }
}

export default requestJoinChannel;